#include"RTPSO.h"


RTPSO::RTPSO()
{
}

RTPSO::~RTPSO()
{
}

void RTPSO::Init(ComPtr<ID3D12Device5> creationdevice)
{
	//at least pipeline config state should be provided for opimization.
	assert(m_statesubobjects.size() != 0);
	m_desc.Type = D3D12_STATE_OBJECT_TYPE::D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	m_desc.NumSubobjects = static_cast<UINT>(m_statesubobjects.size());
	m_desc.pSubobjects = m_statesubobjects.data();
	DXASSERT(creationdevice->CreateStateObject(&m_desc, IID_PPV_ARGS(m_stateobject.GetAddressOf())))
	
}

void RTPSO::SetPipelineConfig(UINT maxtracerecursiondepth)
{
	m_rtconfig.MaxTraceRecursionDepth = maxtracerecursiondepth;
	D3D12_STATE_SUBOBJECT subobj = {};
	subobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	subobj.pDesc = &m_rtconfig;
	m_statesubobjects.push_back(subobj);
}