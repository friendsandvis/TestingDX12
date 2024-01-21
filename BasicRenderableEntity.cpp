#include"BasicRenderableEntity.h"

BasicRenderableEntity::BasicRenderableEntity()
	:m_model(ModelDataUploadMode::COPY)

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

void BasicRenderableEntity::Render(DX12Commandlist& cmdlist)
{
	D3D12_VERTEX_BUFFER_VIEW vbview=m_model.GetVBView();
	D3D12_INDEX_BUFFER_VIEW Ibview= m_model.GetIBView();
	cmdlist->IASetVertexBuffers(0, 1, &vbview);
	cmdlist->IASetIndexBuffer(&Ibview);
	cmdlist->DrawIndexedInstanced(m_model.GetIndiciesCount(), 1, 0, 0, 0);
}