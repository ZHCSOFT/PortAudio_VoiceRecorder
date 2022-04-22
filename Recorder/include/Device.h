#pragma once

class AudioDevice
{
public:
    AudioDevice();
    ~AudioDevice();
    int GetInputDevice();
private:
    int m_defaultInputDeviceID;
};
