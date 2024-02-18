#pragma once
#include"RenderableEntity.h"
#include"AssetManager.h"

enum class MODELTYPE
{
	BASIC_CUBE,
	BASIC_PLANE
};
struct BasicMaterialData
{
	DirectX::XMFLOAT4 m_albedo;
};
class BasicRenderableEntity :public RenderableEntity
{
public:
	BasicRenderableEntity();
	BasicRenderableEntity(MODELTYPE modeltype);
	~BasicRenderableEntity();
	inline void SetBasicMaterialData(BasicMaterialData matdata)
	{
		m_materialData = matdata;
	}
	BasicMaterialData& GetBasicMaterialData()
	{
		return m_materialData;
	}
	virtual void Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& cmdlist)override;
	virtual void Render(DX12Commandlist& cmdlist)override;
	MODELTYPE GetModelType() { return m_modelType; }

private:
	MODELTYPE m_modelType;
	Model m_model;
	BasicMaterialData m_materialData;
};
