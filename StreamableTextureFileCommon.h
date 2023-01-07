#pragma once
#include"DXTexManager.h"
struct SubResDataBlock
{
	uint64_t rowpitch;
	uint64_t SlicePitch;
};

SubResDataBlock GetSubResDataBlockFromDXImage(const Image& img);