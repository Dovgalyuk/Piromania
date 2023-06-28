///////////////////////////////////////////////////////////
// CWAVE.H: Header file for the WAVE class.
///////////////////////////////////////////////////////////

#ifndef CWAVE_H
#define CWAVE_H

#include <windows.h>
#include <mmsystem.h>



class CWave
{
public:
    DWORD m_waveSize;
    BOOL m_waveOK;
    char* m_pWave;
    WAVEFORMATEX m_waveFormatEx;

    CWave();
    explicit CWave(char* fileName);
    ~CWave();

    DWORD GetWaveSize();
    LPWAVEFORMATEX GetWaveFormatPtr();
    char* GetWaveDataPtr();
    BOOL WaveOK();

    BOOL LoadWaveFile(HMMIO hMMIO);
    BOOL LoadWaveFile(char* fileName);
};

#endif

