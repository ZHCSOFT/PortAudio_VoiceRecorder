#include <iostream>
#include <thread>
#include <fstream>
#include <deque>
#include "Recorder.h"
#include "WAVImpl.h"


#define NUM_WRITES_PER_BUFFER   (4)
#define NUM_CHANNELS            (1)

typedef char SAMPLE;

int threadFuncReader(Recorder * RecorderEntity, std::vector<char> * m_Buffer)
{
    std::vector<char> m_readBuffer;
    WAVImpl wavSaver;
    while (true)
    {
        RecorderEntity->Read(&m_readBuffer);
        m_Buffer->insert(m_Buffer->end(), m_readBuffer.begin(), m_readBuffer.end());
        printf("[ TEST ] GET: m_readBuffer.size = %d\n", m_readBuffer.size());
        printf("[ TEST ] GET: m_Buffer.size = %d\n", m_Buffer->size());
        /* Sleep a little while... */
        Pa_Sleep(100);
    }

    return 0;
}



int main(int argc, char* argv[])
{
    std::string savePath = "./recorded.wav";

    Recorder RecorderEntity;
    WAVImpl wavSaver;
    std::vector<char> m_Buffer;

    waveHeader wavInfo = waveHeader(1, RecorderEntity.GetReadSampleRate(), 16);
    printf("[ TEST ] RecorderEntity.GetReadSampleRate = %f\n", RecorderEntity.GetReadSampleRate());

    RecorderEntity.Listen();
    printf("[ TEST ] RecorderEntity.Listen() Fin \n");
    // USAudioEntity.SetReadDevice(1);
    RecorderEntity.PauseListen(true);
    printf("[ TEST ] RecorderEntity.PauseListen(true) Fin \n");
    RecorderEntity.PauseListen(false);
    printf("[ TEST ] RecorderEntity.PauseListen(false) Fin \n");

    std::thread m_readSaveThread(threadFuncReader, &RecorderEntity, &m_Buffer);
    m_readSaveThread.detach();

    Pa_Sleep(60000);
    printf("[ TEST ] Fin: m_Buffer.size() = %d\n", m_Buffer.size());
    wavSaver.write(savePath, wavInfo, m_Buffer.data(), m_Buffer.size());
    return 0;
}
