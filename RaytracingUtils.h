#pragma once
#include"AssetManager.h"

class AccelerationStructureResource :public DX12ResourceBase
{
public:
	void Init(ComPtr< ID3D12Device5> device, UINT64 size, bool isscratch=false);
private:
	D3D12_RESOURCE_DESC m_resdesc;
};

//represents a BLAS
class ModelAccelerationStructure
{
	void Init(Model& modeltoprocess);
	//setup all the needful for building the as
	void Build(ComPtr< ID3D12Device5> device);
	void IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist);
private:
	D3D12_RAYTRACING_GEOMETRY_DESC m_rtgeometrydesc;
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO m_prebuildinfo;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS m_asinputs;
	AccelerationStructureResource m_accelerationstucturescratch,m_accelerationstructure;

};