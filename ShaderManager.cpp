#include"ShaderManager.h"
#pragma comment(lib,"D3DCompiler.lib")


DX12Shader::DX12Shader()
	:m_sourcehlslfilename(L""),
	m_outputbytecode(nullptr),
	m_errormsgs(nullptr),
	m_shadertype(VS)
{
}

DX12Shader::~DX12Shader()
{
	if (m_errormsgs)
	{
		m_errormsgs->Release();
	}

	if (m_outputbytecode)
	{
		m_outputbytecode->Release();
	}

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

	HRESULT res =
		D3DCompileFromFile(m_sourcehlslfilename, nullptr, nullptr, "main", target, 0, 0, &m_outputbytecode, &m_errormsgs);

	if (m_errormsgs)
	{
		const char* errormsg = (char*)m_errormsgs->GetBufferPointer();
		//DebugBreak();
		OutputDebugStringA(errormsg);
	}

	return ((res == S_OK) && (m_errormsgs == nullptr));



}