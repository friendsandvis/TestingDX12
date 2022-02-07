#pragma once

#include"DXUtils.h"
#include<d3dcompiler.h>
#include<Windows.h>
#include<DXC/dxcapi.h>


struct DX12Customblob
{
	DX12Customblob();
	DX12Customblob(size_t datasize);
	~DX12Customblob();

	//init blob with expected data size
	void Init(size_t datasize);
	//init blob with size and data that will be copied locally(srcdata expected to be of datasize)
	void Init(size_t datasize,void* srcdata);
	size_t m_datasize;
	byte* m_data;
};

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
	size_t GetCompiledCodeSize() { return m_compiledcode.m_datasize; }
	void* GetCompiledCode();
private:
	DX12Customblob m_compiledcode;
	DX12Customblob m_errormsgs;
	LPCWSTR m_sourcehlslfilename;
	ShaderType m_shadertype;
};