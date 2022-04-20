#pragma once
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<string.h>
#include<vector>
using namespace std;

struct Vertex
{
	float X;
	float Y;
	float Z;
	float U;
	float V;
};

struct AssimpLoadedMesh
{
	vector<Vertex> verticies;
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

