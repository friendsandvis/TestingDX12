#include "ps5ModelDataExporter.h"
#include "AssetManager.h"
#include<string>
#include <vector>

void PS5ModelExporter::Export(Model& model, std::string outfilename)
{
	std::ofstream outFile = std::ofstream(outfilename, std::ios::binary | std::ios::ate);
	//write num models
	uint8_t numModels = 1;
	outFile.write(reinterpret_cast<char*>(&numModels), sizeof(numModels));
	WriteModelExportData(model, outFile);
	outFile.close();
}
void PS5ModelExporter::WriteModelExportData(Model& model, std::ofstream& outFile, bool writeDiffuseTexName)
{
	//has indicies
	bool hasIndicies = model.HasIndexBuffer();
	outFile.write(reinterpret_cast<char*>(&hasIndicies), sizeof(hasIndicies));
	//export vertexdata
	std::vector<float> vertexData;
	GetVertexData(model, vertexData);
	uint32_t vertexDataCount = static_cast<uint32_t>(vertexData.size());
	//num floats in vertex data
	outFile.write(reinterpret_cast<char*>(&vertexDataCount), sizeof(vertexDataCount));
	outFile.write(reinterpret_cast<char*>(vertexData.data()), sizeof(vertexData[0]) * vertexDataCount);
	//export index Data
	if (hasIndicies)
	{
		std::vector<unsigned int> indiciesData = model.Getindicies();
		uint32_t indexDataCount = static_cast<uint32_t>(indiciesData.size());
		outFile.write(reinterpret_cast<char*>(&indexDataCount), sizeof(indexDataCount));
		outFile.write(reinterpret_cast<char*>(indiciesData.data()), sizeof(indiciesData[0]) * indexDataCount);
	}
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform,model.GetTransform());
	//write transform matrix data
	const size_t transformMatSize = sizeof(transform);
	assert(transformMatSize == (sizeof(float) * 16));
	outFile.write(reinterpret_cast<char*>(&transform), transformMatSize);
	//texture name
	if (writeDiffuseTexName)
	{
		std::wstring texFileNamewstr = model.GetLoadedMaterial().GetDiffuseTexture()->GetExternalTextureFileName();
		std::string texFileName(texFileNamewstr.begin(), texFileNamewstr.end());

		auto dotpos = texFileName.find('.');
		assert(dotpos != std::string::npos);
		std::string fileName = texFileName.substr(0, dotpos);
		//write texture's name data
		uint32_t strCharcount = static_cast<uint32_t>(fileName.length());
		outFile.write(reinterpret_cast<char*>(&strCharcount), sizeof(strCharcount));
		outFile.write(fileName.c_str(), strCharcount);
	}
}
void PS5ModelExporter::Export(CompoundModel& compoundModel, std::string outfilename)
{
	const vector<Model*>& subModels = compoundModel.GetModels();
	
	std::ofstream outFile = std::ofstream(outfilename, std::ios::binary | std::ios::ate);
	//write num models
	uint8_t numModels = static_cast<uint8_t>(subModels.size());
	outFile.write(reinterpret_cast<char*>(&numModels), sizeof(numModels));
	for(Model* subModel:subModels)
	{
		WriteModelExportData(*subModel, outFile,true);
	}
	outFile.close();
}
void PS5ModelExporter::GetVertexData(Model& model, std::vector<float>& outVertexData)
{
	const std::vector<VertexBase*> modelverticies = model.GetVerticies();
	for(VertexBase* v : modelverticies)
	{
		switch(v->m_vertversion)
		{
		case VERTEXVERSION3:
		{
			VertexV3* v3 = static_cast<VertexV3*>(v);
			outVertexData.push_back(v3->m_position.x);outVertexData.push_back(v3->m_position.y);outVertexData.push_back(v3->m_position.z);
			outVertexData.push_back(v3->m_normal.x); outVertexData.push_back(v3->m_normal.y); outVertexData.push_back(v3->m_normal.z);
			outVertexData.push_back(v3->m_uv.x); outVertexData.push_back(v3->m_uv.y);
			break;

		}

		}
	}
}