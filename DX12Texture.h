#pragma once
#include"DX12Resource.h"
#include"DX12Buffer.h"

//forward declare dximagedata
struct DXImageData;
struct TextureCreationProperties
{
	D3D12_RESOURCE_DESC texdesc;
	D3D12_RESOURCE_STATES initstate;
	D3D12_CLEAR_VALUE clearvalue;
	D3D12_HEAP_PROPERTIES heapprop;
	D3D12_HEAP_FLAGS heapflags;
};

//a simple dx12 texture which does not represent any specialized use of texture 
class DX12TextureSimple:public DX12Resource
{
public:
	
	DX12TextureSimple();
	~DX12TextureSimple();

};

class DX12TextureUploadeHelper
{
public:
	DX12TextureUploadeHelper();
	~DX12TextureUploadeHelper();
	//prepares the texture for full upload 
	void PrepareUpload(ComPtr< ID3D12Device> creationdevice,DX12ResourceBase* targettexture);

	//upload the actual texture data to the upload buffers
	void SetUploadTextureData(const DXImageData& imagedata);
	
	//numsubres -1 is all subresources
	void Upload(DX12Commandlist& copycmdlist,UINT numsubres=-1,UINT firstsubres=0);

private:
	DX12ResourceBase* m_targettexture;
	UINT* m_subresrowcount;
	UINT64* m_subresrowsize;
	DX12Buffer m_uploadbuffer;
	UINT m_subresoucecount;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* m_subresplacedfootprints;

};

