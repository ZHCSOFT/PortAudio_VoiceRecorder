#include "Recorder.h"
#include <iostream>


Recorder::Recorder(): m_Devices{std::make_shared<AudioDevice>()}
{
    m_sourceImpl = SourceImpl::Create(m_Devices);
}

// m_sourceImpl

void Recorder::SetReadDevice(int deviceID)
{
    if(m_sourceImpl)
        m_sourceImpl->SetDevice(deviceID);
}

void Recorder::SetReadSampleFormat(AudioDataFormat format)
{
    if(m_sourceImpl)
        m_sourceImpl->SetSampleFormat(format);
}

void Recorder::SetReadChannelCount(int count)
{
    if(m_sourceImpl)
        m_sourceImpl->SetChannelCount(count);
}

void Recorder::SetReadSampleRate(double rate)
{   
    if(m_sourceImpl)
        m_sourceImpl->SetSampleRate(rate);
}

int Recorder::GetReadDevice()
{
    if(m_sourceImpl)
        return m_sourceImpl->GetDevice();
    return -1;
}

AudioDataFormat Recorder::GetReadSampleFormat()
{
    if(m_sourceImpl)
        return m_sourceImpl->GetSampleFormat();
    return AudioDataFormat::Unknown;
}

PaSampleFormat Recorder::GetReadPaSampleFormat()
{
    if(m_sourceImpl)
        return m_sourceImpl->GetPaSampleFormat();
    return paNonInterleaved;
}

int Recorder::GetReadChannelCount()
{
    if(m_sourceImpl)
        return m_sourceImpl->GetChannelCount();
    return -1;
}

double Recorder::GetReadSampleRate()
{
    if(m_sourceImpl)
        return m_sourceImpl->GetSampleRate();
    return -1.;
}

void Recorder::Listen()
{
    if(m_sourceImpl)
        m_sourceImpl->Listen();
}

void Recorder::PauseListen(bool bPause)
{
    if(m_sourceImpl){
        bPause ? m_sourceImpl->Suspend() : m_sourceImpl->Listen();
    }
}

void Recorder::Read(std::vector<float> * buffer)
{
    if(m_sourceImpl){
        m_sourceImpl->Read(buffer);
    }
}

void Recorder::Read(std::vector<char> * buffer)
{
    if(m_sourceImpl){
        m_sourceImpl->Read(buffer);
    }
}
