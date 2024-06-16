#pragma once
#include<string>
#include"EntityRenderer.h"
#include"BasicRenderableEntity.h"
//saves a list of entities with properties(aka scene) in a file
/*
* eg save steps:
* m_basicEntityrenderer.AddEntity(m_basicCubeEntitysharedPtr);
	m_basicEntityrenderer.AddEntity(m_basicPlaneEntitysharedPtr);
	SceneSerializer::SaveScene("testfile1.sce", &m_basicEntityrenderer);
*/
class SceneSerializer
{
public:
	struct EntityBasicData
	{
		char name[10];
		unsigned entityType;
		DirectX::XMFLOAT4 scale;
		DirectX::XMFLOAT4 translate;
		DirectX::XMFLOAT4 rotationaxis;
		float rotationangle;


	};
	static void SaveScene(std::string fileName, EntityRenderer* renderer);
	static void LoadScene(std::string fileName, std::vector<std::shared_ptr<Entity>>& outLoadedEntities);

private:
	static void SaveEntity(std::ofstream& savefilestream,std::shared_ptr<Entity> entity);
	static void SaveBasicRenderableEntity(std::ofstream& savefilestream, std::shared_ptr<BasicRenderableEntity> entity);
	static std::shared_ptr<Entity> LoadEntity(std::ifstream& scenefilestream);
	static void FetchEntityBasicData(std::shared_ptr<Entity> entity,EntityBasicData& OutentityBasicdata);
	static void SetEntityBasicDataCommon(EntityBasicData& entityBasicdata,std::shared_ptr<Entity> entity);
	static std::shared_ptr<BasicRenderableEntity> LoadBasicRenderableEntity(std::ifstream& scenefilestream);

};