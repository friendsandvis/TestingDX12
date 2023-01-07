#include"StreamableTextureFileCommon.h"

SubResDataBlock GetSubResDataBlockFromDXImage(const Image& img)
{
	SubResDataBlock datablock = {};
	datablock.rowpitch = static_cast<uint64_t>(img.rowPitch);
	datablock.SlicePitch = static_cast<uint64_t>(img.slicePitch);
	return datablock;
}