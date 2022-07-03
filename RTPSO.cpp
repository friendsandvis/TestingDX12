#include"RTPSO.h"


RTPSO::RTPSO()
{
}

RTPSO::~RTPSO()
{
	//delete rtpso shaders if any.
	for (size_t i = 0; i < m_shaderstouse.size(); i++)
	{
		delete m_shaderstouse[i];
	}
	//delete subobject descs if any
	for (size_t i = 0; i < m_subobjectdescs.size(); i++)
	{
		delete[] m_subobjectdescs[i];
	}
	// delete export association if any
	for (size_t i = 0; i < m_associationsused.size(); i++)
	{
		delete m_associationsused[i];
	}
}

void RTPSO::Init(ComPtr<ID3D12Device5> creationdevice)
{
	//at least pipeline config state should be provided for opimization.
	assert(m_statesubobjects.size() != 0);
	m_desc.Type = D3D12_STATE_OBJECT_TYPE::D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	m_desc.NumSubobjects = static_cast<UINT>(m_statesubobjects.size());
	m_desc.pSubobjects = m_statesubobjects.data();
	DXASSERT(creationdevice->CreateStateObject(&m_desc, IID_PPV_ARGS(m_stateobject.GetAddressOf())))
	DXASSERT(m_stateobject->QueryInterface(IID_PPV_ARGS(m_stateobjectprops.GetAddressOf())))
	
		//retrive shader identifiers for all shaders in state object.
		for (size_t i = 0; i < m_shaderstouse.size(); i++)
		{
			if (!m_shaderstouse[i]->IsShaderIdentifierRetriveable())
			{
				continue;
			}
			wstring shadername=m_shaderstouse[i]->GetUniqueName();
			m_shaderidentifiermap[shadername] = m_stateobjectprops->GetShaderIdentifier(shadername.c_str());
	}
	//retrive hitgroup identifiers for all hit groups  in state object.
	for (size_t i = 0; i < m_hitgroupstouse.size(); i++)
	{
		wstring hitgroupname = m_hitgroupstouse[i];
		m_hitgroupidentifiermap[hitgroupname] = m_stateobjectprops->GetShaderIdentifier(hitgroupname.c_str());
	}
}
void* RTPSO::GetIdentifier(wstring name, bool ishitgroup)
{
	if (ishitgroup)
	{
		return m_hitgroupidentifiermap[name];
	}
	return m_shaderidentifiermap[name];
}
void RTPSO::AddHitGroup(D3D12_HIT_GROUP_DESC& desc)
{
	uint8_t* descdata = new uint8_t[sizeof(D3D12_HIT_GROUP_DESC)];
	m_subobjectdescs.push_back(descdata);
	D3D12_HIT_GROUP_DESC * hitgroupdesc=reinterpret_cast<D3D12_HIT_GROUP_DESC*>(descdata);
	(*hitgroupdesc) = desc;
	D3D12_STATE_SUBOBJECT hitgroupobj = {};
	hitgroupobj.pDesc = hitgroupdesc;
	hitgroupobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
	m_statesubobjects.push_back(hitgroupobj);
	m_hitgroupstouse.push_back(desc.HitGroupExport);
}
void RTPSO::AddShaderConfig(D3D12_RAYTRACING_SHADER_CONFIG shaderconfigdesc, string name)
{
	uint8_t* descdata = new uint8_t[sizeof(D3D12_RAYTRACING_SHADER_CONFIG)];
	m_subobjectdescs.push_back(descdata);
	D3D12_RAYTRACING_SHADER_CONFIG* shaderconfig = reinterpret_cast<D3D12_RAYTRACING_SHADER_CONFIG*>(descdata);
	(*shaderconfig) = shaderconfigdesc;
	D3D12_STATE_SUBOBJECT shaderconfigsubobj = {};
	shaderconfigsubobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
	shaderconfigsubobj.pDesc = shaderconfig;
	m_statesubobjects.push_back(shaderconfigsubobj);
	m_shaderconfigmap[name] = static_cast<UINT>(m_statesubobjects.size() - 1);


	
}
void RTPSO::AddShaderConfigAssociation(vector<LPCWSTR> exportstoassociateto,string shaderconfigtoassociate)
{
	const UINT configsubobjectidx = m_shaderconfigmap[shaderconfigtoassociate];
	
	ExportAssociation* exportassociation=new ExportAssociation();
	m_associationsused.push_back(exportassociation);
	
	D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION associationdesc = {};
	associationdesc.NumExports = (UINT)exportstoassociateto.size();
	associationdesc.pExports = exportstoassociateto.data();
	
	exportassociation->Init(exportstoassociateto, &m_statesubobjects[configsubobjectidx]);
	D3D12_STATE_SUBOBJECT associationsubobject = {};
	exportassociation->PrepareSubObject(associationsubobject);
	m_statesubobjects.push_back(associationsubobject);
	
}
void RTPSO::AddLocalRootSignature(DX12RootSignature& rootsignature)
{
	D3D12_STATE_SUBOBJECT localrootsigobj = {};
	localrootsigobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
	
	
}
//rootsignature arg passed must be initialized.
void RTPSO::SetGlobalRootSignature(DX12RootSignature& rootsignature)
{
	m_globalrootsignature = rootsignature;
	m_globalrootsignaturedesc.pGlobalRootSignature = m_globalrootsignature.GetRootSignature();
	D3D12_STATE_SUBOBJECT globalrootsigobj = {};
	globalrootsigobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
	globalrootsigobj.pDesc = &m_globalrootsignaturedesc;
	m_statesubobjects.push_back(globalrootsigobj);
}

void RTPSO::SetPipelineConfig(UINT maxtracerecursiondepth)
{
	m_rtconfig.MaxTraceRecursionDepth = maxtracerecursiondepth;
	D3D12_STATE_SUBOBJECT subobj = {};
	subobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	subobj.pDesc = &m_rtconfig;
	m_statesubobjects.push_back(subobj);
}
void RTPSO::AddShader(DX12Shader* shader, wstring hlslentry, wstring uniquename,RTPSOSHADERTYPE shadertype)
{
	RTPSOShader* rtshaderunit = new RTPSOShader();
	m_shaderstouse.push_back(rtshaderunit);
	
	rtshaderunit->Init(shader, hlslentry, uniquename,shadertype);
	D3D12_STATE_SUBOBJECT shadersubobj = {};
	shadersubobj.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
	shadersubobj.pDesc = &rtshaderunit->GetDXILLIBDESC();
	m_statesubobjects.push_back(shadersubobj);

}
RTPSOShader::RTPSOShader()
	:m_shader(nullptr)
{

}
RTPSOShader::~RTPSOShader()
{
	//shader is dynamically allocated(externally).
	if (m_shader)
	{
		delete m_shader;
	}
}
ExportAssociation::ExportAssociation()
{
}

ExportAssociation::~ExportAssociation()
{
}
void ExportAssociation::Init(vector<LPCWSTR> exportstoassociateto, const D3D12_STATE_SUBOBJECT* subobjecttoassociate)
{
	m_exportstoassociateto = exportstoassociateto;
	m_desc.NumExports = static_cast<UINT>(m_exportstoassociateto.size());
	m_desc.pExports = m_exportstoassociateto.data();
	m_desc.pSubobjectToAssociate = subobjecttoassociate;
}
void ExportAssociation::PrepareSubObject(D3D12_STATE_SUBOBJECT& outassociationsubobject)
{
	outassociationsubobject.Type = D3D12_STATE_SUBOBJECT_TYPE::D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
	outassociationsubobject.pDesc = &m_desc;
}
bool RTPSOShader::IsShaderIdentifierRetriveable()
{
	bool result = (m_type!=RTPSOSHADERTYPE::CLOSESTHIT);
	return result;
}
void RTPSOShader::Init(DX12Shader* shader, wstring hlslentry, wstring uniquename, RTPSOSHADERTYPE shadertype)
{
	m_type = shadertype;
	m_shader = shader;
	m_uniquename = uniquename;
	m_hlslentrypoint = hlslentry;
	m_exportdesc.Flags = D3D12_EXPORT_FLAGS::D3D12_EXPORT_FLAG_NONE;
	m_exportdesc.Name = m_uniquename.c_str();
	m_exportdesc.ExportToRename = m_hlslentrypoint.c_str();
	//m_exportdesc.Flags = D3D12_EXPORT_FLAGS::D3D12_EXPORT_FLAG_NONE;

	m_dxillibdesc.NumExports = 1;
	m_dxillibdesc.pExports = &m_exportdesc;
	m_dxillibdesc.DXILLibrary.BytecodeLength = m_shader->GetCompiledCodeSize();
	m_dxillibdesc.DXILLibrary.pShaderBytecode = m_shader->GetCompiledCode();
	
}