#include "Device.h"
#include <portaudio.h>
#include <QMediaDevices>
#include <iostream>

AudioDevice::AudioDevice(): m_defaultInputDeviceID(paNoDevice)
{
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("[ ERROR ] Failed initialize PortAudio, Error is %s\n", Pa_GetErrorText(err));
    }
    else
    {
        m_defaultInputDeviceID = Pa_GetDefaultInputDevice();
    }
}

AudioDevice::~AudioDevice()
{

}

int AudioDevice::GetInputDevice()
{
    return m_defaultInputDeviceID;
}
