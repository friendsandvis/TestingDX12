#include"DX12RootSignature.h"

DX12RootSignature::DX12RootSignature()
{
	// initialize signature desc
	m_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	m_desc.NumParameters = 0;
	m_desc.NumStaticSamplers = 0;
	m_desc.pParameters = nullptr;
	m_desc.pStaticSamplers = nullptr;
}

DX12RootSignature::~DX12RootSignature()
{
}

void DX12RootSignature::Init(ComPtr< ID3D12Device> creationdevice,D3D_ROOT_SIGNATURE_VERSION rootsigversion)
{
	DXASSERT(D3D12SerializeRootSignature(&m_desc, rootsigversion, m_rootSignatureblob.GetAddressOf(), m_errorblob.GetAddressOf()))
	DXASSERT(creationdevice->CreateRootSignature(0,m_rootSignatureblob->GetBufferPointer(),m_rootSignatureblob->GetBufferSize(),IID_PPV_ARGS(m_signature.GetAddressOf())))
}