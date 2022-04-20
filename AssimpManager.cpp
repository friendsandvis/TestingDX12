#include"AssimpManager.h"

AssimpManager::AssimpManager()
{
}

AssimpManager::~AssimpManager()
{
}

void AssimpManager::LoadModelFromFile(std::string filename, AssimpLoadedModel& outmodel)
{
	Assimp::Importer animporter;
	const aiScene* animportedscene=animporter.ReadFile(filename, aiProcess_Triangulate);
	ProcessNode(animportedscene->mRootNode, outmodel);
}

void AssimpManager::ProcessNode(aiNode* anode, AssimpLoadedModel& outmodel)
{
}