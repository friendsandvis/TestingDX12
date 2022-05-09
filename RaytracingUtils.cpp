#include"RaytracingUtils.h"

void ModelAccelerationStructure::Init(Model& modeltoprocess)
{
	//we need ib &vb in gpu memory.
	assert(modeltoprocess.GetUploadMode() == ModelDataUploadMode::COPY);
	m_rtgeometrydesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE::D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	//fixed in the current frame work(index is unsigned int)
	m_rtgeometrydesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	m_rtgeometrydesc.Triangles.IndexBuffer = modeltoprocess.GetIertexBufferGPUVirtualAddress();
	m_rtgeometrydesc.Triangles.IndexCount = (UINT)modeltoprocess.GetIndiciesCount();

	m_rtgeometrydesc.Triangles.VertexBuffer.StartAddress = modeltoprocess.GetVertexBufferGPUVirtualAddress();
	
	//stride is the same as vertex size used in model
	m_rtgeometrydesc.Triangles.VertexBuffer.StrideInBytes = DXVertexManager::GetVertexSize(modeltoprocess.GetVertexVersionUsed());
	//a vertex position always has same format no matter the vertex version used
	m_rtgeometrydesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	m_rtgeometrydesc.Triangles.VertexCount = modeltoprocess.GetVerticiesCount();
	m_rtgeometrydesc.Triangles.Transform3x4 = NULL;
	
	
}