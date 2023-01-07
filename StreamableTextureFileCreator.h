#pragma once
#include"DXTextureUtils.h"
#include"StreamableTextureFileCommon.h"


/*
* creates .stf files which are binary file containing texture data and subresource data to allow them to be streamed(typically by mipstreamer.
* NOTE: stf onluy supported for 2d tex now for simplification as following simplified rules are followed:
* 1.  subbresources for a 2d tex is its mips.
* 2.  subresource's pixel data size is exactly equal to it's slicepitch for 2d tex
*/
/*
* STF data format:
* - first metadata block
* - then number of subresources(UINT64)
* -then numsubresource "SubResDataBlock"
* -then actual subresource pixel data for all subresources
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
	
	int ExportToSTF(const wchar_t* imagefile, const char* outfilename);
private:

};

