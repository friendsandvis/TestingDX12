#include"RaytracingUtils.h"

void ModelAccelerationStructure::Init(Model& modeltoprocess)
{
	assert(modeltoprocess.GetUploadMode() == ModelDataUploadMode::COPY);
	m_rtgeometrydesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE::D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	
}