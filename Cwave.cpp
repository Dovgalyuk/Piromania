///////////////////////////////////////////////////////////
// CWAVE.CPP: Implementation file for the WAVE class.
///////////////////////////////////////////////////////////

#include "cwave.h"

#pragma comment(lib, "winmm.lib")


///////////////////////////////////////////////////////////
// CWave::CWave()
///////////////////////////////////////////////////////////
CWave::CWave()
{
    // Initialize the class's members.
    m_waveSize = 0;
    m_waveOK = FALSE;
    m_pWave = NULL;
}

///////////////////////////////////////////////////////////
// CWave::CWave()
///////////////////////////////////////////////////////////
CWave::CWave(char* fileName)
{
    // Initialize the class's members.
    m_waveSize = 0;
    m_waveOK = FALSE;
    m_pWave = NULL;

    // Load the wave file.
    m_waveOK = LoadWaveFile(fileName);
}

///////////////////////////////////////////////////////////
// CWave::~CWave()
///////////////////////////////////////////////////////////
CWave::~CWave()
{
    // Free the memory assigned to the wave data.
    delete[] m_pWave;
}

///////////////////////////////////////////////////////////
// CWave::LoadWaveFile()
//
// This function loads a wave from disk into memory. It
// also initializes various class data members.
///////////////////////////////////////////////////////////
BOOL CWave::LoadWaveFile(char* fileName)
{
    HMMIO hMMIO;

    // Open the wave file.
    hMMIO = mmioOpen(fileName, NULL, MMIO_READ | MMIO_ALLOCBUF);

    LoadWaveFile(hMMIO);

    mmioClose(hMMIO, 0);

    return TRUE;
}

///////////////////////////////////////////////////////////
// CWave::LoadWaveFile()
//
// This function loads a wave from disk into memory. It
// also initializes various class data members.
///////////////////////////////////////////////////////////
BOOL CWave::LoadWaveFile(HMMIO hMMIO)
{
    MMCKINFO mmCkInfoRIFF;
    MMCKINFO mmCkInfoChunk;

    // Descend into the RIFF chunk.
    mmCkInfoRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    mmioDescend(hMMIO, &mmCkInfoRIFF, NULL, MMIO_FINDRIFF);

    // Descend into the format chunk.
    mmCkInfoChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    mmioDescend(hMMIO, &mmCkInfoChunk, &mmCkInfoRIFF, MMIO_FINDCHUNK);

    // Read the format information into the WAVEFORMATEX structure.
    mmioRead(hMMIO, (char*)&m_waveFormatEx, sizeof(WAVEFORMATEX));

    // Ascend out of the format chunk.
    mmioAscend(hMMIO, &mmCkInfoChunk, 0);

    // Descend into the data chunk.
    mmCkInfoChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    mmioDescend(hMMIO, &mmCkInfoChunk, &mmCkInfoRIFF, MMIO_FINDCHUNK);

    // Save the size of the wave data.
    m_waveSize = mmCkInfoChunk.cksize;// + mmCkInfoChunk.cksize % 2;

    // Allocate a buffer for the wave data.
    if (m_pWave)
      delete[] m_pWave;

    m_pWave = new char[m_waveSize];

    // Read the wave data into the buffer.
    mmioRead(hMMIO, (char*)m_pWave, m_waveSize);

    return TRUE;
}

///////////////////////////////////////////////////////////
// CWave::GetWaveSize()
//
// This returns the size in bytes of the wave data.
///////////////////////////////////////////////////////////
DWORD CWave::GetWaveSize()
{
    return m_waveSize;
}

///////////////////////////////////////////////////////////
// CWave::GetWaveFormatPtr()
//
// This function returns a pointer to the wave file's
// WAVEFORMATEX structure.
///////////////////////////////////////////////////////////
LPWAVEFORMATEX CWave::GetWaveFormatPtr()
{
    return &m_waveFormatEx;
}

///////////////////////////////////////////////////////////
// CWave::GetWaveDataPtr()
//
// This function returns a pointer to the wave's
// actual sound data.
///////////////////////////////////////////////////////////
char* CWave::GetWaveDataPtr()
{
    return m_pWave;
}

///////////////////////////////////////////////////////////
// CWave::WaveOK()
//
// This function returns a Boolean value indicating whether
// the wave file object was set up successfully.
///////////////////////////////////////////////////////////
BOOL CWave::WaveOK()
{
    return m_waveOK;
}

