#include"AssimpManager.h"
#include<assert.h>
#pragma comment(lib,"assimp-vc140-mt.lib")



AssimpManager::AssimpManager(std::string filename)
{
	Assimp::Importer animporter;
	m_scene = animporter.ReadFile(filename,aiProcess_MakeLeftHanded);
	
	if (m_scene == nullptr)
	{
		const char* errormsg = animporter.GetErrorString();
		__debugbreak();
	}
	ProcessNode(m_scene->mRootNode);

}
AssimpManager::~AssimpManager()
{

}



void AssimpManager::ProcessNode(aiNode* annode)
{
	aiMatrix4x4 finaltransform=annode->mTransformation;
	if (annode->mParent)
	{
		finaltransform=finaltransform*annode->mTransformation;
	}

	for (unsigned i = 0; i < annode->mNumMeshes;i++)
	{
		unsigned scenemeshidx = annode->mMeshes[i];
		aiMesh* scenemesh=m_scene->mMeshes[scenemeshidx];
		ProcessMesh(scenemesh,finaltransform);
	}
	for (unsigned i = 0; i < annode->mNumChildren;i++)
	{
		ProcessNode(annode->mChildren[i]);
	}
}

void AssimpManager::ProcessMesh(aiMesh* amesh, aiMatrix4x4 finaltrasform)
{
	m_processedmodel.m_meshes.push_back(AssimpLoadedMesh());
	AssimpLoadedMesh& aprocessedmesh=m_processedmodel.m_meshes[m_processedmodel.m_meshes.size()-1];
	aprocessedmesh.transform = finaltrasform;

	for (unsigned i0 = 0; i0 < amesh->mNumVertices; i0++)
	{
		AssimpLoadedVertex ameshvert = {};
		aiVector3D apos=amesh->mVertices[i0];
		ameshvert.pos.x = apos.x;
		ameshvert.pos.y = apos.y;
		ameshvert.pos.z = apos.z;
		if (amesh->HasTextureCoords(0))
		{
			aiVector3D auvw =amesh->mTextureCoords[0][i0];
			ameshvert.uv.x = auvw.x;
			ameshvert.uv.y = auvw.y;
		}
		if (amesh->HasNormals())
		{
			aiVector3D anormal =amesh->mNormals[i0];
			ameshvert.normal.x = anormal.x;
			ameshvert.normal.y = anormal.y;
			ameshvert.normal.z = anormal.z;
		}
		aprocessedmesh.verticies.push_back(ameshvert);

	}
	for (unsigned i1 = 0; i1 < amesh->mNumFaces; i1++)
	{
		aiFace face=amesh->mFaces[i1];
		
		for (unsigned i = 0; i < face.mNumIndices; i++)
		{
			aprocessedmesh.indicies.push_back(face.mIndices[i]);
		}
	}

}

XMMATRIX AssimpManager::ToXMMatrix(aiMatrix4x4 assimpmatrix)
{
	float* matdata=(float*)&assimpmatrix;
	XMMATRIX outmat(matdata);
	outmat = XMMatrixTranspose(outmat);
	return outmat;

}