#include"ShaderManager.h"
#pragma comment(lib,"D3DCompiler.lib")


DX12Shader::DX12Shader()
	:m_shadertype(VS)
{
}

DX12Shader::~DX12Shader()
{}

void* DX12Shader::GetCompiledCode()
{
	return reinterpret_cast<void*>(m_compiledcode.m_data);
}

bool DX12Shader::Init(LPCWSTR sourcehlslfilepath, ShaderType shadertype)
{
	m_shadertype = shadertype;
	m_sourcehlslfilename = sourcehlslfilepath;

	LPCSTR target = "";

	switch (m_shadertype)
	{
	case VS:
		target = "vs_5_0"; break;
	case PS:
		target = "ps_5_0"; break;

	}

	ComPtr<ID3DBlob> errors, outputcode;
	HRESULT res =
		D3DCompileFromFile(m_sourcehlslfilename, nullptr, nullptr, "main", target, 0, 0,outputcode.GetAddressOf(),errors.GetAddressOf());

	//copy code to local blob
	m_compiledcode.Init(outputcode->GetBufferSize(), outputcode->GetBufferPointer());
	if (errors.Get())
	{
		m_errormsgs.Init(errors->GetBufferSize(), errors->GetBufferPointer());
	}



	return ((res == S_OK) && (errors.Get()==nullptr));



}

DX12Customblob::DX12Customblob()
	:m_data(nullptr),
	m_datasize(0)
{

}

DX12Customblob::DX12Customblob(size_t datasize)
	:m_datasize(datasize)
{
	Init(datasize);
}
DX12Customblob::~DX12Customblob()
{
	if (m_data)
	{
		delete[] m_data;
		m_data = nullptr;
	}
}

void DX12Customblob::Init(size_t datasize)
{
	m_datasize = datasize;
	m_data = new byte[datasize];
}
void DX12Customblob::Init(size_t datasize, void* srcdata)
{
	Init(datasize);

	if (datasize==0)
	{
		return;
	}

	memcpy(m_data, srcdata, datasize);
}