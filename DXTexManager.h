#pragma once
#include"DXUtils.h"
#include<dxtex/DirectXTex.h>
#include"DX12Buffer.h"

using namespace DirectX;

struct DXImageData
{
	ScratchImage m_image;
	TexMetadata m_imagemetadata;

	void GetSubresData(ComPtr< ID3D12Device> creationdevice, vector< D3D12_SUBRESOURCE_DATA>& out_subresdata);

};
class DX12Commandlist;

//does not represents a generic dx12 texture but just a dx12 texture resource representing a directxtex loaded textre(the init creates commited resource as it calls DXTex createtexture fuction)
class DXTexture:public DX12ResourceBase
{
	DXImageData m_texdata;
	DX12Buffer m_uploadbuffer;
	vector< D3D12_SUBRESOURCE_DATA> m_uploadsubresdata;

public:
	DXImageData& GetDXImageData() { return m_texdata; }
	size_t GetTotalMipCount() { return m_texdata.m_imagemetadata.mipLevels; }
	void CreateSRV(ComPtr< ID3D12Device> creationdevice, D3D12_SHADER_RESOURCE_VIEW_DESC srvdesc, D3D12_CPU_DESCRIPTOR_HANDLE srvhandle);

	bool Init(ComPtr< ID3D12Device> creationdevice);
	void UploadTexture(DX12Commandlist& copycmdlist);
};

class DXTextureCube :public DX12ResourceBase
{
	DXImageData m_texdata;
	vector<D3D12_SUBRESOURCE_DATA> m_subresdata;

public:
	void UploadTexureData(DX12Commandlist& copycmdlist);
void Init(ComPtr< ID3D12Device> creationdevice);
	DXImageData& GetDXImageData() { return m_texdata; }
	DX12Buffer m_uploadbuffer;
};

class DXTexManager
{
public:
	DXTexManager();
	~DXTexManager();
	static bool LoadTexture(const wchar_t* imagefile, DXImageData& outloadedImagedata);

private:

};

