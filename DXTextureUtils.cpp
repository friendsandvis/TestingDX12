#include"DXTextureUtils.h"

void DXImageData::GetSubresData(ComPtr< ID3D12Device> creationdevice, vector< D3D12_SUBRESOURCE_DATA>& out_subresdata)
{
	DXASSERT(PrepareUpload(creationdevice.Get(), m_image.GetImages(), m_image.GetImageCount(), m_imagemetadata, out_subresdata))
}