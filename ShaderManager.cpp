#include"ShaderManager.h"
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment(lib,"dxcompiler.lib")
#define DEBUGSHADER


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
#ifdef USEDXC
	
		DXCCOMPILEParams dxccompileparams = {};
	dxccompileparams.sourcehlslfilepath = sourcehlslfilepath;
	dxccompileparams.entrypoint = L"main";
	dxccompileparams.out_compiledcodeblob = &m_compiledcode;
	dxccompileparams.out_errorblob = &m_errormsgs;
#ifdef DEBUGSHADER
	dxccompileparams.args.push_back(L"-Zi");
	dxccompileparams.args.push_back(L"-Qembed_debug");
#endif // DEBUGSHADER

	
	
	
	


	switch (m_shadertype)
	{
	case VS:
		dxccompileparams.targetprofile = L"vs_6_5"; break;
	case PS:
		dxccompileparams.targetprofile = L"ps_6_5"; break;
	case CS:
		dxccompileparams.targetprofile = L"cs_6_5"; break;

	}
	DXCmanager::s_manager.CompileShader(dxccompileparams);

#endif // USEDXC

#ifndef USEDXC
	FXCCOMPILEParams fxccompileparams = {};
	fxccompileparams.sourcehlslfilepath = sourcehlslfilepath;
	fxccompileparams.entrypoint = "main";
#ifdef DEBUGSHADER
	fxccompileparams.flag1 = D3DCOMPILE_DEBUG;
#endif // DEBUGSHADER

	
	fxccompileparams.flag2 = 0;
	fxccompileparams.out_compiledcodeblob = &m_compiledcode;
	fxccompileparams.out_errorblob = &m_errormsgs;
	switch (m_shadertype)
	{
	case VS:
		fxccompileparams.target ="vs_5_0"; break;
	case PS:
		fxccompileparams.target = "ps_5_0"; break;

	case CS:
		fxccompileparams.target = "cs_5_0"; break;

	}
	FXCManager::s_manager.CompileShader(fxccompileparams);
#endif // !USEDXC

	

	

	//FXCManager::s_manager.CompileShader(fxccompileparams);
	



	return (m_errormsgs.m_datasize == 0);



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


uint32_t DXCmanager::codePage=CP_UTF8;
DXCmanager DXCmanager::s_manager;

DXCmanager::DXCmanager()

{
	DXASSERT(DxcCreateInstance(CLSID_DxcLibrary,IID_PPV_ARGS(m_lib.GetAddressOf())))
		DXASSERT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_compiler.GetAddressOf())))

}

DXCmanager::~DXCmanager()
{
}

void DXCmanager::CompileShader(DXCCOMPILEParams& compileparams)
{
	
	ComPtr<IDxcOperationResult> opres;
	ComPtr<IDxcBlobEncoding> hlslsource;
	DXASSERT(m_lib->CreateBlobFromFile(compileparams.sourcehlslfilepath, &codePage, hlslsource.GetAddressOf()))

		DXASSERT(m_compiler->Compile(hlslsource.Get(), compileparams.sourcehlslfilepath, compileparams.entrypoint, compileparams.targetprofile, compileparams.args.data(), compileparams.args.size(), nullptr, 0, nullptr, opres.GetAddressOf()))

		ComPtr<IDxcBlob> rescode;
		ComPtr<IDxcBlobEncoding> errors;
	DXASSERT(opres->GetErrorBuffer(errors.GetAddressOf()))
	DXASSERT(opres->GetResult(rescode.GetAddressOf()))

		//copy over the compiled code and errors to out blobs
		if (errors.Get())
		{
			compileparams.out_errorblob->Init(errors->GetBufferSize(), errors->GetBufferPointer());
			if (errors->GetBufferSize())
			{
				DebugBreak();
			}
		}
		if (rescode.Get())
		{
			compileparams.out_compiledcodeblob->Init(rescode->GetBufferSize(), rescode->GetBufferPointer());
		}
	
}

FXCManager FXCManager::s_manager;
FXCManager::FXCManager()
{}
FXCManager::~FXCManager()
{

}
void FXCManager::CompileShader(FXCCOMPILEParams& compileparams)
{
	ComPtr<ID3DBlob> errors, outputcode;
	HRESULT res =
		D3DCompileFromFile(compileparams.sourcehlslfilepath, nullptr, nullptr, compileparams.entrypoint, compileparams.target, compileparams.flag1, compileparams.flag2, outputcode.GetAddressOf(), errors.GetAddressOf());

	//copy code to local blob
	if (errors.Get())
	{

		compileparams.out_errorblob->Init(errors->GetBufferSize(), errors->GetBufferPointer());
		DebugBreak();
	}
	compileparams.out_compiledcodeblob->Init(outputcode->GetBufferSize(), outputcode->GetBufferPointer());
	


}