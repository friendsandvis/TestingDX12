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
void RTPSO::SetShader(DX12Shader* shader, wstring hlslentry, wstring uniquename)
{
	m_shaderstouse.push_back(RTPSOShader());
	RTPSOShader& rtshaderunit = m_shaderstouse[(m_shaderstouse.size()-1)];
	rtshaderunit.Init(shader, hlslentry, uniquename);
	D3D12_STATE_SUBOBJECT shadersubobj = {};
	shadersubobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	shadersubobj.pDesc = &rtshaderunit.GetDXILLIBDESC();
	m_statesubobjects.push_back(shadersubobj);

}
RTPSOShader::RTPSOShader()
	:m_shader(nullptr)
{

}
RTPSOShader::~RTPSOShader()
{
	//shader is dynamically allocated.
	if (m_shader)
	{
		delete m_shader;
	}
}

void RTPSOShader::Init(DX12Shader* shader, wstring hlslentry, wstring uniquename)
{
	m_uniquename = uniquename;
	m_hlslentrypoint = hlslentry;
	m_exportdesc.Flags = D3D12_EXPORT_FLAGS::D3D12_EXPORT_FLAG_NONE;
	m_exportdesc.Name = m_uniquename.c_str();
	m_exportdesc.ExportToRename = m_hlslentrypoint.c_str();

	m_dxillibdesc.NumExports = 1;
	m_dxillibdesc.pExports = &m_exportdesc;
	m_dxillibdesc.DXILLibrary.BytecodeLength = shader->GetCompiledCodeSize();
	m_dxillibdesc.DXILLibrary.pShaderBytecode = shader->GetCompiledCode();
}