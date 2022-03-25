#pragma once
#include"DXUtils.h"

class DX12RootSignature
{
public:
	DX12RootSignature();
	~DX12RootSignature();
	void Init(ComPtr< ID3D12Device> creationdevice, D3D_ROOT_SIGNATURE_VERSION rootsigversion);
	D3D12_ROOT_SIGNATURE_DESC& getSignatureDescforModification() { return m_desc; }
	ID3D12RootSignature* GetRootSignature() { return m_signature.Get(); }
private:
	ComPtr<ID3DBlob> m_rootSignatureblob, m_errorblob;
	D3D12_ROOT_SIGNATURE_DESC m_desc;
	ComPtr<ID3D12RootSignature> m_signature;
};

class StaticSamplerManager
{
public:
	static D3D12_STATIC_SAMPLER_DESC GetDefaultStaticSamplerDesc();
};

