#include"SamplerFeedbackManager.h"



DX12FeedBackUnit::DX12FeedBackUnit()
{
}

DX12FeedBackUnit::~DX12FeedBackUnit()
{
}

void DX12FeedBackUnit::Init(ComPtr<ID3D12Device8> creationdevice, samplerFeedbackUnitInitData initdata)
{
	//only minmip type feedback supported as we are implementing streaming for target texture.
	assert(initdata.isMinMipMap);
	
	D3D12_RESOURCE_DESC pairedresourcedesc=initdata.feedbacktexrestopairwith->GetResource()->GetDesc();
	SamplerFeedbackTextureInitData feedbacktexturedata = {};
	feedbacktexturedata.feedbacktexdesc.Alignment=0;
	feedbacktexturedata.feedbacktexdesc.DepthOrArraySize = pairedresourcedesc.DepthOrArraySize;
	feedbacktexturedata.feedbacktexdesc.MipLevels = pairedresourcedesc.MipLevels;
	feedbacktexturedata.feedbacktexdesc.Width = pairedresourcedesc.Width;
	feedbacktexturedata.feedbacktexdesc.Height = pairedresourcedesc.Height;
	feedbacktexturedata.feedbacktexdesc.SampleDesc.Count = 1;
	feedbacktexturedata.feedbacktexdesc.SampleDesc.Quality=0;
	if (initdata.isMinMipMap)
	{
		feedbacktexturedata.feedbacktexdesc.Format = DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
	}
	else
	{
		feedbacktexturedata.feedbacktexdesc.Format = DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;
	}
	feedbacktexturedata.feedbacktexdesc.SamplerFeedbackMipRegion = initdata.feedbackmipregion;
	feedbacktexturedata.feedbacktexdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	feedbacktexturedata.feedbacktexdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	feedbacktexturedata.feedbacktexdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	feedbacktexturedata.feedbacktexheapflags = D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE;
	feedbacktexturedata.feedbacktexheapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
	feedbacktexturedata.feedbacktexheapprops.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	feedbacktexturedata.feedbacktexheapprops.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//init feedback texture first
	m_feedbacktex.Init(creationdevice, feedbacktexturedata);
	m_feedbacktex.SetName(L"Feedbacktexture");
	//pair the feedback texture to create uav
	m_feedbacktex.Pair(creationdevice, initdata.feedbacktexrestopairwith, initdata.feedbacktexuavhandle);
	ComPtr<ID3D12Device> creationdevice1;
	DXASSERT(creationdevice.As(&creationdevice1))
	//init readbackbuffer
	DX12ResourceCreationProperties readbackbufferproperties;
	DX12Buffer::InitResourceCreationProperties(readbackbufferproperties);
	readbackbufferproperties.resheapprop.Type = D3D12_HEAP_TYPE_READBACK;
	readbackbufferproperties.resdesc.Width = m_feedbacktex.GetRequiredBufferSizeForTranscodeing();
	m_feedbackreadbackbuffer.Init(creationdevice1, readbackbufferproperties, ResourceCreationMode::COMMITED);
	m_feedbackreadbackbuffer.SetName(L"ReadbackresolveBuffer");
	//residency map intermidate buffer props are supposed  to be same as readback buffer for now.
	intermidiateresidencymapbuffer.Init(creationdevice1, readbackbufferproperties, ResourceCreationMode::COMMITED);
	intermidiateresidencymapbuffer.SetName(L"ResidencymapIntermidiateBuffer");
	{
		DX12ResourceCreationProperties readbackresolvetexprops;
		DX12TextureSimple::InitResourceCreationProperties(readbackresolvetexprops);
		readbackresolvetexprops.resdesc.Format = DXGI_FORMAT_R8_UINT;
		readbackresolvetexprops.resdesc.Height = m_feedbacktex.GetRequiredTextureHeightForTranscodeing();
		readbackresolvetexprops.resdesc.Width = m_feedbacktex.GetRequiredTextureWidthForTranscodeing();
		readbackresolvetexprops.resdesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		//init feedback resolve tex as well
		m_feedbackresolvedtex.Init(creationdevice1, readbackresolvetexprops, ResourceCreationMode::COMMITED);
		m_feedbackresolvedtex.SetName(L"ReadbackresolveTex");

		//init residency map it is similar in properties to resolved feedback texture;in here it's exactly same in props.
		m_residencymap.Init(creationdevice, readbackresolvetexprops, ResourceCreationMode::COMMITED);
		m_residencymap.SetName(L"ResidencyMap");
	}

	//init physical memory manager
	
	m_reservedresmemorymanager.Init(creationdevice,initdata.feedbacktexrestopairwith);

	//init the uav heaps if needed
	if(initdata.allowreservedresourceuavclear)
	{
		D3D12_DESCRIPTOR_HEAP_DESC uavheapdesc = {};
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		uavheapdesc.NodeMask = 0;
		uavheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		//1 uav per mip
		uavheapdesc.NumDescriptors = initdata.feedbacktexrestopairwith->GetTotalMipCount();
		m_uavheap.Init(uavheapdesc, creationdevice);
		uavheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_uavheapupload.Init(uavheapdesc, creationdevice);
		for (unsigned i = 0; i < uavheapdesc.NumDescriptors; i++)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE uavhandle=m_uavheapupload.GetCPUHandleOffseted(i);
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
			uavdesc.Texture2D.MipSlice = i;
			uavdesc.Texture2D.PlaneSlice = 0;
			uavdesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			initdata.feedbacktexrestopairwith->CreateUAV(creationdevice, uavdesc, uavhandle);
		}
		creationdevice->CopyDescriptorsSimple(uavheapdesc.NumDescriptors, m_uavheap.GetCPUHandlefromstart(), m_uavheapupload.GetCPUHandlefromstart(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	
}
void DX12FeedBackUnit::CreateFeedbackResolveTexUAV(ComPtr< ID3D12Device> creationdevice,D3D12_CPU_DESCRIPTOR_HANDLE cpudeschandle)
{
	
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
	uavdesc.Texture2D.MipSlice = 0;
	uavdesc.Texture2D.PlaneSlice = 0;
	uavdesc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D;
	m_feedbackresolvedtex.CreateUAV(creationdevice, uavdesc, cpudeschandle);
}

void DX12FeedBackUnit::Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist)
{
	//first transcode in a buffer the feedback data
	m_feedbacktex.Readback(commandlist, &m_feedbackreadbackbuffer);
	//next transcode in a texture the feedback data
	m_feedbacktex.Readback(commandlist, &m_feedbackresolvedtex);
	D3D12_RESOURCE_BARRIER barrier = {};
	//after transcoding in a texture copy it to the residency map as well for furthur processing of residency map.
	if (m_feedbackresolvedtex.GetCurrentResourceState() != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE)
	{
		D3D12_RESOURCE_BARRIER barrier = m_feedbackresolvedtex.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandlist->ResourceBarrier(1, &barrier);
	}
	
	
		
	
	if (m_residencymap.GetCurrentResourceState() != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST)
	{
		barrier = m_residencymap.TransitionResState(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
		commandlist->ResourceBarrier(1, &barrier);
	}

	commandlist->CopyResource(m_residencymap.GetResource().Get(), m_feedbackresolvedtex.GetResource().Get());
	
}

void DX12FeedBackUnit::ProcessReadbackdata()
{
	
	//get the readback data into a cpu buffer to process.
	BufferMapParams mapparams = {};
	//read full buffer
	mapparams.range.Begin = 0;
	mapparams.range.End=m_feedbackreadbackbuffer.GetSize();
	uint8_t* readbackdata = reinterpret_cast<uint8_t*>(m_feedbackreadbackbuffer.Map(mapparams));
	Set<uint8_t> currentmipssampled;

	//since mapped data is uint8_t(size==1 byte) so num elements is the same as mapped region length
	for (size_t i = 0; i < mapparams.range.End; i++)
	{
		//each entry is a mip level used.
		uint8_t miplevelsampled = readbackdata[i];
		
		if (miplevelsampled != 0xFF)
		{
			//if(currentmipssampled.GetSize())
			//assert(miplevelsampled == currentmipssampled.GetDataptr()[0]);
			//some mip was sampled(0xff is clear value in feedback.
			currentmipssampled.PushUnique(miplevelsampled);
		}

	}
	//logic specifically ment to reduce the scope of sfs by testing in condition were a single mip level is sampledd through out the texture(hence avoiding the need of a residency map;mip clamp can be handled by a simple constant.
	{
		//if simplified case is to work then only 1 mip level must have been sampled.
		//assert(currentmipssampled.GetSize() == 1);
		
		
		uint8_t expectedmiplevelclamp=currentmipssampled.GetDataptr()[0];
		//in this simplified implementation single mip level is expected to be sampled throughout so that is expected to be the lod clamp directly(the residency of the mip itself is verified in the update step to avoid any glitches).
		m_lodclampvalue = expectedmiplevelclamp;
	}
	Set<uint8_t> mipstobemapped;
	//doing very specicialized thing  here(retrive from sampled mips set  a set of mips that should be resident)
	Set<uint8_t> mipstoberesident;
	{
		//remember if minlod feedback map returns x mip as sampled then it means x till the last mip must be resident.
		//first find the most detailed(smallest) mip level of the sampledmips
		uint8_t minmipvaluesampled = GetMinimunMipValue(currentmipssampled.GetDataptr(), currentmipssampled.GetSize());
		//use it to get all resident mips
		for (; minmipvaluesampled < m_reservedresmemorymanager.GetSubResCount(); minmipvaluesampled++)
		{
			mipstoberesident.Push(minmipvaluesampled);
		}
	}
	Set<uint8_t> mipstobeunmapped = m_currentlymappedmips.Minus(mipstoberesident);
	Set<uint8_t> mipsexpectedtobemapped = currentmipssampled.Minus(m_currentlymappedmips);
	if (mipsexpectedtobemapped.GetSize() != 0)
	{
		size_t mostdetailedexpectedmip = GetMinimunMipValue(mipsexpectedtobemapped.GetDataptr(),mipsexpectedtobemapped.GetSize());
		
		for (size_t i = 0; i < mipsexpectedtobemapped.GetSize(); i++)
		{
			uint8_t expectedmip = mipsexpectedtobemapped.GetDataptr()[i];
			if (expectedmip < mostdetailedexpectedmip)
			{
				mostdetailedexpectedmip = expectedmip;
			}


		}
		/*for (uint8_t i = mostdetailedexpectedmip; i < m_reservedresmemorymanager.GetSubResCount(); i++)
		{
				
				mipstobemapped.Push(i);
		}*/
		//map mips needed to be mapped
		m_reservedresmemorymanager.BindMemory2(mostdetailedexpectedmip);
		for (size_t i = 0; i < mipstoberesident.GetSize(); i++)
		{
			m_currentlymappedmips.PushUnique(mipstoberesident.GetDataptr()[i]);
		}
	}

	
	{
		
		/*uint8_t* mipstomap = mipstobemapped.GetDataptr();

		for (size_t i = 0; i < mipstobemapped.GetSize(); i++)
		{
			if (m_reservedresmemorymanager.IsMemoryBound(mipstomap[i]))
			{
				continue;
			}
			m_reservedresmemorymanager.BindMemory(mipstomap[i]);
			m_currentlymappedmips.PushUnique(mipstomap[i]);
			//also try update mip clamp.
			//TryUpdateLODClamp_MipLoaded(mipstomap[i]);
		}*/
	}
	{
		//unmap mips needed to be unmapped
		uint8_t* mipstounmap = mipstobeunmapped.GetDataptr();
		
		for (size_t i = 0; i < mipstobeunmapped.GetSize(); i++)
		{
			
			assert(m_reservedresmemorymanager.IsMemoryBound(mipstounmap[i]));
			bool unbindissued=m_reservedresmemorymanager.UnbindMemory(mipstounmap[i]);
			if (unbindissued)
			{
				m_currentlymappedmips.Remove(mipstounmap[i]);
			}
		}
	}

	//writing nothing
	mapparams.range.End = 0;
	m_feedbackreadbackbuffer.UnMap(mapparams);
}
uint8_t DX12FeedBackUnit::GetMinimunMipValue(uint8_t* values,size_t nummipvalues)
{
	assert(nummipvalues != 0);
	uint8_t minvalue = values[0];

	for (size_t t= 1; t < nummipvalues; t++)
	{
		if (values[t] < minvalue)
		{
			minvalue = values[t];
		}
	}
	return minvalue;
}
void DX12FeedBackUnit::TryUpdateLODClamp_MipLoaded(unsigned loadedlodidx)
{
	//special case where the current lod clamp is not bound(eg at starting) then bluindly update lodclamp to the new loaded mip level
	if (m_reservedresmemorymanager.IsMemoryBound(m_lodclampvalue))
	{
		if (m_lodclampvalue > loadedlodidx) { m_lodclampvalue=loadedlodidx; }
	}
	else
	{
		m_lodclampvalue = loadedlodidx;
	}
}
void DX12FeedBackUnit::VerifyLodClamp()
{
	//if the lod clamp value is a mip of reserved res texture that is bound then proceed safely nothing to do.
	if (m_reservedresmemorymanager.IsMemoryBound(m_lodclampvalue))
	{
		return;
	}
	//otherwise find the most detailed mip that is bound so that we can clamp to it.
	m_lodclampvalue=m_reservedresmemorymanager.GetMostDetailedMappedMipIndex();

}

void DX12FeedBackUnit::ClearReservedResourceMip(DX12Commandlist& cmdlist, uint8_t mipindextoclear, float* clearcolour)
{
	DX12ReservedResource* reservedrestex = m_reservedresmemorymanager.GetReservedresource();
	assert(mipindextoclear<reservedrestex->GetTotalMipCount());
	D3D12_CPU_DESCRIPTOR_HANDLE cpuhandle = m_uavheapupload.GetCPUHandleOffseted(mipindextoclear);
	D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = m_uavheap.GetGPUHandleOffseted(mipindextoclear);
	ID3D12DescriptorHeap* heapstoset[1] = {m_uavheap.GetDescHeap()};
	cmdlist->SetDescriptorHeaps(1, heapstoset);
	cmdlist->ClearUnorderedAccessViewFloat(gpuhandle,cpuhandle , reservedrestex->GetResource().Get(), clearcolour, 0, nullptr);
}
void DX12FeedBackUnit::AllClearReservedResourceMip(DX12Commandlist& cmdlist)
{
	float clearcolour[4] = {1.0f,0.0f,0.0f,1.0f};
	ID3D12DescriptorHeap* heapstoset[1] = { m_uavheap.GetDescHeap() };
	cmdlist->SetDescriptorHeaps(1, heapstoset);
	DX12ReservedResource* reservedrestex = m_reservedresmemorymanager.GetReservedresource();
	for (size_t mipindextoclear = 0; mipindextoclear < reservedrestex->GetTotalMipCount(); mipindextoclear++)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuhandle = m_uavheapupload.GetCPUHandleOffseted(mipindextoclear);
		D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = m_uavheap.GetGPUHandleOffseted(mipindextoclear);

		cmdlist->ClearUnorderedAccessViewFloat(gpuhandle, cpuhandle, reservedrestex->GetResource().Get(), clearcolour, 0, nullptr);
	}
}

void DX12FeedBackUnit::BindMipLevel(uint8_t mipleveltobind, bool makeunmapable)
{
	bool valfound;
	m_currentlymappedmips.Find(mipleveltobind,valfound);
	if (valfound)
	{
		//already maped
		return;
	}
	assert(!m_reservedresmemorymanager.IsMemoryBound(mipleveltobind));
	m_reservedresmemorymanager.BindMemory(mipleveltobind,makeunmapable);
	m_currentlymappedmips.PushUnique(mipleveltobind);
	//update clamp value
	TryUpdateLODClamp_MipLoaded(mipleveltobind);
}

void DX12FeedBackUnit::InitReedbackBuffer()
{
	DX12ResourceCreationProperties readbackbufferprops;
	DX12Resource::InitResourceCreationProperties(readbackbufferprops);
	readbackbufferprops.resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	readbackbufferprops.resdesc.Width = m_feedbacktex.GetRequiredBufferSizeForTranscodeing();
	readbackbufferprops.resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	readbackbufferprops.resheapprop.Type = D3D12_HEAP_TYPE_READBACK;
}


SamplerFeedbackTexture::SamplerFeedbackTexture()
	:m_pairedres(nullptr)
{}
SamplerFeedbackTexture::~SamplerFeedbackTexture()
{}

UINT64 SamplerFeedbackTexture::GetRequiredBufferSizeForTranscodeing()
{
	//make sure resource is paired 
	assert(m_pairedres!=nullptr);
	UINT64 buffersize = (UINT64)ceil(m_resdesc.Height / (double)m_resdesc.SamplerFeedbackMipRegion.Height);
	buffersize *= ((UINT64)ceil(m_resdesc.Width / (double)m_resdesc.SamplerFeedbackMipRegion.Width));
	return buffersize;

}

UINT64 SamplerFeedbackTexture::GetRequiredTextureWidthForTranscodeing()
{
	UINT64 width = (UINT64)ceil(m_resdesc.Width / (double)m_resdesc.SamplerFeedbackMipRegion.Width);
	return width;
}
UINT64 SamplerFeedbackTexture::GetRequiredTextureHeightForTranscodeing()
{
	UINT64 height = (UINT64)ceil(m_resdesc.Height / (double)m_resdesc.SamplerFeedbackMipRegion.Height);
	return height;
}

void SamplerFeedbackTexture::Readback(ComPtr<ID3D12GraphicsCommandList1> commandlist, DX12ResourceBase* dstres)
{
	
	ComPtr<ID3D12Resource> dstresource = dstres->GetResource();
	if (!dstres->IsResState(D3D12_RESOURCE_STATE_RESOLVE_DEST))
	{
		D3D12_RESOURCE_BARRIER abarrier = dstres->TransitionResState(D3D12_RESOURCE_STATE_RESOLVE_DEST);
		commandlist->ResourceBarrier(1, &abarrier);
	}
	if(!IsResourceState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE))
	{
		D3D12_RESOURCE_BARRIER abarrier = TransitionResState(D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
		commandlist->ResourceBarrier(1, &abarrier);
	}
	


	commandlist->ResolveSubresourceRegion(dstresource.Get(), 0, 0, 0, m_resource.Get(), 0, nullptr, DXGI_FORMAT_R8_UINT, D3D12_RESOLVE_MODE_DECODE_SAMPLER_FEEDBACK);
	//transition feedback res back to uav after resolve
	D3D12_RESOURCE_BARRIER abarrier= TransitionResState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandlist->ResourceBarrier(1, &abarrier);
}

void SamplerFeedbackTexture::Init(ComPtr<ID3D12Device8> creationdevice, SamplerFeedbackTextureInitData initdata)
{
	m_resdesc = initdata.feedbacktexdesc;
	DXASSERT(creationdevice->CreateCommittedResource2(&initdata.feedbacktexheapprops, initdata.feedbacktexheapflags,&m_resdesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())))
		m_currentresstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	m_rescreationmode = ResourceCreationMode::COMMITED;
	m_pairedres = nullptr;


}

void SamplerFeedbackTexture::Pair(ComPtr<ID3D12Device8> creationdevice, DX12ResourceBase* restopairwith, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle)
{
	//1 paired res only
	assert(m_pairedres == nullptr);
	m_pairedres = restopairwith;

	//create UAV as well
	creationdevice->CreateSamplerFeedbackUnorderedAccessView(m_pairedres->GetResource().Get(), m_resource.Get(), uavhandle);

}