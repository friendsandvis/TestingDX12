#pragma once
#include"DXUtils.h"

class RTPSO
{
public:
	RTPSO();
	~RTPSO();

	void Init(ComPtr<ID3D12Device5> creationdevice);

private:
	ComPtr<ID3D12StateObject> m_stateobject;
	D3D12_STATE_OBJECT_DESC m_desc;
	vector<D3D12_STATE_SUBOBJECT> m_statesubobjects;
};

