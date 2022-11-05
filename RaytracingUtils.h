#pragma once
#include"AssetManager.h"
#include"DX12Buffer.h"

class AccelerationStructureResource :public DX12ResourceBase
{
public:
	void Init(ComPtr< ID3D12Device5> device, UINT64 size, bool isscratch=false);
private:
	D3D12_RESOURCE_DESC m_resdesc;
};

//represents a BLAS
class ModelAccelerationStructureBLAS
{
public:
	AccelerationStructureResource& GetBLAS() { return m_accelerationstructure; }
	ModelAccelerationStructureBLAS();
	
	void Init(ComPtr< ID3D12Device> creationdevice,Model& modeltoprocess);
	//setup all the needful for building the as
	void Build(ComPtr< ID3D12Device5> device);
	void IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist);
private:
	bool m_buildcmdissued;
	D3D12_RAYTRACING_GEOMETRY_DESC m_rtgeometrydesc;
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO m_prebuildinfo;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS m_asinputs;
	AccelerationStructureResource m_accelerationstucturescratch,m_accelerationstructure;
	DX12Buffer m_transformbuffer;

};
class CompoundModelAccelerationStructureBLAS
{
public:
	CompoundModelAccelerationStructureBLAS();
	~CompoundModelAccelerationStructureBLAS();
	const std::vector<ModelAccelerationStructureBLAS*>& GetBlas() { return m_modelblas; }
	void Init(ComPtr< ID3D12Device> creationdevice, CompoundModel& modeltoprocess);
	void Build(ComPtr< ID3D12Device5> device);
	void IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist);

private:
	bool m_buildcmdissued;
	std::vector<ModelAccelerationStructureBLAS*> m_modelblas;
};




class ModelAccelerationStructureTLAS
{
	public:
		ModelAccelerationStructureTLAS();
		void Init(ComPtr< ID3D12Device> creationdevice,vector<D3D12_RAYTRACING_INSTANCE_DESC> instancedescs);
		void Build(ComPtr< ID3D12Device5> device);
		void IssueBuild(ComPtr<ID3D12GraphicsCommandList4>buildcmdlist);
		void CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle);
private:
	bool m_buildcmdissued;
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO m_prebuildinfo;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS m_asinputs;
	AccelerationStructureResource m_accelerationstucturescratch, m_accelerationstructure;
	vector<D3D12_RAYTRACING_INSTANCE_DESC> m_instancedescs;
	DX12Buffer m_instancedescbuffer;
};

class RaytracingCommon
{
public:
	static void InitAsIdentityMatrix(FLOAT arr[3][4]);
	static void XMMatrixToRowMajor3x4(XMMATRIX&mat,float* outmat);
};