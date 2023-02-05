#include"DXTexManager.h"
#include"DX12CommandList.h"
#include"StreamableTextureFileReader.h"
#pragma comment(lib,"DirectXTex.lib")



DXTexManager::DXTexManager()
{
}

DXTexManager::~DXTexManager()
{
}

bool DXTexManager::LoadTexture(const wchar_t* imagefile, DXImageData& outloadedImagedata)
{
	HRESULT res= LoadFromDDSFile(imagefile, DDS_FLAGS_ALLOW_LARGE_FILES, &outloadedImagedata.m_imagemetadata, outloadedImagedata.m_image);
	

	return(res == S_OK);
}
shared_ptr<StreamableTextureFileReader> DXTexManager::LoadSTFTexture(const char* imagefile)
{
	shared_ptr<StreamableTextureFileReader> outstfreader = make_shared<StreamableTextureFileReader>();
	int res = outstfreader->Init(imagefile);
	if (res == 1)
	{
		return outstfreader;
	}
	return nullptr;
}



bool DXTexture::Init(ComPtr< ID3D12Device> creationdevice)
{
	m_currentresstate = D3D12_RESOURCE_STATE_COPY_DEST;
	//create texture resource from texdata
	HRESULT res = CreateTexture(creationdevice.Get(), m_texdata.m_imagemetadata, m_resource.GetAddressOf());

	//create the intermidiateuploadbuffer & upload subresources too
	m_texdata.GetSubresData(creationdevice, m_uploadsubresdata);
	//DXASSERT(PrepareUpload(creationdevice.Get(), m_texdata.m_image.GetImages(), m_texdata.m_image.GetImageCount(), m_texdata.m_imagemetadata, m_uploadsubresdata))

	{
		UINT64 uploadbuffersize = GetRequiredIntermediateSize(m_resource.Get(), 0, static_cast<UINT>(m_uploadsubresdata.size()));

		DX12ResourceCreationProperties uploadbuffproperties;
		uploadbuffproperties.resheapflags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;
		uploadbuffproperties.resinitialstate = D3D12_RESOURCE_STATE_COMMON;
		uploadbuffproperties.resheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadbuffproperties.resheapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadbuffproperties.resheapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadbuffproperties.resheapprop.CreationNodeMask = 0;
		uploadbuffproperties.resheapprop.VisibleNodeMask = 0;
		uploadbuffproperties.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadbuffproperties.resdesc.Alignment = 0;
		uploadbuffproperties.resdesc.DepthOrArraySize = 1;
		uploadbuffproperties.resdesc.MipLevels = 1;
		uploadbuffproperties.resdesc.Height = 1;
		uploadbuffproperties.resdesc.Width = uploadbuffersize;
		uploadbuffproperties.resdesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadbuffproperties.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadbuffproperties.resdesc.SampleDesc.Count = 1;
		uploadbuffproperties.resdesc.SampleDesc.Quality = 0;
		uploadbuffproperties.resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		m_uploadbuffer.Init(creationdevice, uploadbuffproperties, ResourceCreationMode::COMMITED);
	}

	return (res == S_OK);
}

void DXTexture::UploadTexture(DX12Commandlist& copycmdlist)
{
	UpdateSubresources(copycmdlist.GetcmdList(), m_resource.Get(), m_uploadbuffer.GetResource().Get(), 0, 0, static_cast<UINT>(m_uploadsubresdata.size()), m_uploadsubresdata.data());
	D3D12_RESOURCE_BARRIER barrier=TransitionResState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	copycmdlist->ResourceBarrier(1, &barrier);
}

void DXTexture::CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle)
{
	creationdevice->CreateShaderResourceView(m_resource.Get(), &srvdesc, srvhandle);
}


void DXTextureCube::Init(ComPtr< ID3D12Device> creationdevice)
{
	//make sure is a cube texture;
	assert(m_texdata.m_imagemetadata.arraySize % 6 == 0);
	D3D12_RESOURCE_DESC resourcedesc = {};
	resourcedesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourcedesc.Alignment = 0;
	resourcedesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourcedesc.Format = m_texdata.m_imagemetadata.format;
	resourcedesc.Height = m_texdata.m_imagemetadata.height;
	resourcedesc.Width = m_texdata.m_imagemetadata.width;
	resourcedesc.DepthOrArraySize = m_texdata.m_imagemetadata.arraySize;
	resourcedesc.MipLevels = m_texdata.m_imagemetadata.mipLevels;
	resourcedesc.SampleDesc.Count = 1;
	resourcedesc.SampleDesc.Quality = 0;

	D3D12_HEAP_PROPERTIES heapprops = {};
	heapprops.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprops.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
	DXASSERT(creationdevice->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE, &resourcedesc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())))
		m_currentresstate = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
	m_texdata.GetSubresData(creationdevice, m_subresdata);
	UINT64 uploadbuffersize = GetRequiredIntermediateSize(m_resource.Get(), 0, static_cast<UINT>(m_subresdata.size()));
	DX12ResourceCreationProperties uploadbufferresprops;
	DX12Buffer::InitResourceCreationProperties(uploadbufferresprops);
	uploadbufferresprops.resdesc.Width = uploadbuffersize;
	m_uploadbuffer.Init(creationdevice, uploadbufferresprops, ResourceCreationMode::COMMITED);
	m_uploadbuffer.SetName(L"Texturecubeuploadbuffer");

}

void DXTextureCube::UploadTexureData(DX12Commandlist& copycmdlist)
{
	
	UpdateSubresources(copycmdlist.GetcmdList(), m_resource.Get(), m_uploadbuffer.GetResource().Get(), 0, 0, static_cast<UINT>(m_subresdata.size()), m_subresdata.data());
	D3D12_RESOURCE_BARRIER barrier = TransitionResState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	copycmdlist->ResourceBarrier(1, &barrier);
}


