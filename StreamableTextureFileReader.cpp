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
		char* data = reinterpret_cast<char*>(&subresdata.subresdatablock);
		m_stfinput.read(data, sizeof(SubResDataBlock));
		m_subresdata.push_back(subresdata);


	}
	//add file stream locations in the file reader.
	streampos currentstreampos = m_stfinput.tellg();
	for (uint64_t i = 0; i < numsubres; i++)
	{
		m_subresdata[i].pixeldatastreampos = currentstreampos;
		currentstreampos += m_subresdata[i].subresdatablock.SlicePitch;
	}
	return 1;
}