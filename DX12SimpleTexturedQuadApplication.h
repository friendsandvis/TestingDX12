#pragma once
#include"DX12ApplicationManagerBase.h"
#include"DX12PSO.h"
#include"AssetManager.h"
#include"DXTexManager.h"
#include"DX12Texture.h"


class TexturedQuadApplication:public DX12ApplicationManagerBase
{
public:
	TexturedQuadApplication();
	~TexturedQuadApplication();
	void Render()override;
	

protected:
	void InitExtras();

private:
	DX12PSO m_basicpso;
	ComPtr<ID3DBlob> m_emptyrootsignatureblob, m_rootsignatureerrors;
	ComPtr<ID3D12RootSignature> m_emptyrootsignature;
	Model m_planemodel;
	DXTexture m_mainLoadedtexture;
	DX12TextureSimple m_whitetexture;
	DX12Buffer m_whitetexuploadbuffer;
	DX12DESCHEAP m_resaccessviewdescheap;//CBV/SRV/UAV

	void InitBasicPSO();

};


