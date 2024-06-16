#include"BasicRenderableEntity.h"

BasicRenderableEntity::BasicRenderableEntity()
	:m_model(ModelDataUploadMode::COPY),
	 m_modelType(MODELTYPE::BASIC_CUBE)

{
	m_entityType = ENTITYTYPE::BASICRENDERABLEENTITY;
}
BasicRenderableEntity::~BasicRenderableEntity()
{

}
void BasicRenderableEntity::Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& cmdlist)
{
	switch (m_modelType)
	{
	case MODELTYPE::BASIC_CUBE:
		BasicModelManager::InitCubeModel(creationdevice, m_model);
		break;
	case MODELTYPE::BASIC_PLANE:
		BasicModelManager::InitPlaneModel(creationdevice, m_model);
		break;
	default:
		break;
	}
	m_model.UploadModelDatatoGPUBuffers(cmdlist);
}
BasicRenderableEntity::BasicRenderableEntity(MODELTYPE modeltype)
	:m_modelType(modeltype)
{
	m_entityType = ENTITYTYPE::BASICRENDERABLEENTITY;
}

void BasicRenderableEntity::Render(DX12Commandlist& cmdlist)
{
	D3D12_VERTEX_BUFFER_VIEW vbview=m_model.GetVBView();
	D3D12_INDEX_BUFFER_VIEW Ibview= m_model.GetIBView();
	cmdlist->IASetVertexBuffers(0, 1, &vbview);
	cmdlist->IASetIndexBuffer(&Ibview);
	cmdlist->DrawIndexedInstanced(m_model.GetIndiciesCount(), 1, 0, 0, 0);
}

BasicEntityMultiPlaneCube::BasicEntityMultiPlaneCube()
{
	m_PlaneEntityCubeFacePYsharedPtr = std::make_shared<BasicRenderableEntity>(MODELTYPE::BASIC_PLANE);
	m_PlaneEntityCubeFaceNYsharedPtr = std::make_shared<BasicRenderableEntity>(MODELTYPE::BASIC_PLANE);
	m_PlaneEntityCubeFacePZsharedPtr = std::make_shared<BasicRenderableEntity>(MODELTYPE::BASIC_PLANE);
	m_PlaneEntityCubeFaceNZsharedPtr = std::make_shared<BasicRenderableEntity>(MODELTYPE::BASIC_PLANE);
	m_PlaneEntityCubeFacePXsharedPtr = std::make_shared<BasicRenderableEntity>(MODELTYPE::BASIC_PLANE);
	m_PlaneEntityCubeFaceNXsharedPtr = std::make_shared<BasicRenderableEntity>(MODELTYPE::BASIC_PLANE);

}
void BasicEntityMultiPlaneCube::Init(ComPtr< ID3D12Device> creationdevice, DX12Commandlist& uploadcmdlist)
{
	BasicMaterialData basicMatData = {};
	//Positive Y
	m_PlaneEntityCubeFacePYsharedPtr->Init(creationdevice, uploadcmdlist);
	m_PlaneEntityCubeFacePYsharedPtr->SetName("PlanePY");
	m_PlaneEntityCubeFacePYsharedPtr->SetTransformationData(1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), 90.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));
	basicMatData.m_albedo = { 1.0f,1.0f,0.0f,1.0f };
	m_PlaneEntityCubeFacePYsharedPtr->SetBasicMaterialData(basicMatData);
	m_cubeFacePlanes[0] = m_PlaneEntityCubeFacePYsharedPtr;

	//Negitive Y
	m_PlaneEntityCubeFaceNYsharedPtr->Init(creationdevice, uploadcmdlist);
	m_PlaneEntityCubeFaceNYsharedPtr->SetName("PlaneNY");
	m_PlaneEntityCubeFaceNYsharedPtr->SetTransformationData(1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), 90.0f, DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f));
	basicMatData.m_albedo = { 1.0f,0.0f,0.0f,1.0f };
	m_PlaneEntityCubeFaceNYsharedPtr->SetBasicMaterialData(basicMatData);
	m_cubeFacePlanes[1] = m_PlaneEntityCubeFaceNYsharedPtr;

	//Positive Z
	m_PlaneEntityCubeFacePZsharedPtr->Init(creationdevice, uploadcmdlist);
	m_PlaneEntityCubeFacePZsharedPtr->SetName("PlanePZ");
	m_PlaneEntityCubeFacePZsharedPtr->SetTransformationData(1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f));
	basicMatData.m_albedo = { 0.0f,1.0f,0.0f,1.0f };
	m_PlaneEntityCubeFacePZsharedPtr->SetBasicMaterialData(basicMatData);
	m_cubeFacePlanes[2] = m_PlaneEntityCubeFacePZsharedPtr;

	//Negitive Z
	m_PlaneEntityCubeFaceNZsharedPtr->Init(creationdevice, uploadcmdlist);
	m_PlaneEntityCubeFaceNZsharedPtr->SetName("PlaneNZ");
	m_PlaneEntityCubeFaceNZsharedPtr->SetTransformationData(1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f), 0.0f, DirectX::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f));
	basicMatData.m_albedo = { 0.0f,0.0f,1.0f,1.0f };
	m_PlaneEntityCubeFaceNZsharedPtr->SetBasicMaterialData(basicMatData);
	m_cubeFacePlanes[3] = m_PlaneEntityCubeFaceNZsharedPtr;

	//Positive X
	m_PlaneEntityCubeFacePXsharedPtr->Init(creationdevice, uploadcmdlist);
	m_PlaneEntityCubeFacePXsharedPtr->SetName("PlanePX");
	m_PlaneEntityCubeFacePXsharedPtr->SetTransformationData(1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), 90.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f));
	basicMatData.m_albedo = { 1.0f,0.0f,1.0f,1.0f };
	m_PlaneEntityCubeFacePXsharedPtr->SetBasicMaterialData(basicMatData);
	m_cubeFacePlanes[4] = m_PlaneEntityCubeFacePXsharedPtr;

	//Negitive X
	m_PlaneEntityCubeFaceNXsharedPtr->Init(creationdevice, uploadcmdlist);
	m_PlaneEntityCubeFaceNXsharedPtr->SetName("PlanePX");
	m_PlaneEntityCubeFaceNXsharedPtr->SetTransformationData(1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), 90.0f, DirectX::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f));
	basicMatData.m_albedo = { 0.0f,1.0f,1.0f,1.0f };
	m_PlaneEntityCubeFaceNXsharedPtr->SetBasicMaterialData(basicMatData);
	m_cubeFacePlanes[5] = m_PlaneEntityCubeFaceNXsharedPtr;
}