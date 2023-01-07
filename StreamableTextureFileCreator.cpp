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
	if (imgdata.m_imagemetadata.dimension != DirectX::TEX_DIMENSION_TEXTURE2D)
	{
		//stf only supported for 2d texture
		return 0;
	}
	ofstream stfoutput(outfilename, ios::out | ios::binary|ios::trunc);
	if (!stfoutput.is_open())
	{
		return 0;
	}
	//first write metadatablock.
	char* data = reinterpret_cast<char*>(&(imgdata.m_imagemetadata));
	size_t datasize = sizeof(imgdata.m_imagemetadata);
	stfoutput.write(data,datasize);
	//write img data
	for (size_t i = 0; i < imgdata.m_image.GetImageCount(); i++)
	{
		
	}
	stfoutput.close();
	return 1;
}