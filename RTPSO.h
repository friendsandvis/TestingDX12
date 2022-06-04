#pragma once
#include"DXUtils.h"
#include"ShaderManager.h"
#include<map>

//a collective unit to repesent a shader used in rtpso as a dxil lib.
class RTPSOShader
{
public:

	RTPSOShader();
	~RTPSOShader();
	
	void Init(DX12Shader* shader, wstring hlslentry, wstring uniquename);
	D3D12_DXIL_LIBRARY_DESC& GetDXILLIBDESC() { return m_dxillibdesc; }
	wstring GetUniqueName() { return m_uniquename; }

private:
	D3D12_DXIL_LIBRARY_DESC m_dxillibdesc;
	D3D12_EXPORT_DESC m_exportdesc;
	DX12Shader* m_shader;
	wstring m_hlslentrypoint, m_uniquename;
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
	void AddLocalRootSignature();
	void AddShaderConfigAssociation(vector<LPCWSTR> exportstoassociateto, string shaderconfigtoassociate);
	void AddShaderConfig(D3D12_RAYTRACING_SHADER_CONFIG shaderconfigdesc, string name);
	void AddHitGroup(D3D12_HIT_GROUP_DESC& desc);
	void Init(ComPtr<ID3D12Device5> creationdevice);
	void SetPipelineConfig(UINT maxtracerecursiondepth=1);
	void AddShader(DX12Shader* shader, wstring hlslentry, wstring uniquename);
private:
	ComPtr<ID3D12StateObject> m_stateobject;
	ComPtr<ID3D12StateObjectProperties> m_stateobjectprops;
	D3D12_STATE_OBJECT_DESC m_desc;
	vector<D3D12_STATE_SUBOBJECT> m_statesubobjects;
	//only 1 rt config for a pipeline(many does not makes any sense so keep this desc a direct member variable.
	D3D12_RAYTRACING_PIPELINE_CONFIG m_rtconfig;
	vector< RTPSOShader*> m_shaderstouse;
	vector< ExportAssociation> m_associationsused;
	//descs needed for diffrent subobjects e diffrent so we dynamically allocate them(as byte array)
	vector< uint8_t*> m_subobjectdescs;
	map<wstring, void*> m_shaderidentifiermap;
	//map for easily finding shader config subobj in the subobject vector by index(map pairs shaderconfig name to the congig's subobject index in the vector
	map<std::string, UINT> m_shaderconfigmap;
};

