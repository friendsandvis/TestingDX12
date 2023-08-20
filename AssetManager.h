#pragma once
#include"AssimpManager.h"
#include"DXUtils.h"
#include"DX12Buffer.h"
#include"DXVertexManager.h"
#include"DXTexManager.h"


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
/*this struct specifics the constants required by shader/vertexshader for a typical camera based render scenario
(i.e. 2 matricies: 1st for mvp matrix and 2nd for model matrix alone)*/
struct ShaderTransformConstants_General
{
	XMMATRIX mvp;
	XMMATRIX model;
};

//forwarddeclare
class DX12Commandlist;
class ModelMaterial
{
public:
	void LoadDifuseTexture(std::wstring texname);
	void LoadNormalTexture(std::wstring texname);

	ModelMaterial();
	~ModelMaterial();
	DXTexture* GetDiffuseTexture() { return m_diffusetexture; }
	DXTexture* GetNormalTexture() { return m_normaltexture; }
private:
	std::wstring GetTextureFilePath(std::wstring texname);
	DXTexture* m_diffusetexture;
	DXTexture* m_normaltexture;
};
class Model
{
public:
	XMMATRIX GetTransform() { return m_transform; }
	void Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix,UINT mvpmatrixrootparamindex,bool usemodelmatrix=true,bool setmvpmatrix=true);
	void Extratransform(XMMATRIX extratransformmat);
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
	void TransitionVextexAndIndexBufferState(D3D12_RESOURCE_STATES state, ComPtr<ID3D12GraphicsCommandList4>cmdlist);
	ModelMaterial& GetLoadedMaterial() { return m_loadedmaterial; }

private:
	ShaderTransformConstants_General m_shadertransformconsts;
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
	//no difference between assimp material & model material for now.
	MeshMaterial m_material;
	//use meshmaterial to load/create final material for use
	ModelMaterial m_loadedmaterial;
	void GetVertexArray(vector<VertexBase*>& outverticies, AssimpLoadedMesh& ameshtoadd,VertexVersion vertversion);
	void BuildVertexRawData();
	void InitMaterial();
};
//contains various basic models
class CompoundModel
{
public:
	//set ib & vb and issue draw command.
	void Extratransform(XMMATRIX extratransformmat);
	void Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix,UINT mvpmatrixrootparamindex);
	void UploadModelDatatoBuffers();
	void UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist);
	const vector<Model*>& GetModels() { return m_models; }
	CompoundModel(ModelDataUploadMode uploadmode = NOCOPY);
	~CompoundModel();
	void Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, VertexVersion modelvertexversion);
	void AddModel(Model* abasicModel) { m_models.push_back(abasicModel); }
	ModelDataUploadMode m_datauploadmode;
	VertexVersion m_vertexversion;
	vector<Model*> m_models;
	//model material related data
	DX12DESCHEAP m_texturesrvheap;
};

class BasicModelManager
{
public:
	static void InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel);
	static void InitCubeModel(ComPtr< ID3D12Device> creationdevice, Model& cubemodel);
	static void InitTriangleModel(ComPtr< ID3D12Device> creationdevice, Model& trianglemodel);
	static void LoadModel(ComPtr< ID3D12Device> creationdevice,std::string modelfilepath, Model& outmodel, VertexVersion requiredvertexversion);
	static void LoadModel(ComPtr< ID3D12Device> creationdevice, std::string modelfilepath, CompoundModel& outmodel, VertexVersion requiredvertexversion);
	static void GetTriangleRTVertexData(vector<RTVertexDataV0>& rtvertexdata);

private:
	static void GetPlaneVerticiesV0(vector<VertexBase*>& outverticies);
	static void GetCubeVerticiesV0(vector<VertexBase*>& outverticies);
	static void GetTriangleVerticiesV0(vector<VertexBase*>& outverticies);
};

