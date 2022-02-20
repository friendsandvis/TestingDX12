#include"DX12Buffer.h"


void* DX12Buffer::Map(BufferMapParams readparams)
{
	void* mappedbuffer=nullptr;
	DXASSERT(m_resource->Map(readparams.subresource,&readparams.range,&mappedbuffer))
	return mappedbuffer;
}

void DX12Buffer::UnMap(BufferMapParams writeparams)
{
	m_resource->Unmap(writeparams.subresource, &writeparams.range);
}

