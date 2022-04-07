#pragma once
#include"DXTextureUtils.h"
#include"DX12ResourceBase.h"

//the subresource is designed to treat mips as sub resource other elements(like depth/array) not considered in subresource calculations.
class DX12ReservedResource :public DX12ResourceBase
{
public:
	void Init(ComPtr< ID3D12Device> creationdevice, DX12ResourceCreationProperties resprops);
	void Init(ComPtr< ID3D12Device> creationdevice);
	void InitTilingInfo(ComPtr< ID3D12Device> creationdevice);

	inline size_t GetTotalMipCount() { return m_creationproperties.resdesc.MipLevels; }
	void CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle);
	void CreateUAV(ComPtr< ID3D12Device> creationdevice, D3D12_UNORDERED_ACCESS_VIEW_DESC uavdesc, D3D12_CPU_DESCRIPTOR_HANDLE uavhandle);
	const D3D12_PACKED_MIP_INFO& GetPackedMipInfo() { return m_packedmipinfo; }
	const vector<D3D12_SUBRESOURCE_TILING> GetSubResourceTilingInfo() { return m_subresourcetiling; }
	DXImageData& GetImageData() { return m_imagedata; }
	DX12ReservedResource();
	~DX12ReservedResource();
	// initialize the resource creation properties with defaults to prevent missing any params while manual init
	static void InitResourceCreationProperties(DX12ResourceCreationProperties& rescreationprops);

protected:
	DX12ResourceCreationProperties m_creationproperties;
	D3D12_PACKED_MIP_INFO m_packedmipinfo;
	vector<D3D12_SUBRESOURCE_TILING> m_subresourcetiling;
	DXImageData m_imagedata;

};