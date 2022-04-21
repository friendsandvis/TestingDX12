#pragma once
#include"AssimpManager.h"
#include"DXUtils.h"
#include"DX12Buffer.h"
#include"DXVertexManager.h"


/*struct Vertex
{
	float X;
	float Y;
	float Z;
	float U;
	float V;
};*/



enum ModelDataUploadMode
{
	NOCOPY,//using uploadbuffer directly as vb & ib
	COPY	//using an upload buffer to copy vb & ib to a default heap buffer
};

//forwarddeclare
class DX12Commandlist;

class Model
{
public:
	
	Model(ModelDataUploadMode uploadmode=NOCOPY);
	~Model();
	void Init(AssimpLoadedModel& assimpModel);
	void InitVertexBuffer(ComPtr< ID3D12Device> creationdevice,vector<Vertex>& verticies);
	void InitIndexBuffer(ComPtr< ID3D12Device> creationdevice,vector<unsigned>& indicies);
	inline D3D12_INDEX_BUFFER_VIEW GetIBView() { return m_indexbufferview; }
	inline D3D12_VERTEX_BUFFER_VIEW GetVBView() { return m_vertexbufferview; }
	void UploadModelDatatoBuffers();
	void UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist);
	inline size_t GetIndiciesCount() { return m_indicies.size(); }
	inline ModelDataUploadMode GetUploadMode() { return m_uploadmode; }
	inline VertexVersion GetVertexVersionUsed() { return m_vertexversionused; }
	inline void SetVertexVersionUsed(VertexVersion vvused) { m_vertexversionused=vvused; }

private:
	DX12Buffer m_vertexbuffer, m_indexbuffer,m_vertexuploadbuffer,m_indexuploadbuffer;
	vector<Vertex> m_verticies;
	vector<unsigned> m_indicies;
	D3D12_VERTEX_BUFFER_VIEW m_vertexbufferview;
	D3D12_INDEX_BUFFER_VIEW m_indexbufferview;
	ModelDataUploadMode m_uploadmode;
	VertexVersion m_vertexversionused;
};

class BasicModelManager
{
public:
	static void InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel);
	static void InitCubeModel(ComPtr< ID3D12Device> creationdevice, Model& cubemodel);
	static void LoadModel(std::string modelfilepath);

private:
	static void InitCubeVerticiesV1(vector<VetexV1>& verticies);
	static void InitCubeIndicies(vector<unsigned>& indicies);
};

