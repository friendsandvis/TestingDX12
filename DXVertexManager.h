#pragma once

#include<DirectXMath.h>
#include"DXUtils.h"
using namespace DirectX;
/*
Define Various Vertex classes to be used in applications.

*/

//most used simple position & uv vertex.
struct Vertex
{
	float X;
	float Y;
	float Z;
	float U;
	float V;
};

enum VertexVersion
{
	UNKNOWN,
	VERTEXVERSION0,
	VERTEXVERSION1,
	VERTEXVERSION2
};
class VertexBase
{
public:
	VertexVersion m_vertversion=VertexVersion::UNKNOWN;
	virtual unsigned GetSize() { return 0; }
};

class VetexV0:public VertexBase
{
public:
	XMFLOAT3 m_position;
	XMFLOAT2 m_uv;
	unsigned GetSize() { return (sizeof(float) * 5); }
	VetexV0()
	{
		m_vertversion = VertexVersion::VERTEXVERSION0;
	}
	void Set(XMFLOAT3 pos, XMFLOAT2 uv) { m_position = pos; m_uv = uv; }
};

class VetexV1 :public VertexBase
{
public:
	XMFLOAT3 m_position;
	XMFLOAT3 m_vertexcolour;
	unsigned GetSize() { return (sizeof(float) * 6); }
	VetexV1()
	{
		m_vertversion = VertexVersion::VERTEXVERSION1;
	}
	void Set(XMFLOAT3 pos, XMFLOAT3 col) { m_position = pos; m_vertexcolour = col; }
};

class VetexV2 :public VertexBase
{
public:
	XMFLOAT3 m_position;
	XMFLOAT3 m_normal;
	VetexV2()
	{
		m_vertversion = VertexVersion::VERTEXVERSION2;
	}
};



class DXVertexManager
{
public:
	static void BuildDefaultInputelementdesc(std::vector< D3D12_INPUT_ELEMENT_DESC>& outinputelementdescs, VertexVersion vtype);
	static unsigned GetVertexSize(VertexVersion vtype);
	static void RetriveRawVertexData(vector<float>& outverticiesrawdata, VertexBase* avertex);
};