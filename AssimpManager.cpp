#include"AssimpManager.h"
#include<assert.h>
#include"DXTexManager.h"
#include<assimp/StringUtils.h>
#include<assimp/material.h>
#pragma comment(lib,"assimp-vc143-mt.lib")



AssimpManager::AssimpManager(std::string filename, wstring texfilepath)
{
	m_texturefilepath = texfilepath;
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
		finaltransform=finaltransform* annode->mParent->mTransformation;
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
	{
		const aiMaterial* const mat=m_scene->mMaterials[amesh->mMaterialIndex];
		
		
		//retrive diffuse textures.
		unsigned diffusetexcount = mat->GetTextureCount(aiTextureType::aiTextureType_BASE_COLOR);
		for (unsigned i = 0; i < diffusetexcount; i++)
		{
			aiString texname;
			mat->GetTexture(aiTextureType_BASE_COLOR, i, &texname);
			aprocessedmesh.material.AddDiffuseTexture(texname.C_Str());
			std::string texfilename = texname.C_Str();
			wstring texfilenamewstr(texfilename.begin(), texfilename.end());
			wstring texpath = m_texturefilepath + texfilenamewstr;
			if (DXTexManager::IsTextureTransparent(texpath.c_str()))
			{
				aiBlendMode blend_mode = aiBlendMode::aiBlendMode_Default;
				aiString blend_mode_str;
				if (mat->Get(AI_MATKEY_BLEND_FUNC, blend_mode_str) == aiReturn_SUCCESS) {
					blend_mode = (aiBlendMode)std::stoi(blend_mode_str.C_Str());
				}
				aprocessedmesh.material.SetTransparent(true);
			}
		}
		//retrive normal map textures
		unsigned normaltexcount = mat->GetTextureCount(aiTextureType_NORMALS);
		for (unsigned i = 0; i < normaltexcount; i++)
		{
			aiString texname;
			mat->GetTexture(aiTextureType_NORMALS, i, &texname);
			aprocessedmesh.material.AddNormalTexture(texname.C_Str());
		}
		//retrive metalness map textures
		unsigned metalnesstexcount = mat->GetTextureCount(aiTextureType::aiTextureType_METALNESS);
		for (unsigned i = 0; i < metalnesstexcount; i++)
		{
			aiString texname;
			mat->GetTexture(aiTextureType_METALNESS, i, &texname);
			aprocessedmesh.material.AddMetalnessTexture(texname.C_Str());
		}
		//retrive roughness map textures
		unsigned roughnesstexcount = mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS);
		for (unsigned i = 0; i < roughnesstexcount; i++)
		{
			aiString texname;
			mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, i, &texname);
			aprocessedmesh.material.AddRoughnessTexture(texname.C_Str());
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