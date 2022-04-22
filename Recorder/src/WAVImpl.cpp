#include "WAVImpl.h"


WAVImpl::WAVImpl()
{
    header = nullptr;
    data = nullptr;
}

WAVImpl::~WAVImpl()
{
    header = nullptr;
    data = nullptr;
}

bool WAVImpl::write(const std::string& filename, const waveHeader &header, void *data, uint32_t length)
{
    std::ofstream ofs(filename, std::ofstream::binary);
    if (!ofs)
		return false;
    header.data->cb_size = ((length + 1) / 2) * 2;
	header.riff->cb_size = 4 + 4 + header.fmt->cb_size + 4 + 4 + header.data->cb_size + 4;

    // Write RIFF
    char *chunk = (char*)header.riff.get();
	ofs.write(chunk, sizeof(baseChunk));

    // Write WAVE fourcc
    ofs.write((char*)&(header.wave_fcc), 4);

    // Write fmt
    chunk = (char*)header.fmt.get();
    ofs.write(chunk, sizeof(baseChunk));

    // Write fmt_data
    chunk = (char*)header.fmt_data.get();
    ofs.write(chunk, header.fmt->cb_size);

    // Write data
    chunk = (char*)header.data.get();
    ofs.write(chunk, sizeof(baseChunk));
    ofs.write((char*)data, length);

    ofs.close();
    return true;
}

bool WAVImpl::read(const std::string &filename)
{
    if (!readHeader(filename))
		return false;

    uint32_t offset = header->riff->cb_size - header->data->cb_size + 8;
    data = std::unique_ptr<uint8_t[]>(new uint8_t[header->data->cb_size]);

    std::ifstream ifs(filename, std::ifstream::binary);
    if (!ifs)
		return false;
    
    ifs.seekg(offset);
    ifs.read((char*)(data.get()), header->data->cb_size);
    return true;
}

bool WAVImpl::readHeader(const std::string &filename)
{
    std::ifstream ifs(filename, std::ifstream::binary);
    if (!ifs)
        return false;

    header = std::make_unique<waveHeader>();

    // Read RIFF chunk
    FOURCC fourcc;
    ifs.read((char*)&fourcc, sizeof(FOURCC));

    if (fourcc != makeFOURCC<'R', 'I', 'F', 'F'>::value) // Check RIFF
        return false;
    baseChunk riff_chunk(fourcc);
    ifs.read((char*)&riff_chunk.cb_size, sizeof(uint32_t));

    header->riff = std::make_shared<baseChunk>(riff_chunk);

    // Read WAVE FOURCC
    ifs.read((char*)&fourcc, sizeof(FOURCC));
    if (fourcc != makeFOURCC<'W', 'A', 'V', 'E'>::value)
        return false;
    header->wave_fcc = fourcc;

    // Read format chunk
    ifs.read((char*)&fourcc, sizeof(FOURCC));
    if (fourcc != makeFOURCC<'f', 'm', 't', ' '>::value)
        return false;

    baseChunk fmt_chunk(fourcc);
    ifs.read((char*)&fmt_chunk.cb_size, sizeof(uint32_t));

    header->fmt = std::make_shared<baseChunk>(fmt_chunk);

    // Read format data
    waveFormat format;
    ifs.read((char*)&format, fmt_chunk.cb_size);

    // Read data chunk
    ifs.read((char*)&fourcc, sizeof(fourcc));

    if (fourcc != makeFOURCC<'d', 'a', 't', 'a'>::value)
        return false;

    baseChunk data_chunk(fourcc);
    ifs.read((char*)&data_chunk.cb_size, sizeof(uint32_t));

    header->data = std::make_shared<baseChunk>(data_chunk);

    ifs.close();

    return true;
}
