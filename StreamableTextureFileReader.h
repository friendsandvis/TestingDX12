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
	inline TexMetadata GetMetaData() { return m_metadata; }

private:
	//containnsd minimal data needed to represent a subres and where to stream its data from
	class SubResourceData
	{
	public:
		SubResourceData();
		~SubResourceData();
		SubResDataBlock GetSubResDataBlock() { return m_subresdatablock; }
		void SetSubResDataBlock(SubResDataBlock datablock) {  m_subresdatablock= datablock; }
		streampos GetPixelDataStreamPos() { return m_pixeldatastreampos; }
		void SetPixelDataStreamPos(streampos pos) { m_pixeldatastreampos=pos; }
		uint8_t* GetPixelData() { return m_pixeldata; }
		void BuildStorageForPixelData();
	private:

		SubResDataBlock m_subresdatablock;
		streampos m_pixeldatastreampos;
		uint8_t* m_pixeldata;
		
	};
private:
	TexMetadata m_metadata;
	vector<SubResourceData> m_subresdata;
	ifstream m_stfinput;
	void PreparePixelDataForSubResource(size_t subresidx);
public:
	void PrepSubResPixData();
	void GetSubResourceData(size_t subresidx, D3D12_SUBRESOURCE_DATA& outsubresdata);
};

