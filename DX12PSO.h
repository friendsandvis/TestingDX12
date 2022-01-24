#pragma once
#include"DXUtils.h"
#include"ShaderManager.h"

enum PSOType
{
	COMPUTE,
	GRAPHICS
};

union PSODESC
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicspsodesc;
	D3D12_COMPUTE_PIPELINE_STATE_DESC computepsodesc;

};
	struct PSOInitData
{
		PSOType type;
		PSODESC psodesc;
		std::vector<DX12Shader*> m_shaderstouse;

		
};

class DX12PSO
{
public:
	DX12PSO();
	~DX12PSO();

	void Init(ComPtr< ID3D12Device> creationdevice,PSOInitData initdata);

private:
	ComPtr< ID3D12PipelineState> m_pso;
	PSOInitData m_initdata;
	 
};

