#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"DX12ReservedResource.h"
#include"DX12ReservedResourceMemoryManager.h"


class DX12ReservedResourceApplication :public DX12ApplicationManagerBase
{
public:
	DX12ReservedResourceApplication();
	~DX12ReservedResourceApplication();
	void Render()override;
	


protected:
	void InitExtras();

private:
	DX12PSO m_basicpso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature;
	Model m_planemodel;
	DXTexture m_redtexture;
	DX12ReservedResource m_greentex_reservedres;
	DX12ReservedResourceMemoryManager m_greentexmemmanager;
	DX12DESCHEAP m_resaccessviewdescheap;//CBV/SRV/UAV
	//comandlist exclusively used for uploading reservedresource data
	DX12Commandlist m_reservedresuploadcmdlist;

	void InitBasicPSO();

};


