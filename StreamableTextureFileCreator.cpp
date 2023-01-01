#include"StreamableTextureFileCreator.h"
#include"DXTexManager.h"
#include<fstream>
using namespace std;

StreamableTextureFileCreator::StreamableTextureFileCreator()
{
}

StreamableTextureFileCreator::~StreamableTextureFileCreator()
{
}
int StreamableTextureFileCreator::ExportToSTF(const wchar_t* imagefile,const char* outfilename)
{
	DXImageData imgdata;

	DXTexManager::LoadTexture(imagefile, imgdata);
	ofstream stfoutput(outfilename, ios::out | ios::binary|ios::trunc);
	if (!stfoutput.is_open())
	{
		return 0;
	}
	char* data = reinterpret_cast<char*>(&(imgdata.m_imagemetadata));
	size_t datasize = sizeof(imgdata.m_imagemetadata);
	stfoutput.write(data,datasize);
	stfoutput.close();
	return 1;
}