#pragma once
#include"AssetManager.h"

class AccelerationStructureResource :public DX12ResourceBase
{
private:
	
};


class ModelAccelerationStructure
{
	void Init(Model& modeltoprocess);
private:
	D3D12_RAYTRACING_GEOMETRY_DESC m_rtgeometrydesc;
};