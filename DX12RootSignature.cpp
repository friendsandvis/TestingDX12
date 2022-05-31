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
void DX12RootSignature::BuidDesc(vector<D3D12_ROOT_PARAMETER>& rootparams, vector<D3D12_STATIC_SAMPLER_DESC>staticsamplers, D3D12_ROOT_SIGNATURE_FLAGS flags)
{
	m_rootparams = rootparams;
	m_staticsamplerdescs = staticsamplers;
	m_desc.NumParameters = (UINT)m_rootparams.size();
	m_desc.pParameters = m_rootparams.data();
	m_desc.NumStaticSamplers = (UINT)m_staticsamplerdescs.size();
	m_desc.pStaticSamplers = m_staticsamplerdescs.data();
	m_desc.Flags = flags;

}

void DX12RootSignature::Init(ComPtr< ID3D12Device> creationdevice,D3D_ROOT_SIGNATURE_VERSION rootsigversion)
{
	DXASSERT(D3D12SerializeRootSignature(&m_desc, rootsigversion, m_rootSignatureblob.GetAddressOf(), m_errorblob.GetAddressOf()))
		
		

	DXASSERT(creationdevice->CreateRootSignature(0,m_rootSignatureblob->GetBufferPointer(),m_rootSignatureblob->GetBufferSize(),IID_PPV_ARGS(m_signature.GetAddressOf())))
}

D3D12_STATIC_SAMPLER_DESC StaticSamplerManager::GetDefaultStaticSamplerDesc()
{
	D3D12_STATIC_SAMPLER_DESC simplesampler = {};
	simplesampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	simplesampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	simplesampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	simplesampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	simplesampler.MipLODBias = 0;
	simplesampler.MaxAnisotropy = 0;
	simplesampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	simplesampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	simplesampler.MinLOD = 0.0f;
	simplesampler.MaxLOD = D3D12_FLOAT32_MAX;
	simplesampler.ShaderRegister = 0;
	simplesampler.RegisterSpace = 0;
	simplesampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	return simplesampler;
}