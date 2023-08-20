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
	std::set<std::string>& GetDiffuseTextureNames() { return m_diffusetextureNames; }
	std::set<std::string>& GetNormalTextureNames() { return m_normaltextureNames; }
	std::set<std::string>& GetAmbientTextureNames() { return m_ambienttextureNames; }
	void AddDiffuseTexture(std::string diffusetexpath) { m_diffusetextureNames.insert(diffusetexpath); }
	void AddNormalTexture(std::string normaltexpath) { m_normaltextureNames.insert(normaltexpath); }
	void AddAmbientTexture(std::string ambienttexpath) { m_ambienttextureNames.insert(ambienttexpath); }
private:
	std::set<std::string> m_diffusetextureNames,m_normaltextureNames, m_ambienttextureNames;
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

