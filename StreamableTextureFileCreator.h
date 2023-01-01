#pragma once
#include"DXTextureUtils.h"
struct Header
{
	TexMetadata metadata;
};
struct SubResDataBlocks
{
	LONG_PTR rowpitch;
	LONG_PTR SlicePitch;

};
/*
* creates .stf files which atre binary file containing texture data and subresource data to allow them to be streamed(typically by mipstreamer.
*/
class StreamableTextureFileCreator
{
public:
	StreamableTextureFileCreator();
	~StreamableTextureFileCreator();
	/*output-
	* 1:sucess
	* 0:failed
	*/
	
	int ExportToSTF(const const wchar_t* imagefile, const char* outfilename);
private:

};

