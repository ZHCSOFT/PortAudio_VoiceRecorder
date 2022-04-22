#pragma once
#include <memory>
#include <fstream>


#define FOURCC uint32_t
#define MAKE_FOURCC(a, b, c, d) \
                   ( ((uint32_t)d) | ( ((uint32_t)c) << 8) | ( ((uint32_t)b) << 16) | ( ((uint32_t)a) << 24) )
                   template <char ch0, char ch1, char ch2, char ch3> \
                            struct makeFOURCC{enum { value = (ch0 << 0) + (ch1 << 8) + (ch2 << 16) + (ch3 << 24)}; };

struct baseChunk
{
    FOURCC fcc;
    uint32_t cb_size;

    baseChunk(FOURCC fourcc): fcc(fourcc)
    {
        cb_size = 0;
    }
};

struct waveFormat
{
    uint16_t format_tag;
    uint16_t channels;
    uint32_t sample_per_sec;
    uint32_t bytes_per_sec;
    uint16_t block_aligin;
    uint16_t bits_per_sample;
    uint16_t ex_size;

    waveFormat()
    {
        format_tag = 1;
        ex_size = 0;

        channels = 0;
        sample_per_sec = 0;
        bytes_per_sec = 0;
        block_aligin = 0;
        bits_per_sample = 0;
    }

    waveFormat(uint16_t nb_channel, uint32_t sample_rate, uint16_t sample_bits): channels(nb_channel),
                                                                                 sample_per_sec(sample_rate),
                                                                                 bits_per_sample(sample_bits)
    {
        format_tag = 0x01;
        bytes_per_sec = channels * sample_per_sec * bits_per_sample / 8;
        block_aligin = channels * bits_per_sample / 8;
        ex_size = 0;
    }
};

struct waveHeader
{
    std::shared_ptr<baseChunk> riff;
    FOURCC wave_fcc;
    std::shared_ptr<baseChunk> fmt;
    std::shared_ptr<waveFormat> fmt_data;
    std::shared_ptr<baseChunk> data;

    waveHeader()
    {
        riff = nullptr;
        fmt = nullptr;

        fmt_data = nullptr;
        data = nullptr;

        wave_fcc = 0;
    }

    waveHeader(uint16_t nb_channel, uint32_t sample_rate, uint16_t sample_bits)
    {
        riff = std::make_shared<baseChunk>(makeFOURCC<'R', 'I', 'F', 'F'>::value);
        fmt = std::make_shared<baseChunk>(makeFOURCC<'f', 'm', 't', ' '>::value);
        fmt -> cb_size = 18;

        fmt_data = std::make_shared<waveFormat>(nb_channel, sample_rate, sample_bits);
        data = std::make_shared<baseChunk>(makeFOURCC<'d', 'a', 't', 'a'>::value);

        wave_fcc = makeFOURCC<'W', 'A', 'V', 'E'>::value;
    }
};


class WAVImpl
{

public:
    WAVImpl();
    ~WAVImpl();
    bool write(const std::string& filename, const waveHeader &header, void *data, uint32_t length);
    bool read(const std::string &filename);

private:
    bool readHeader(const std::string &filename);

    std::shared_ptr<waveHeader> header;
    std::unique_ptr<uint8_t[]> data;

};
