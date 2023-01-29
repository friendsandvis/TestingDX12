#pragma once
#include<string.h>
#include<vector>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<dxmath/DirectXMath.h>
#include<set>
#include<string>

using namespace std;
using namespace DirectX;


struct AssimpLoadedVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};
class MeshMaterial
{
public:
	void AddDiffuseTexture(std::string diffusetexpath) { m_diffusetexturesPath.insert(diffusetexpath); }
	void AddNormalTexture(std::string normaltexpath) { m_normaltexturesPath.insert(normaltexpath); }
	void AddAmbientTexture(std::string ambienttexpath) { m_ambienttexturesPath.insert(ambienttexpath); }
private:
	std::set<std::string> m_diffusetexturesPath,m_normaltexturesPath, m_ambienttexturesPath;
};
struct AssimpLoadedMesh
{
	vector<AssimpLoadedVertex> verticies;
	vector<unsigned> indicies;
	aiMatrix4x4 transform;
	MeshMaterial material;
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

