#pragma once

#include<DirectXMath.h>
#include"DXUtils.h"
using namespace DirectX;
/*
Define Various Vertex structures to be used in applications.

*/
enum VertexVersion
{
	UNKNOWN,
	VERTEXVERSION0,
	VERTEXVERSION1,
	VERTEXVERSION2
};
struct VertexBase
{

	VertexVersion m_vertversion=VertexVersion::UNKNOWN;
	virtual unsigned GetSize() { return 0; }
};

struct Vetex_V0:public VertexBase
{

	XMFLOAT3 m_position;
	XMFLOAT2 m_uv;
	unsigned GetSize() { return (sizeof(float) * 5); }
	Vetex_V0()
	{
		m_vertversion = VertexVersion::VERTEXVERSION0;
	}
	void Set(XMFLOAT3 pos, XMFLOAT2 uv) { m_position = pos; m_uv = uv; }
};

struct VetexV1 :public VertexBase
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_vertexcolour;
	unsigned GetSize() { return (sizeof(float) * 6); }
	VetexV1()
	{
		m_vertversion = VertexVersion::VERTEXVERSION1;
	}
	void Set(XMFLOAT3 pos, XMFLOAT3 col) { m_position = pos; m_vertexcolour = col; }
};

struct VetexV2 :public VertexBase
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_vertexcolour;
	XMFLOAT2 m_uv;
	unsigned GetSize() { return (sizeof(float) * 8); }
	VetexV2()
	{
		m_vertversion = VertexVersion::VERTEXVERSION2;
	}
	void Set(XMFLOAT3 pos, XMFLOAT3 col, XMFLOAT2 uv) { m_position = pos; m_vertexcolour = col; m_uv = uv; }
};



class DXVertexManager
{
public:
	static void BuildDefaultInputelementdesc(std::vector< D3D12_INPUT_ELEMENT_DESC>& outinputelementdescs, VertexVersion vtype);
};