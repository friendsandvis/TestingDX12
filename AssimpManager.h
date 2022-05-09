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
	aiMatrix4x4 transform;
};
struct AssimpLoadedModel
{
	vector<AssimpLoadedMesh> m_meshes;
};

class AssimpManager
{
public:
	AssimpLoadedModel& GetProcessedModel() { return m_processedmodel; }
	AssimpManager(std::string filename);
	~AssimpManager();
	static XMMATRIX ToXMMatrix(aiMatrix4x4 assimpmatrix);
private:
	void ProcessNode(aiNode* annode);
	void ProcessMesh(aiMesh* amesh,aiMatrix4x4 trasform);
	
	const aiScene* m_scene;
	AssimpLoadedModel m_processedmodel;
};

