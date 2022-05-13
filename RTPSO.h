#pragma once
#include"DXUtils.h"

class RTPSO
{
public:
	RTPSO();
	~RTPSO();

	void Init(ComPtr<ID3D12Device5> creationdevice);
	void SetPipelineConfig(UINT maxtracerecursiondepth=1);

private:
	ComPtr<ID3D12StateObject> m_stateobject;
	D3D12_STATE_OBJECT_DESC m_desc;
	vector<D3D12_STATE_SUBOBJECT> m_statesubobjects;
	//only 1 rt config for a pipeline(many does not makes any sense so keep this desc a direct member variable.
	D3D12_RAYTRACING_PIPELINE_CONFIG m_rtconfig;
};

