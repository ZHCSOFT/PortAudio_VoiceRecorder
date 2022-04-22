#pragma once
#include "export_api.h"
#include "SinkImpl.h"
#include "SourceImpl.h"
#include "Device.h"


class DLLDECL Recorder
{
public:
    Recorder();
    ~Recorder(){};

    void SetReadDevice(int deviceID);
    void SetReadSampleFormat(AudioDataFormat format);
    void SetReadChannelCount(int count);
    void SetReadSampleRate(double rate);

    int GetReadDevice();
    AudioDataFormat GetReadSampleFormat();
    PaSampleFormat GetReadPaSampleFormat();
    int GetReadChannelCount();
    double GetReadSampleRate();

    void Listen();
    void PauseListen(bool bPause);
    void Read(std::vector<float> * buffer);
    void Read(std::vector<char> * buffer);
private:
    std::shared_ptr<SinkImpl> m_sinkImpl;
    std::shared_ptr<SourceImpl> m_sourceImpl;
    std::shared_ptr<AudioDevice> m_Devices;
};
