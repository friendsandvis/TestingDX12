#pragma once
#include"AssimpManager.h"
#include"DXUtils.h"
#include"DX12Buffer.h"
#include"DXVertexManager.h"
#include"DXTexManager.h"
#include"DX12Texture.h"


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
/*
* Imp terms
* Load a texture means load from file the tex data/params
* Upload means upload actual data to gpu resource
* init means create actual resource objhects from loaded texture data.
*/
class ModelMaterial
{
public:
	//create the texture objects if the textures are loaded.
	void Init(ComPtr< ID3D12Device> creationdevice);
	//upload data to gpu resource.
	void UploadTextures(DX12Commandlist& copycmdlist);
	//load from file the texture data imp:no resource creation here
	void LoadDifuseTexture(std::wstring texname);
	//load from file the texture data imp:no resource creation here
	void LoadNormalTexture(std::wstring texname);
	//load from file the texture data imp:no resource creation here
	void LoadRoughnessTexture(std::wstring texname);
	//load from file the texture data imp:no resource creation here
	void LoadMetalnessTexture(std::wstring texname);
	void GetMaterialTextures(vector< DXTexture*>& textures);
	void SetTexPath(wstring texfilesPath) { m_texfilepath = texfilesPath; }
	wstring GetTexPath() { return m_texfilepath; }
	ModelMaterial();
	~ModelMaterial();
	DXTexture* GetDiffuseTexture() { return m_diffusetexture; }
	DXTexture* GetNormalTexture() { return m_normaltexture; }
	DXTexture* GetRoughnessTexture() { return m_roughnesstexture; }
	DXTexture* GetMetalnessTexture() { return m_metalnesstexture; }
private:
	std::wstring GetTextureFilePath(std::wstring texname);
	//pbr texture set
	DXTexture* m_diffusetexture;
	DXTexture* m_normaltexture;
	DXTexture* m_roughnesstexture;
	DXTexture* m_metalnesstexture;
	wstring m_texfilepath = L"";

};
//hold texture index for each material texture srv parameter in srvheap(pbr material only for now)
struct MaterialDataGPU
{
	unsigned diffusetexidx;
	unsigned normaltexidx;
	unsigned roughnesstexidx;

};
class Model
{
public:
	struct MaterialConstants
	{
		unsigned int texsrvidx;
	};
	XMMATRIX GetTransform() { return m_transform; }
	void Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix,UINT mvpmatrixrootparamindex,UINT materialconstsrootparamindex,bool usemodelmatrix=true,bool setmvpmatrix=true,bool supportmaterial=false);
	void Extratransform(XMMATRIX extratransformmat);
	Model(ModelDataUploadMode uploadmode=NOCOPY);
	~Model();
	
	
	D3D12_GPU_VIRTUAL_ADDRESS GetVertexBufferGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS GetIndexBufferGPUVirtualAddress();
	void Init(ComPtr< ID3D12Device> creationdevice,AssimpLoadedModel& assimpModel,UINT meshindexinassimpmodeltoload,VertexVersion modelvertexversion,bool supportmaterial =false);
	void InitVertexBuffer(ComPtr< ID3D12Device> creationdevice,vector<VertexBase*>& verticies);
	void InitIndexBuffer(ComPtr< ID3D12Device> creationdevice,vector<unsigned>& indicies);
	inline D3D12_INDEX_BUFFER_VIEW GetIBView() { return m_indexbufferview; }
	inline D3D12_VERTEX_BUFFER_VIEW GetVBView() { return m_vertexbufferview; }
	void UploadModelDatatoBuffers();
	void UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist);
	void UploadModelTextureData(DX12Commandlist& copycmdlist);
	inline size_t GetIndiciesCount() { return m_indicies.size(); }
	inline size_t GetVerticiesCount() { return m_verticies.size(); }
	inline ModelDataUploadMode GetUploadMode() { return m_uploadmode; }
	inline VertexVersion GetVertexVersionUsed() { return m_vertexversion; }
	inline void SetVertexVersionUsed(VertexVersion vvused) { m_vertexversion=vvused; }
	void TransitionVextexAndIndexBufferState(D3D12_RESOURCE_STATES state, ComPtr<ID3D12GraphicsCommandList4>cmdlist);
	ModelMaterial& GetLoadedMaterial() { return m_loadedmaterial; }
	void SetMatGPUIdx(unsigned idx) { m_tmpmaterialgpuindex = idx; m_matconsts.texsrvidx = m_tmpmaterialgpuindex; }
	void SetDiffuseIdx(unsigned idx) { m_diffusetexidx = idx; m_matconsts.texsrvidx = m_diffusetexidx;
	}
	unsigned GetMatGPUIdx() { return m_tmpmaterialgpuindex; }
	void GetMaterialTextures(vector< DXTexture*>& textures);
	void AllowRender(bool allow = true) { m_Allowrender=allow; }
	//materialdata gpu control functions
	void SetDiffusetextureIdx(unsigned idx) { m_materialdata_gpu.diffusetexidx=idx; }
	void SetNormaltextureIdx(unsigned idx) { m_materialdata_gpu.normaltexidx = idx; }
	void SetRoughnesstextureIdx(unsigned idx) { m_materialdata_gpu.roughnesstexidx = idx; }
	MaterialDataGPU GetMaterialDataGPU() { return m_materialdata_gpu; }
	void SetTexPath(wstring texfilesPath) { m_texfilepath = texfilesPath; }
	wstring GetTexPath() { return m_texfilepath;}
	bool HasOpaqueMaterial() { return m_hasOpaqueMaterial; }

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
	AssimpMaterial m_material;
	//actual material block used to retrive material data for model in shaders
	MaterialDataGPU m_materialdata_gpu;
	//use meshmaterial to load/create final material for use
	ModelMaterial m_loadedmaterial;
	MaterialConstants m_matconsts;
	bool m_Allowrender;
	wstring m_texfilepath = L"";
	bool m_hasOpaqueMaterial = true;
	//an index used to refer in a global descriptor heap/mat table or similar
	unsigned m_tmpmaterialgpuindex,m_diffusetexidx;
	void GetVertexArray(vector<VertexBase*>& outverticies, AssimpLoadedMesh& ameshtoadd,VertexVersion vertversion);
	void BuildVertexRawData();
	void InitMaterial(ComPtr< ID3D12Device> creationdevice, wstring texfilepath);
	bool HasTransparentMaterial(wstring texfilepath);
};
//contains various basic models
class CompoundModel
{
public:
	//set ib & vb and issue draw command.
	void Extratransform(XMMATRIX extratransformmat);
	void Draw(DX12Commandlist& renderingcmdlist, XMMATRIX vpmatrix,UINT mvpmatrixrootparamindex, UINT materialconstsrootparamindex=2, bool drawNonOpaqueModels = true);
	void UploadModelDatatoBuffers();
	void UploadModelDatatoGPUBuffers(DX12Commandlist& copycmdlist);
	void UploadModelDataDefaultTexture(DX12Commandlist& copycmdlist);
	//a combined upload function to issue upload commands for model data to gpu and other default data if any.
	void UploadData(DX12Commandlist& copycmdlist, bool uploaddefaults = true, bool uploadmodeldatatogpubuffers = true);
	void UploadModelTextureData(DX12Commandlist& copycmdlist);
	const vector<Model*>& GetModels() { return m_models; }
	DX12DESCHEAP& GetResourceviewHeap() { return m_resourceviewheap; }
	CompoundModel(ModelDataUploadMode uploadmode = NOCOPY);
	~CompoundModel();
	void Init(ComPtr< ID3D12Device> creationdevice, AssimpLoadedModel& assimpModel, VertexVersion modelvertexversion,bool supportmaterial=false);
	void AddModel(Model* abasicModel) { m_models.push_back(abasicModel); }
	void SetTexPath(wstring texfilesPath) { m_texfilepath = texfilesPath; }
	wstring GetTexPath() { return m_texfilepath; }
	ModelDataUploadMode m_datauploadmode;
	VertexVersion m_vertexversion;
	vector<Model*> m_models;
	//model material related data
	DX12DESCHEAP m_resourceviewheap;
	DX12TextureSimple m_whitetexture;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_whitetexsubresfootprint;
	DX12Buffer m_whitetexuploadbuffer;
	vector< DXTexture*> m_texturestoupload;
	
	std::vector<MaterialDataGPU> m_allmaterialsused;
	DX12Buffer m_materialtable;
	wstring m_texfilepath = L"";
	//local textures upload helper utils
	size_t m_currenttexidxtoupload;
	bool m_supportmaterial;
	bool SupportMaterial() { return m_supportmaterial; }
	void UploadCurrentFrameModelTextureData(DX12Commandlist& copycmdlist,bool increment =true);
	bool NeedToUploadTextures() { return (m_currenttexidxtoupload<m_texturestoupload.size()); }
	
};

class BasicModelManager
{
public:
	static void InitPlaneModel(ComPtr< ID3D12Device> creationdevice, Model& planemodel);
	static void InitCubeModel(ComPtr< ID3D12Device> creationdevice, Model& cubemodel);
	static void InitTriangleModel(ComPtr< ID3D12Device> creationdevice, Model& trianglemodel);
	static void LoadModel(ComPtr< ID3D12Device> creationdevice,std::string modelfilepath, Model& outmodel, VertexVersion requiredvertexversion);
	static void LoadModel(ComPtr< ID3D12Device> creationdevice, std::string modelfilepath, CompoundModel& outmodel, VertexVersion requiredvertexversion, wstring texfilepath = L"", bool supportmaterial = false);
	static void GetTriangleRTVertexData(vector<RTVertexDataV0>& rtvertexdata);

private:
	static void GetPlaneVerticiesV0(vector<VertexBase*>& outverticies);
	static void GetCubeVerticiesV0(vector<VertexBase*>& outverticies);
	static void GetTriangleVerticiesV0(vector<VertexBase*>& outverticies);
};

