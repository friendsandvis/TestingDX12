#pragma once
#include"DXUtils.h"
#include"ShaderManager.h"

//a collective unit to repesent a shader used in rtpso as a dxil lib.
class RTPSOShader
{
public:

	RTPSOShader();
	~RTPSOShader();
	void Init(DX12Shader* shader, wstring hlslentry, wstring uniquename);
	D3D12_DXIL_LIBRARY_DESC& GetDXILLIBDESC() { return m_dxillibdesc; }

private:
	D3D12_DXIL_LIBRARY_DESC m_dxillibdesc;
	D3D12_EXPORT_DESC m_exportdesc;
	DX12Shader* m_shader;
	wstring m_hlslentrypoint, m_uniquename;
};
class RTPSO
{
public:
	RTPSO();
	~RTPSO();

	void Init(ComPtr<ID3D12Device5> creationdevice);
	void SetPipelineConfig(UINT maxtracerecursiondepth=1);
	void SetShader(DX12Shader* shader, wstring hlslentry, wstring uniquename);
private:
	ComPtr<ID3D12StateObject> m_stateobject;
	D3D12_STATE_OBJECT_DESC m_desc;
	vector<D3D12_STATE_SUBOBJECT> m_statesubobjects;
	//only 1 rt config for a pipeline(many does not makes any sense so keep this desc a direct member variable.
	D3D12_RAYTRACING_PIPELINE_CONFIG m_rtconfig;
	vector< RTPSOShader> m_shaderstouse;
	//kee
};

