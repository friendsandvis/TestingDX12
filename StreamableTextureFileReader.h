#pragma once
#include"StreamableTextureFileCommon.h"
#include"DXTextureUtils.h"
#include<vector>
#include<fstream>
using namespace std;

class StreamableTextureFileReader
{
public:
	StreamableTextureFileReader();
	~StreamableTextureFileReader();
	int Init(const char* stffilepath);
private:
	//containnsd minimal data needed to represent a subres and where to stream its data from
	struct SubResourceData
	{
		SubResDataBlock subresdatablock;
		streampos pixeldatastreampos;
	};
	
	TexMetadata m_metadata;
	vector<SubResourceData> m_subresdata;
	ifstream m_stfinput;
};

