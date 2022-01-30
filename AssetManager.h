#pragma once
#include"DXUtils.h"
#include"DX12Buffer.h"

struct Vertex
{
	float X;
	float Y;
	float Z;
	float U;
	float V;
};

class Model
{
public:
	Model();
	~Model();
	void InitVertexBuffer(ComPtr< ID3D12Device> creationdevice,vector<Vertex>& verticies);
	void InitIndexBuffer(ComPtr< ID3D12Device> creationdevice,vector<unsigned>& indicies);

private:
	DX12Buffer m_vertexbuffer, m_indexbuffer;
	vector<Vertex> m_verticies;
	vector<unsigned> m_indicies;
};

class BasicModelManager
{
public:
	static void InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel);
};

