#pragma once
#include"DXUtils.h"
#include"DX12Resource.h"

class SamplerFeedbackTexture :public DX12Resource
{
	void Init();
};

class DX12FeedBackUnit
{
public:
	DX12FeedBackUnit();
	~DX12FeedBackUnit();

private:
	SamplerFeedbackTexture m_feedbacktex;
};

