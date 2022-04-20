#include"AssimpManager.h"
#include<assert.h>



AssimpManager::AssimpManager(std::string filename)
{
	Assimp::Importer animporter;
	m_scene = animporter.ReadFile(filename, aiProcess_Triangulate);
	assert(m_scene != nullptr);
	ProcessNode(m_scene->mRootNode);

}
AssimpManager::~AssimpManager()
{

}



void AssimpManager::ProcessNode(aiNode* annode)
{
	for (unsigned i = 0; i < annode->mNumMeshes;i++)
	{
		unsigned scenemeshidx = annode->mMeshes[i];
		aiMesh* scenemesh=m_scene->mMeshes[scenemeshidx];
		ProcessMesh(scenemesh);
	}
	for (unsigned i = 0; i < annode->mNumChildren;i++)
	{
		ProcessNode(annode->mChildren[i]);
	}
}

void AssimpManager::ProcessMesh(aiMesh* amesh)
{
	AssimpLoadedMesh aprocessedmesh;
	for (unsigned i = 0; i < amesh->mNumVertices; i++)
	{
		Vertex ameshvert = {};
		aiVector3D apos=amesh->mVertices[i];
		ameshvert.X = apos.x;
		ameshvert.Y = apos.y;
		ameshvert.Z = apos.z;
		if (amesh->HasTextureCoords(0))
		{
			aiVector3D auvw =amesh->mTextureCoords[0][i];
			ameshvert.U = auvw.x;
			ameshvert.V = auvw.y;
		}

	}
}