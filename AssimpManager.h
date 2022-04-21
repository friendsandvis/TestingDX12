#pragma once
#include<string.h>
#include<vector>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<dxmath/DirectXMath.h>

using namespace std;
using namespace DirectX;


struct AssimpLoadedVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

struct AssimpLoadedMesh
{
	vector<AssimpLoadedVertex> verticies;
	vector<unsigned> indicies;
};
struct AssimpLoadedModel
{
	vector<AssimpLoadedMesh> m_meshes;
};

class AssimpManager
{
public:
	AssimpManager(std::string filename);
	~AssimpManager();
private:
	void ProcessNode(aiNode* annode);
	void ProcessMesh(aiMesh* amesh);
	const aiScene* m_scene;
	AssimpLoadedModel m_processedmodel;
};

