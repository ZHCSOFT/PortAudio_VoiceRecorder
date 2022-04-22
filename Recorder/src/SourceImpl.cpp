#include "SourceImpl.h"
#include "Device.h"

#include "pa_ringbuffer.h"
#include "pa_util.h"
#include <iostream>


bool SourceContinue = false;

int ringBufferSize = 65536;

PaUtilRingBuffer pa_ringbuffer_;
static int num_lost_samples_ = 0;
static int paOutStreamBk(const void* input, void* output, unsigned long frameCount,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags, void* userData)
{

    ring_buffer_size_t num_written_samples = PaUtil_WriteRingBuffer(&pa_ringbuffer_, input, frameCount);
    num_lost_samples_ += frameCount - num_written_samples;

    if (SourceContinue)
        return paContinue;
    else
    {
        printf("[ INFO ] paOutStreamBk exit\n");
        return paComplete;
    }
}

std::shared_ptr<SourceImpl> SourceImpl::Create(const std::shared_ptr<AudioDevice> &m_Devices)
{
    return std::make_shared<SourceImpl>(m_Devices);
}

SourceImpl::SourceImpl(const std::shared_ptr<AudioDevice> &devices): m_Devices(devices)
{
    if (m_Devices)
    {
        inputDeviceID = m_Devices->GetInputDevice();
        if (inputDeviceID == paNoDevice)
        {
            printf("[ ERROR ] Failed get input device, inputDeviceID = %d\n", inputDeviceID);
        }
        else
        {
            m_audioReadFormat.channelCount = 1;
            SetSampleFormat(USAudioFormat::Int16);
            SetSampleRate(Pa_GetDeviceInfo(inputDeviceID)->defaultSampleRate);
            Changed();
        }
        
    }
    else
    {
        printf("[ ERROR ] Invalid m_Devices on SourceImpl constructor\n");
    }
}

SourceImpl::~SourceImpl()
{
    if (m_audioReadStream)
    {
        Suspend();
        PaError err = Pa_Terminate();
        if (err != paNoError)
        {
            printf("[ ERROR ] Failed terminate PortAudio, Error is %s\n", Pa_GetErrorText(err));
        }
    }
    PaUtil_FreeMemory(ringbuffer_);
}

void SourceImpl::Changed()
{
    if (m_audioReadParam.device != inputDeviceID)
    {
        std::cout << "[ INFO ] Audio Read Device ID changed " << m_audioReadParam.device << " -> " << inputDeviceID << std::endl;
        std::cout << "[ INFO ] Audio Read device name : " << Pa_GetDeviceInfo(inputDeviceID)->name << std::endl;

        if (m_audioReadFormat.inputSampleRate != Pa_GetDeviceInfo(inputDeviceID)->defaultSampleRate)
        {
            std::cout << "[ ATTEN ] Audio Read device default sample rate mismatch" << std::endl;
            std::cout << "[ ATTEN ] Suggested: " << Pa_GetDeviceInfo(inputDeviceID)->defaultSampleRate << std::endl;
            std::cout << "[ ATTEN ] Current: " << m_audioReadFormat.inputSampleRate << std::endl;
        }
    }
    m_audioReadParam.device = inputDeviceID; 

    if (m_audioReadParam.channelCount != m_audioReadFormat.channelCount)
    {
        std::cout << "[ INFO ] Audio Read Sample ChannelCount changed " << m_audioReadParam.channelCount << " -> " << m_audioReadFormat.channelCount << std::endl;
    }
    m_audioReadParam.channelCount = m_audioReadFormat.channelCount;

    m_audioReadParam.suggestedLatency = NULL;

    if (SourceContinue)
    {
        Listen();
    }
}

void SourceImpl::SetDevice(int deviceID)
{
    if (Pa_GetDeviceInfo(inputDeviceID))
    {
        inputDeviceID = deviceID;
    }
    else
    {
        printf("[ REJECTED ] Invalid Audio Read Device ID change provided, ID = %d\n", deviceID);
    }
    
    Changed();
}

void SourceImpl::SetSampleRate(double rate)
{
    if (m_audioReadFormat.inputSampleRate != rate)
    {
        std::cout << "[ INFO ] Audio Read SampleRate changed " << m_audioReadFormat.inputSampleRate << " -> " << rate << std::endl;
    }
    m_audioReadFormat.inputSampleRate = rate;

    if (SourceContinue)
    {
        Listen();
    }
}

void SourceImpl::SetChannelCount(int count)
{
    m_audioReadFormat.channelCount = count;
    Changed();
}

void SourceImpl::SetSampleFormat(AudioDataFormat readFormat)
{
    switch (readFormat)
    {
    case AudioDataFormat::Int16:
        m_audioReadFormat.sampleFormat = paInt16;
        bytesPerSample = 2;
        break;
    case AudioDataFormat::Int32:
        m_audioReadFormat.sampleFormat = paInt32;
        bytesPerSample = 4;
        break;
    case AudioDataFormat::Float:
        m_audioReadFormat.sampleFormat = paFloat32;
        bytesPerSample = 4;
        break;
    default:
        std::__throw_runtime_error("[ FATAL ] AudioDataFormat Unknown format!");
        break;
    }

    if (m_audioReadParam.sampleFormat != m_audioReadFormat.sampleFormat)
    {
        std::cout << "[ INFO ] Audio Read Sample Format changed " << m_audioReadParam.sampleFormat << " -> " << m_audioReadFormat.sampleFormat << std::endl; 
    }
    m_audioReadParam.sampleFormat = m_audioReadFormat.sampleFormat;   

    if (SourceContinue)
    {
        Listen();
    }
}

int SourceImpl::GetDevice()
{
    return inputDeviceID;
}

AudioDataFormat SourceImpl::GetSampleFormat()
{
    AudioDataFormat result;
    switch (m_audioReadParam.sampleFormat)
    {
    case paInt16:
        result = AudioDataFormat::Int16;
        break;
    case paInt32:
        result = AudioDataFormat::Int32;
        break;
    case paFloat32:
        result = AudioDataFormat::Float;
        break;
    default:
        std::__throw_runtime_error("[ FATAL ] m_audioReadParam.sampleFormat Unknown format!");
        break;
    }
    return result;
}

PaSampleFormat SourceImpl::GetPaSampleFormat()
{
    return m_audioReadParam.sampleFormat;
}

int SourceImpl::GetChannelCount()
{
    return m_audioReadParam.channelCount;
}

double SourceImpl::GetSampleRate()
{
    return m_audioReadFormat.inputSampleRate;
}

void SourceImpl::Suspend()
{
    SourceContinue = false;
    if (num_lost_samples_ != 0)
    {
        printf("[ ATTEN ] Lost %d samples due to ring buffer overflow\n");
        num_lost_samples_ = 0;
    }

    if (m_audioReadStream)
    {
        PaError err = Pa_StopStream(m_audioReadStream);
        if (err != paNoError)
        {
            printf("[ ERROR ] Failed Stop Input Stream, Error is %s\n", Pa_GetErrorText(err));
        }
        else
        {
            printf("[ INFO ] Pa_StopStream done\n");
        }
        printf("[ INFO ] PortAudio record stream suspended\n");
    }
    else
    {
        printf("[ FATAL ] Invalid m_audioReadStream on Suspend()\n");
        std::__throw_runtime_error("[ FATAL ] Invalid m_audioReadStream on Suspend()");
    }
}

void SourceImpl::Listen()
{
    ringbuffer_ = static_cast<char*>(PaUtil_AllocateMemory(bytesPerSample * ringBufferSize));

    if (ringbuffer_ == NULL)
    {
        printf("[ FATAL ] Fail to allocate memory for ring buffer\n");
        std::__throw_runtime_error("[ FATAL ] Fail to allocate memory for ring buffer");
    }

    ring_buffer_size_t rb_init_ans = PaUtil_InitializeRingBuffer(&pa_ringbuffer_,
                                                                 bytesPerSample,
                                                                 ringBufferSize,
                                                                 ringbuffer_);

    if (rb_init_ans == -1)
    {
        printf("[ FATAL ] Ring buffer size not power of 2\n");
        std::__throw_runtime_error("[ FATAL ] Ring buffer size not power of 2");
    }

    // PaError err = Pa_OpenStream(&m_audioReadStream,
    //                             &m_audioReadParam, NULL,
    //                             m_audioReadFormat.inputSampleRate,
    //                             512, paFramesPerBufferUnspecified,
    //                             paOutStreamBk, NULL);

    PaError err = Pa_OpenDefaultStream(&m_audioReadStream,
                                       m_audioReadParam.channelCount,
                                       0,
                                       m_audioReadParam.sampleFormat,
                                       m_audioReadFormat.inputSampleRate,
                                       paFramesPerBufferUnspecified,
                                       paOutStreamBk, NULL);

    if (err != paNoError)
    {
        printf("[ FATAL ] Pa_OpenStream error occured, Error is %s\n", Pa_GetErrorText(err));
        std::__throw_runtime_error("[ FATAL ] Pa_OpenStream error occured");
    }
    else
    {
        printf("[ INFO ] Pa_OpenStream done\n");
    }

    err = Pa_StartStream(m_audioReadStream);
    if (err != paNoError)
    {
        printf("[ FATAL ] Pa_StartStream error occured, Error is %s\n", Pa_GetErrorText(err));
        std::__throw_runtime_error("[ FATAL ] Pa_StartStream error occured");
    }
    else
    {
        printf("[ INFO ] Pa_StartStream done\n");
    }

    SourceContinue = true;
}

void SourceImpl::Read(std::vector<float> * buffer)
{

    int numAvailableSamples = PaUtil_GetRingBufferReadAvailable(&pa_ringbuffer_);
    buffer->resize(numAvailableSamples);
    int numReadSamples = PaUtil_ReadRingBuffer(&pa_ringbuffer_, buffer->data(), numAvailableSamples);

    if (numAvailableSamples != numReadSamples)
    {
        printf("[ ATTEN ] Mismatch read size, numAvailableSamples = %d\n", numAvailableSamples);
        printf("|--------------------------------- numReadSamples = %d\n", numReadSamples);
    }
}

void SourceImpl::Read(std::vector<char> * buffer)
{
    int numAvailableSamples = PaUtil_GetRingBufferReadAvailable(&pa_ringbuffer_);
    buffer->resize(numAvailableSamples * bytesPerSample);
    int numReadSamples = PaUtil_ReadRingBuffer(&pa_ringbuffer_, buffer->data(), numAvailableSamples);

    if (numAvailableSamples != numReadSamples)
    {
        printf("[ ATTEN ] Mismatch read size, numAvailableSamples = %d\n", numAvailableSamples);
        printf("|--------------------------------- numReadSamples = %d\n", numReadSamples);
    }
}
