#include"SceneSerializer.h"
#include<iostream>
#include<fstream>
#include<stdio.h>

void SceneSerializer::SaveScene(std::string fileName, EntityRenderer* renderer)
{
	unsigned entityCount = std::distance(renderer->m_entityList.begin(), renderer->m_entityList.end());

	//open binary file
	
	std::ofstream scenefileout("scenes/sceneOut.sce", ios::out | ios::binary);
	assert(scenefileout);
	char* data = reinterpret_cast<char*>(&entityCount);
	scenefileout.write(data, sizeof(unsigned));

	for (std::shared_ptr<Entity> entity : renderer->m_entityList)
	{
		SaveEntity(scenefileout, entity);


	}
}
void SceneSerializer::SaveEntity(std::ofstream& savefilestream, std::shared_ptr<Entity> entity)
{
	EntityBasicData entityBasicData = {};
	FetchEntityBasicData(entity, entityBasicData);
	char* data = reinterpret_cast<char*>(&entityBasicData);
	savefilestream.write(data, sizeof(EntityBasicData));

	/*
	//write Name
	savefilestream.write(entity->m_name.c_str(), entity->m_name.size());

	//write transforms
	//scale
	char* data = reinterpret_cast<char*>(&entity->m_scale);
	savefilestream.write(data, sizeof(entity->m_scale));
	//translate
	data = reinterpret_cast<char*>(&entity->m_translate);
	savefilestream.write(data, sizeof(entity->m_translate));
	//rotation axis
	data = reinterpret_cast<char*>(&entity->m_rotationaxis);
	savefilestream.write(data, sizeof(entity->m_rotationaxis));
	//rotation angle
	data = reinterpret_cast<char*>(&entity->m_rotationangle);
	savefilestream.write(data, sizeof(entity->m_rotationangle));
	*/
	if (entity->GetEntityType() == ENTITYTYPE::BASICRENDERABLEENTITY)
	{
		SaveBasicRenderableEntity(savefilestream, std::dynamic_pointer_cast<BasicRenderableEntity>(entity));
	}
}
void SceneSerializer::SaveBasicRenderableEntity(std::ofstream& savefilestream, std::shared_ptr<BasicRenderableEntity> entity)
{
	//save model type
	unsigned modelType = static_cast<unsigned>(entity->GetModelType());
	char* data = reinterpret_cast<char*>(&modelType);
	savefilestream.write(data, sizeof(modelType));
	//save basic material
	BasicMaterialData matData = entity->GetBasicMaterialData();
	data = reinterpret_cast<char*>(&matData);
	savefilestream.write(data, sizeof(matData));

}
void SceneSerializer::LoadScene(std::string fileName, std::vector<std::shared_ptr<Entity>>& outLoadedEntities)
{
	std::ifstream scenefilein("scenes/sceneOut.sce", ios::in | ios::binary);
	assert(scenefilein);
	//read num entity
	unsigned entityCount;
	char* data = reinterpret_cast<char*>(&entityCount);
	scenefilein.read(data, sizeof(unsigned));
	//read entity data
	for (unsigned i = 0; i < entityCount; i++)
	{
		std::shared_ptr<Entity> entity = LoadEntity(scenefilein);
		outLoadedEntities.push_back(entity);
	}

}
std::shared_ptr<Entity> SceneSerializer::LoadEntity(std::ifstream& scenefilestream)
{
	std::shared_ptr<Entity> outEntity;
	EntityBasicData entityBasicData = {};
	char* data = reinterpret_cast<char*>(&entityBasicData);
	scenefilestream.read(data, sizeof(EntityBasicData));

	if (entityBasicData.entityType == static_cast<unsigned>(ENTITYTYPE::BASICRENDERABLEENTITY))
	{
		std::shared_ptr<BasicRenderableEntity> entity = LoadBasicRenderableEntity(scenefilestream);
		outEntity = entity;
	}
	else
	{
		outEntity = std::make_shared<Entity>();
	}
	SetEntityBasicDataCommon(entityBasicData, outEntity);
	return outEntity;
}
void SceneSerializer::FetchEntityBasicData(std::shared_ptr<Entity> entity, EntityBasicData& OutentityBasicdata)
{
	strcpy_s(OutentityBasicdata.name, entity->m_name.c_str());
	OutentityBasicdata.entityType = static_cast<unsigned>(entity->GetEntityType());
	OutentityBasicdata.scale = entity->m_scale;
	OutentityBasicdata.translate = entity->m_translate;
	OutentityBasicdata.rotationaxis = entity->m_rotationaxis;
	OutentityBasicdata.rotationangle = entity->m_rotationangle;
	
}
void SceneSerializer::SetEntityBasicDataCommon(EntityBasicData& entityBasicdata, std::shared_ptr<Entity> entity)
{
	entity->m_name = entityBasicdata.name;
	//do not set entity type as it is not must have  property for common data
	entity->m_scale = entityBasicdata.scale;
	entity->m_translate = entityBasicdata.translate;
	entity->m_rotationaxis = entityBasicdata.rotationaxis;
	entity->m_rotationangle = entityBasicdata.rotationangle;

}
std::shared_ptr<BasicRenderableEntity> SceneSerializer::LoadBasicRenderableEntity(std::ifstream& scenefilestream)
{
	unsigned modelType;
	char* data = reinterpret_cast<char*>(&modelType);
	scenefilestream.read(data, sizeof(modelType));
	//read basic material
	BasicMaterialData matData;
	data = reinterpret_cast<char*>(&matData);
	scenefilestream.read(data, sizeof(matData));

	std::shared_ptr<BasicRenderableEntity> entity = std::make_shared<BasicRenderableEntity>(static_cast<MODELTYPE>(modelType));
	entity->SetBasicMaterialData(matData);
	return entity;
}


