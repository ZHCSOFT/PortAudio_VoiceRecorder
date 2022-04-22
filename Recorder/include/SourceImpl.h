#pragma once
#include "portaudio.h"
#include <memory>
#include <mutex>
#include <vector>

class AudioDevice;

#ifndef __USAUDIOFORAMT_H__
#define __USAUDIOFORAMT_H__
enum class AudioDataFormat
{
    Unknown,
    Int16,
    Int32,
    Float
};
#endif

struct AudioReadFormat
{
    double inputSampleRate;
    int volume;
    int channelCount;
    PaSampleFormat sampleFormat;
};

class SourceImpl
{
public:
    static std::shared_ptr<SourceImpl> Create(const std::shared_ptr<AudioDevice> & devices);

    explicit SourceImpl(const std::shared_ptr<AudioDevice> & devices);

    SourceImpl();
    ~SourceImpl();
    
    void SetDevice(int deviceID);
    void SetSampleFormat(AudioDataFormat readFormat);
    void SetChannelCount(int count);
    void SetSampleRate(double rate);

    int GetDevice();
    AudioDataFormat GetSampleFormat();
    PaSampleFormat GetPaSampleFormat();
    int GetChannelCount();
    double GetSampleRate();

    void Suspend();

    void Read(std::vector<float> * buffer);
    void Read(std::vector<char> * buffer);
    void Listen();

private:
    void Changed();

private:
    std::shared_ptr<AudioDevice> m_Devices;
    PaStreamParameters m_audioReadParam;
    AudioReadFormat m_audioReadFormat;
    int inputDeviceID = -1;

    PaStream * m_audioReadStream;
    int bytesPerSample;
    char * ringbuffer_;
};
