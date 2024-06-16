#pragma once
#include"RenderableEntity.h"
#include"AssetManager.h"

enum class MODELTYPE: unsigned
{
	BASIC_CUBE = 1,
	BASIC_PLANE = 2
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
//just a colection of 6 diffrent coloured planes to represent a cube
struct BasicEntityMultiPlaneCube
{
	std::shared_ptr<BasicRenderableEntity> m_PlaneEntityCubeFacePYsharedPtr, m_PlaneEntityCubeFaceNYsharedPtr, m_PlaneEntityCubeFacePXsharedPtr, m_PlaneEntityCubeFaceNXsharedPtr, m_PlaneEntityCubeFacePZsharedPtr, m_PlaneEntityCubeFaceNZsharedPtr;
	std::shared_ptr<BasicRenderableEntity> m_cubeFacePlanes[6];

	BasicEntityMultiPlaneCube();
	void Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& uploadcmdlist);


};