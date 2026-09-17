#pragma once
#include <string>
#include <vector>
#include<fstream>
#include <iostream>
class Model;
class CompoundModel;
class PS5ModelExporter
{
public:
	void Export(Model& model, std::string outfilename);
	
	void Export(CompoundModel& compoundModel, std::string outfilename);
private:
	void GetVertexData(Model& model, std::vector<float>& outVertexData);
	void WriteModelExportData(Model& model, std::ofstream& outFile, bool writeDiffuseTexName = false);
};