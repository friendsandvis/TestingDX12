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
	//set ib & vb and issue draw command.
	
	void Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix,UINT mvpmatrixrootparamindex,bool usemodelmatrix=true,bool setmvpmatrix=true);
	Model(ModelDataUploadMode uploadmode=NOCOPY);
	~Model();
	
	
	D3D12_GPU_VIRTUAL_ADDRESS GetVertexBufferGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS GetIndexBufferGPUVirtualAddress();
	void Init(ComPtr< ID3D12Device> creationdevice,AssimpLoadedModel& assimpModel,UINT meshindexinassimpmodeltoload,VertexVersion modelvertexversion);
	void InitVertexBuffer(ComPtr< ID3D12Device> creationdevice,vector<VertexBase*>& verticies);
	void InitIndexBuffer(ComPtr< ID3D12Device> creationdevice,vector<unsigned>& indicies);
	inline D3D12_INDEX_BUFFER_VIEW GetIBView() { return m_indexbufferview; }
	inline D3D12_VERTEX_BUFFER_VIEW GetVBView() { return m_vertexbufferview; }
	void UploadModelDatatoBuffers();
	void UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist);
	inline size_t GetIndiciesCount() { return m_indicies.size(); }
	inline size_t GetVerticiesCount() { return m_verticies.size(); }
	inline ModelDataUploadMode GetUploadMode() { return m_uploadmode; }
	inline VertexVersion GetVertexVersionUsed() { return m_vertexversion; }
	inline void SetVertexVersionUsed(VertexVersion vvused) { m_vertexversion=vvused; }

private:
	XMMATRIX m_transform;
	DX12Buffer m_vertexbuffer, m_indexbuffer,m_vertexuploadbuffer,m_indexuploadbuffer;
	//verticies are all of a single specialized vertex type and they are all dynamically allocated.
	VertexVersion m_vertexversion;
	vector<VertexBase*> m_verticies;
	//verticies laid down in the raw vertex data form(i.e. array of floats;this is needed because verticies can be of any specialized type hence cannot be copied directly to upload buffer.
	vector<float> m_vertexdataraw;
	vector<unsigned> m_indicies;
	D3D12_VERTEX_BUFFER_VIEW m_vertexbufferview;
	D3D12_INDEX_BUFFER_VIEW m_indexbufferview;
	ModelDataUploadMode m_uploadmode;
	void GetVertexArray(vector<VertexBase*>& outverticies, AssimpLoadedMesh& ameshtoadd,VertexVersion vertversion);
	void BuildVertexRawData();
};
//contains various basic models
class CompoundModel
{
public:
	//set ib & vb and issue draw command.
	void Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix,UINT mvpmatrixrootparamindex);
	void UploadModelDatatoBuffers();
	void UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist);
	CompoundModel(ModelDataUploadMode uploadmode = NOCOPY);
	~CompoundModel();
	void Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, VertexVersion modelvertexversion);
	void AddModel(Model* abasicModel) { m_models.push_back(abasicModel); }
	ModelDataUploadMode m_datauploadmode;
	VertexVersion m_vertexversion;
	vector<Model*> m_models;
};

class BasicModelManager
{
public:
	static void InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel);
	static void InitCubeModel(ComPtr< ID3D12Device> creationdevice, Model& cubemodel);
	static void InitTriangleModel(ComPtr< ID3D12Device> creationdevice, Model& trianglemodel);
	static void LoadModel(ComPtr< ID3D12Device> creationdevice,std::string modelfilepath, Model& outmodel, VertexVersion requiredvertexversion);
	static void LoadModel(ComPtr< ID3D12Device> creationdevice, std::string modelfilepath, CompoundModel& outmodel, VertexVersion requiredvertexversion);

private:
	static void GetPlaneVerticiesV0(vector<VertexBase*>& outverticies);
	static void GetCubeVerticiesV0(vector<VertexBase*>& outverticies);
	static void GetTriangleVerticiesV0(vector<VertexBase*>& outverticies);
};

