#pragma once
#include"DXUtils.h"
#include<DirectXTex.h>
#include"DX12Buffer.h"

using namespace DirectX;

struct DXImageData
{
	ScratchImage m_image;
	TexMetadata m_imagemetadata;

};
class DX12Commandlist;

//does not represents a generic dx12 texture but just a dx12 texture resource representing a directxtex loaded textre
class DXTexture:public DX12Resource
{
	DXImageData m_texdata;
	DX12Buffer m_uploadbuffer;
	vector< D3D12_SUBRESOURCE_DATA> m_uploadsubresdata;

public:
	DXImageData& GetDXImageData() { return m_texdata; }
	bool Init(ComPtr< ID3D12Device> creationdevice);
	void UploadTexture(DX12Commandlist& copycmdlist);
};

class DXTexManager
{
public:
	DXTexManager();
	~DXTexManager();
	static bool LoadTexture(const wchar_t* imagefile, DXImageData& outloadedImagedata);

private:

};

