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
	inline D3D12_INDEX_BUFFER_VIEW GetIBView() { return m_indexbufferview; }
	inline D3D12_VERTEX_BUFFER_VIEW GetVBView() { return m_vertexbufferview; }
	void UploadModelDatatoBuffers();
	inline size_t GetIndiciesCount() { return m_indicies.size(); }

private:
	DX12Buffer m_vertexbuffer, m_indexbuffer;
	vector<Vertex> m_verticies;
	vector<unsigned> m_indicies;
	D3D12_VERTEX_BUFFER_VIEW m_vertexbufferview;
	D3D12_INDEX_BUFFER_VIEW m_indexbufferview;
};

class BasicModelManager
{
public:
	static void InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel);
};

