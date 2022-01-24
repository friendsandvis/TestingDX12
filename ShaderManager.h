#pragma once

#include<d3dcompiler.h>


class DX12Shader
{
public:
	DX12Shader();
	~DX12Shader();

	enum ShaderType
	{
		VS,
		PS
	};


	bool Init(LPCWSTR sourcehlslfilepath, ShaderType shadertype);
	ID3DBlob* GetByteCode() { return m_outputbytecode; }
private:
	ID3DBlob* m_outputbytecode;
	ID3DBlob* m_errormsgs;
	LPCWSTR m_sourcehlslfilename;
	ShaderType m_shadertype;
};