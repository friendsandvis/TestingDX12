#include"StreamableTextureFileReader.h"
StreamableTextureFileReader::StreamableTextureFileReader()
{
}

StreamableTextureFileReader::~StreamableTextureFileReader()
{
	m_stfinput.close();
}
int StreamableTextureFileReader::Init(const char* stffilepath)
{
	m_stfinput.open(stffilepath, ios::in | ios::binary);
	if (!m_stfinput.is_open())
	{
		return 0;
	}
	//read metadatablock first
	char* data = reinterpret_cast<char*>(&m_metadata);
	m_stfinput.read(data, sizeof(m_metadata));
	uint64_t numsubres = 0;
	m_stfinput >> numsubres;
	//read subresdatablocks and prep subresdata structs.
	for (uint64_t i = 0; i < numsubres; i++)
	{
		SubResourceData subresdata = {};
		SubResDataBlock subresdatablock = {};
		char* data = reinterpret_cast<char*>(&subresdatablock);
		m_stfinput.read(data, sizeof(SubResDataBlock));
		subresdata.SetSubResDataBlock(subresdatablock);
		m_subresdata.push_back(subresdata);


	}
	//add file stream locations in the file reader.
	streampos currentstreampos = m_stfinput.tellg();
	for (uint64_t i = 0; i < numsubres; i++)
	{
		m_subresdata[i].SetPixelDataStreamPos(currentstreampos);
		currentstreampos += m_subresdata[i].GetSubResDataBlock().SlicePitch;
	}
	return 1;
}
void StreamableTextureFileReader::PreparePixelDataForSubResource(size_t subresidx)
{
	m_subresdata[subresidx].BuildStorageForPixelData();
	char* pixeldata = reinterpret_cast<char*>(m_subresdata[subresidx].GetPixelData());
	streampos pixeldatastreampos=m_subresdata[subresidx].GetPixelDataStreamPos();
	assert(m_stfinput.is_open());
	m_stfinput.seekg(pixeldatastreampos);
	m_stfinput.read(pixeldata, m_subresdata[subresidx].GetSubResDataBlock().SlicePitch);
}
void StreamableTextureFileReader::PrepSubResPixData()
{
	for (size_t i = 0; i < m_subresdata.size(); i++)
	{
		PreparePixelDataForSubResource(i);
	}
}

StreamableTextureFileReader::SubResourceData::SubResourceData()
	:m_pixeldata(nullptr),
	m_pixeldatastreampos(0)
{
}
StreamableTextureFileReader::SubResourceData::~SubResourceData()
{
	if (m_pixeldata)
	{
		delete[] m_pixeldata;
	}
}
void StreamableTextureFileReader::SubResourceData::BuildStorageForPixelData()
{
	uint64_t pixeldatasize= m_subresdatablock.SlicePitch;
	assert(pixeldatasize != 0);
	m_pixeldata = new uint8_t[pixeldatasize];
}