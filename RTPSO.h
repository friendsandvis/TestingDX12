#pragma once
#include"DXUtils.h"
#include"ShaderManager.h"
#include<map>
#include"DX12RootSignature.h"

//a collective unit to repesent a shader used in rtpso as a dxil lib.
enum RTPSOSHADERTYPE
{
	CLOSESTHIT,
	RAYGEN,
	MISS
};
class RTPSOShader
{
public:

	RTPSOShader();
	~RTPSOShader();
	bool IsShaderIdentifierRetriveable();
	void Init(DX12Shader* shader, wstring hlslentry, wstring uniquename, RTPSOSHADERTYPE shadertype);
	D3D12_DXIL_LIBRARY_DESC& GetDXILLIBDESC() { return m_dxillibdesc; }
	RTPSOSHADERTYPE GetShadertype() { return m_type; }
	wstring GetUniqueName() { return m_uniquename; }

private:
	D3D12_DXIL_LIBRARY_DESC m_dxillibdesc;
	D3D12_EXPORT_DESC m_exportdesc;
	DX12Shader* m_shader;
	wstring m_hlslentrypoint, m_uniquename;
	RTPSOSHADERTYPE m_type;
};
//incapsulates all the needful for representing subobject to exports association.
class ExportAssociation
{
public:
	ExportAssociation();
	~ExportAssociation();
	//warning:this class is not responsible for mentaining the lifecycle of subobjectt to associate obj.
	void Init(vector<LPCWSTR> exportstoassociateto,const D3D12_STATE_SUBOBJECT* subobjecttoassociate);
	void PrepareSubObject(D3D12_STATE_SUBOBJECT& outassociationsubobject);

private:
	vector<LPCWSTR> m_exportstoassociateto;
	D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION m_desc;
};



class RTPSO
{
public:
	RTPSO();
	~RTPSO();
	void AddLocalRootSignature(DX12RootSignature& rootsignature);
	void SetGlobalRootSignature(DX12RootSignature& rootsignature);
	void AddShaderConfigAssociation(vector<LPCWSTR> exportstoassociateto, string shaderconfigtoassociate);
	void AddShaderConfig(D3D12_RAYTRACING_SHADER_CONFIG shaderconfigdesc, string name);
	void AddHitGroup(D3D12_HIT_GROUP_DESC& desc);
	void Init(ComPtr<ID3D12Device5> creationdevice);
	//identifier can be for hitgroupname or shader name
	void* GetIdentifier(wstring name,bool ishitgroup=false);
	void SetPipelineConfig(UINT maxtracerecursiondepth=1);
	void AddShader(DX12Shader* shader, wstring hlslentry, wstring uniquename, RTPSOSHADERTYPE shadertype);
	ID3D12RootSignature* GetGlobalRootSignature() { return m_globalrootsignature.GetRootSignature(); }
	ID3D12StateObject* GetPipelineStateObject() { return m_stateobject.Get(); }
private:
	DX12RootSignature m_globalrootsignature;
	//only 1 global root sig per rtpso
	D3D12_GLOBAL_ROOT_SIGNATURE m_globalrootsignaturedesc;
	ComPtr<ID3D12StateObject> m_stateobject;
	ComPtr<ID3D12StateObjectProperties> m_stateobjectprops;
	D3D12_STATE_OBJECT_DESC m_desc;
	vector<D3D12_STATE_SUBOBJECT> m_statesubobjects;
	//only 1 rt config for a pipeline(many does not makes any sense so keep this desc a direct member variable.
	D3D12_RAYTRACING_PIPELINE_CONFIG m_rtconfig;
	vector< RTPSOShader*> m_shaderstouse;
	vector<wstring> m_hitgroupstouse;
	//association objects are created in heap for use.
	vector< ExportAssociation*> m_associationsused;
	//descs needed for diffrent subobjects e diffrent so we dynamically allocate them(as byte array)
	vector< uint8_t*> m_subobjectdescs;
	map<wstring, void*> m_shaderidentifiermap;
	map<wstring, void*> m_hitgroupidentifiermap;
	//map for easily finding shader config subobj in the subobject vector by index(map pairs shaderconfig name to the congig's subobject index in the vector
	map<std::string, UINT> m_shaderconfigmap;
};

