#pragma once

#include<DirectXMath.h>
#include"DXUtils.h"
using namespace DirectX;
/*
Define Various Vertex structures to be used in applications.

*/

struct Vetex_V0
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
};

struct VetexV1
{
	XMFLOAT3 position;
	XMFLOAT3 vertexcolour;
};

struct VetexV2
{
	XMFLOAT3 position;
	XMFLOAT3 vertexcolour;
	XMFLOAT2 uv;
};

enum VertexType
{
	VERTEXVERSION0,
	VERTEXVERSION1,
	VERTEXVERSION2
};

class DXVertexManager
{
public:
	static void BuildDefaultInputelementdesc(std::vector< D3D12_INPUT_ELEMENT_DESC>& outinputelementdescs, VertexType vtype);
};