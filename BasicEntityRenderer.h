#pragma once
#include"BasicRenderableEntity.h"
#include"EntityRenderer.h"
#include"DX12PSO.h"

class BasicEntityRenderer :public EntityRenderer
{
public:
	BasicEntityRenderer();
	~BasicEntityRenderer();
	void Init(ComPtr< ID3D12Device> creationdevice)override;
	void Render(DX12Commandlist& cmdlist, RenderData& renderData) override;
private:
protected:
	struct SimpleMaterial
	{
		XMFLOAT4 colour;
	};
	SimpleMaterial m_mat;
	ShaderTransformConstants_General m_shadertransformconsts;
	DX12PSO m_basicpso;

};

