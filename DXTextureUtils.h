#pragma once
#include"DXUtils.h"
#include<dxtex/DirectXTex.h>

using namespace DirectX;

struct DXImageData
{
	ScratchImage m_image;
	TexMetadata m_imagemetadata;

	void GetSubresData(ComPtr< ID3D12Device> creationdevice, vector< D3D12_SUBRESOURCE_DATA>& out_subresdata);

};
