#include"DX12Texture.h"
#include"DXTexManager.h"


DX12TextureSimple::DX12TextureSimple()
{

}
DX12TextureSimple::~DX12TextureSimple()
{

}

DX12TextureUploadeHelper::DX12TextureUploadeHelper()
	:m_subresoucecount(0),
	m_subresplacedfootprints(nullptr),
	m_subresrowcount(nullptr),
	m_subresrowsize(nullptr),
	m_targettexture(nullptr)
{}
DX12TextureUploadeHelper::~DX12TextureUploadeHelper()
{
	if (m_subresplacedfootprints)
	{
		delete[] m_subresplacedfootprints;
	}
	if (m_subresrowcount)
	{
		delete[] m_subresrowcount;
	}
	if (m_subresrowsize)
	{
		delete[] m_subresrowsize;
	}
}

void DX12TextureUploadeHelper::PrepareUpload(ComPtr< ID3D12Device> creationdevice,DX12ResourceBase* targettexture)

{
	UINT64 baseoffset = 0;
	UINT64 uploadbuffersize = 0;
	m_targettexture = targettexture;
	D3D12_RESOURCE_DESC targettexdesc=	targettexture->GetResource()->GetDesc();
	m_subresoucecount = targettexdesc.MipLevels * targettexdesc.DepthOrArraySize;
	assert(m_subresplacedfootprints == nullptr);
	m_subresplacedfootprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[m_subresoucecount];
	m_subresrowcount = new UINT[m_subresoucecount];
	m_subresrowsize = new UINT64[m_subresoucecount];

	creationdevice->GetCopyableFootprints(&targettexdesc, 0, m_subresoucecount, baseoffset, m_subresplacedfootprints, m_subresrowcount,m_subresrowsize, &uploadbuffersize);


	//initialize the uploadbuffer
	DX12ResourceCreationProperties uploadbufferproperties;
	DX12Buffer::InitResourceCreationProperties(uploadbufferproperties);
	uploadbufferproperties.resdesc.Width = uploadbuffersize;
	uploadbufferproperties.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	m_uploadbuffer.Init(creationdevice, uploadbufferproperties, ResourceCreationMode::COMMITED);
	m_uploadbuffer.SetName(L"uploadhelperuploadbuffer");



}

void DX12TextureUploadeHelper::Upload(DX12Commandlist& copycmdlist, UINT numsubres, UINT firstsubres)
{
	D3D12_RESOURCE_BARRIER barrier = {};
	if (!m_uploadbuffer.IsResourceState(D3D12_RESOURCE_STATE_COPY_SOURCE))
	{
		 barrier= m_uploadbuffer.TransitionResState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		copycmdlist->ResourceBarrier(1, &barrier);
		
	}

	if (!m_targettexture->IsResourceState(D3D12_RESOURCE_STATE_COPY_DEST))
	{
		barrier = m_targettexture->TransitionResState(D3D12_RESOURCE_STATE_COPY_DEST);
		copycmdlist->ResourceBarrier(1, &barrier);
	}
	UINT numsurestoupload = m_subresoucecount;
	if (numsubres != -1)
	{
		numsurestoupload = numsubres;
	}
	assert(firstsubres < numsurestoupload);
	for (size_t i = firstsubres; i < numsurestoupload; i++)
	{
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		D3D12_TEXTURE_COPY_LOCATION src = {};

		dst.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.pResource = m_targettexture->GetResource().Get();
		dst.SubresourceIndex = i;
		src.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.pResource = m_uploadbuffer.GetResource().Get();
		src.PlacedFootprint = m_subresplacedfootprints[i];

		copycmdlist->CopyTextureRegion(&dst,0,0,0,&src,nullptr);
	}

}

void DX12TextureUploadeHelper::SetUploadTextureData(const DXImageData& imagedata)
{
	//make sure the number of images in scratch image is same as the subresource count of the target teture
	assert(imagedata.m_image.GetImageCount() == m_subresoucecount);

	BufferMapParams buffermapparams = {};
	//nothing to read
	buffermapparams.subresource = 0;
	uint8_t* mappeduploadbuffer = reinterpret_cast<uint8_t*>(m_uploadbuffer.Map(buffermapparams));

	//copy over the subresourcedata to uploadbuffer
	for (size_t i = 0; i < m_subresoucecount; i++)
	{
		
		const Image& subresimage = imagedata.m_image.GetImages()[i];
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subresfootprint =m_subresplacedfootprints[i];
		UINT& subresrowcount = m_subresrowcount[i];
		UINT64 subresrowsize = m_subresrowsize[0];

		//copy data row by row
		UINT64 srcrowsize = subresimage.rowPitch;
		UINT64 dstrowsize = subresfootprint.Footprint.RowPitch;
		uint8_t* dst = mappeduploadbuffer + subresfootprint.Offset;
		uint8_t* src = subresimage.pixels;
		for (size_t i1 = 0; i1 < subresrowcount; i1++)
		{
			
			/*memcpy(dst, src, subresrowsize);
			//increment src and dst to next row
			dst += subresrowsize;
			src = src += subresrowsize;*/
			UINT64 copysize = min(srcrowsize, dstrowsize);
			memcpy(dst, src, copysize);
			//increment src and dst to next row
			dst += dstrowsize;
			src += srcrowsize;
			
		}

	}

	//written to full buffer
	buffermapparams.range.End = m_uploadbuffer.GetSize();
	m_uploadbuffer.UnMap(buffermapparams);
}


DX12TextureUploadHelperSimple::DX12TextureUploadHelperSimple()
	:m_targettexture(nullptr),
	m_subresoucecount(0),
	m_subresplacedfootprints(nullptr),
	m_subresrowsize(nullptr),
	m_subresrowcount(nullptr),
	m_subresdata(nullptr)
{}
DX12TextureUploadHelperSimple::~DX12TextureUploadHelperSimple()
{
	if (m_subresdata)
	{
		delete[]m_subresdata;
	}
	if (m_subresplacedfootprints)
	{
		delete[]m_subresplacedfootprints;
	}
	if (m_subresrowsize)
	{
		delete[]m_subresrowsize;
	}
	if (m_subresrowcount)
	{
		delete[]m_subresrowcount;
	}
}
void DX12TextureUploadHelperSimple::PrepareUpload(ComPtr< ID3D12Device> creationdevice, DX12ResourceBase* targettexture, UINT firstsubres, UINT subrescount)
{
	UINT64 uploadbuffersize=0;
	m_targettexture = targettexture;
	D3D12_RESOURCE_DESC resdesc=m_targettexture->GetResource()->GetDesc();
	m_subresoucecount = subrescount;
	UINT totaltargetsubrescount = resdesc.MipLevels * resdesc.DepthOrArraySize;
	assert(m_subresoucecount <= totaltargetsubrescount && firstsubres<totaltargetsubrescount);
	m_subresplacedfootprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[m_subresoucecount];
	m_subresrowcount = new UINT[m_subresoucecount];
	m_subresrowsize = new UINT64[m_subresoucecount];
	m_subresdata = new vector<uint8_t>[m_subresoucecount];
	creationdevice->GetCopyableFootprints(&resdesc, firstsubres, subrescount, 0, m_subresplacedfootprints, m_subresrowcount, m_subresrowsize, &uploadbuffersize);

	DX12ResourceCreationProperties uploadbufferproperties;
	DX12Buffer::InitResourceCreationProperties(uploadbufferproperties);
	uploadbufferproperties.resdesc.Width = uploadbuffersize;
	uploadbufferproperties.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	m_uploadbuffer.Init(creationdevice, uploadbufferproperties, ResourceCreationMode::COMMITED);
	m_uploadbuffer.SetName(L"uploadhelpersimpleuploadbuffer");

	//init subresource data size
	for (size_t i = 0; i < m_subresoucecount; i++)
	{
		UINT64& subresrowsize = m_subresrowsize[i];
		UINT& subresrowcount = m_subresrowcount[i];
		//resize to expected data size
		m_subresdata[i].resize(subresrowsize * subresrowcount);
	}
}

vector<uint8_t>& DX12TextureUploadHelperSimple::GetSubResourceDataforUpdate(UINT subresIndex)
{
	return m_subresdata[subresIndex];
}

void DX12TextureUploadHelperSimple::SetUploadData()
{
	for (size_t i = 0; i < m_subresoucecount; i++)
	{
		vector<uint8_t>& subresdata = m_subresdata[i];
		for (size_t i1 = 0; i1 < subresdata.size(); i1++)
		{
			subresdata[i1] = 55;
		}
	}
}

void DX12TextureUploadHelperSimple::SetTextureData()
{
	BufferMapParams mapparams = {};
	uint8_t* mappedbuffer = reinterpret_cast<uint8_t*>(m_uploadbuffer.Map(mapparams));
	for (size_t i = 0; i < m_subresoucecount; i++)
	{
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT& subresfootprint = m_subresplacedfootprints[i];
		UINT64& subresrowsize = m_subresrowsize[i];
		UINT& subresrowcount = m_subresrowcount[i];
		vector<uint8_t> subresdata = m_subresdata[i];
		uint8_t* dst = mappedbuffer+subresfootprint.Offset;
		uint8_t* src = subresdata.data();
		for (size_t i1 = 0; i1 < subresrowcount; i1++)
		{
			memcpy(dst, src, subresrowsize);
			src += subresrowsize;
			dst += subresfootprint.Footprint.RowPitch;
		}
	}
	mapparams.range.End = m_uploadbuffer.GetSize();
	m_uploadbuffer.UnMap(mapparams);
}


void DX12TextureUploadHelperSimple::Upload(DX12Commandlist cmdlist)
{
	D3D12_RESOURCE_BARRIER barrier = {};
	if (!m_uploadbuffer.IsResourceState(D3D12_RESOURCE_STATE_COPY_SOURCE))
	{
		barrier=m_uploadbuffer.TransitionResState(D3D12_RESOURCE_STATE_COPY_SOURCE);
		cmdlist->ResourceBarrier(1, &barrier);
	}
	if (!m_targettexture->IsResourceState(D3D12_RESOURCE_STATE_COPY_DEST))
	{
		barrier = m_targettexture->TransitionResState(D3D12_RESOURCE_STATE_COPY_DEST);
		cmdlist->ResourceBarrier(1, &barrier);
	}

	for (size_t i = 0; i < m_subresoucecount; i++)
	{
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.SubresourceIndex = i;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.pResource = m_targettexture->GetResource().Get();

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.PlacedFootprint = m_subresplacedfootprints[i];
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.pResource = m_uploadbuffer.GetResource().Get();

		cmdlist->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	}

	barrier = m_targettexture->TransitionResState(D3D12_RESOURCE_STATE_COMMON);
	cmdlist->ResourceBarrier(1, &barrier);
}