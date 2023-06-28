#include "Sounds.h"
#include "Piromania.h"

#pragma comment(lib, "dsound.lib")



// direct sound object
LPDIRECTSOUND pDirectSoundObj = NULL;
// следующий буфер
int currentBufferNum = 0;
// указатели на буферы
const int MAXBUFFERSCOUNT = 20;
LPDIRECTSOUNDBUFFER bufferPointers[MAXBUFFERSCOUNT];

// буферы-дубликаты
const int MAXBUFFERS = 10;
LPDIRECTSOUNDBUFFER soundBuf[MAXBUFFERS];

// timer to delete sounds
UINT timerID;


// идентификаторы звуков
int SND_FIRE;
int SND_DYNAMITE;
int SND_ENEMY;
int SND_ANGEL;
int SND_ELEPHANT;
int SND_DROPLADDER;
int SND_DROPBUCKET;
int SND_DROPEXTINGUISHER;
int SND_DROP;
int SND_PICKUPLADDER;
int SND_PICKUP;
int SND_MANDIE[2];
int SND_EXTINGUISHER;
int SND_WIN;
int SND_BUCKET;
int SND_STEAM;
int SND_ENEMYDIE;
int SND_WATER;
int SND_DOOR;



int CreateSoundBuffer(CWave* pWave)
{
    DSBUFFERDESC dsBufferDesc;

    // Make sure there's room for another buffer.
    if (currentBufferNum >= MAXBUFFERSCOUNT)
      return -1;

    // Initialize the DSBUFFERDESC structure.
    LPWAVEFORMATEX pWaveFormat = pWave->GetWaveFormatPtr();
    memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
    dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
    dsBufferDesc.dwFlags = /*DSBCAPS_CTRLPAN | */DSBCAPS_CTRLVOLUME /*| DSBCAPS_CTRLFREQUENCY*/;
    dsBufferDesc.dwBufferBytes = pWave->GetWaveSize();
    dsBufferDesc.lpwfxFormat = (LPWAVEFORMATEX) pWaveFormat;

    // Create the secondary sound buffer.
    if (pDirectSoundObj->CreateSoundBuffer(&dsBufferDesc,
        &bufferPointers[currentBufferNum], NULL) != DS_OK)
      return 0;

    // Save the buffer size.
    int bufferSize = dsBufferDesc.dwBufferBytes;

    LPVOID pSoundBlock1;
    LPVOID pSoundBlock2;
    DWORD bytesSoundBlock1;
    DWORD bytesSoundBlock2;

    // Get a pointer to the requested buffer.
    LPDIRECTSOUNDBUFFER pSoundBuffer = bufferPointers[currentBufferNum];

    // Lock the buffer.
    if (pSoundBuffer->Lock(0, bufferSize,
        &pSoundBlock1, &bytesSoundBlock1,
        &pSoundBlock2, &bytesSoundBlock2, 0) != DS_OK)
      return -1;

    // Copy the data into the buffer.
    char* pWaveData = pWave->GetWaveDataPtr();
    DWORD waveSize = pWave->GetWaveSize();
    memcpy((void*)pSoundBlock1, pWaveData, waveSize);

    // Unlock the buffer.
    pSoundBuffer->Unlock(pSoundBlock1, bytesSoundBlock1,
            pSoundBlock2, bytesSoundBlock2);

    return currentBufferNum++;
}


void TickSound()
{
    // kill sounds
    DWORD status;
    for (int i = 0 ; i < MAXBUFFERS ; i++)
        if (soundBuf[i])
        {
            soundBuf[i]->GetStatus(&status);
            if (!(status & DSBSTATUS_PLAYING))
            {
                //delete soundBuf[i];
                soundBuf[i]->Release();
                soundBuf[i] = NULL;
            }
        }
}


// остановить все звуки
void StopSounds()
{
    // kill sounds
    for (int i = 0 ; i < MAXBUFFERS ; i++)
        if (soundBuf[i])
        {
            soundBuf[i]->Stop();
            soundBuf[i]->Release();
            soundBuf[i] = NULL;
        }
}



BOOL InitSounds()
{
    currentBufferNum = 0;
    
    for (int i = 0 ; i < MAXBUFFERS ; i++)
        soundBuf[i] = NULL;
    
    // Create the main DirectSound object.
    if (DirectSoundCreate(NULL, &pDirectSoundObj, NULL) == DS_OK)
    {
        // Set the priority level.
        if (pDirectSoundObj->SetCooperativeLevel(hWnd, DSSCL_NORMAL) != DS_OK)
            pDirectSoundObj = NULL;
    }
    
    // created successfully
    if (pDirectSoundObj)
    {
        CWave wave;
        
        wave.LoadWaveFile("fire.wav");
        SND_FIRE = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("dynamite.wav");
        SND_DYNAMITE = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("enemy.wav");
        SND_ENEMY = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("angel.wav");
        SND_ANGEL = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("elephant.wav");
        SND_ELEPHANT = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("dropbucket.wav");
        SND_DROPBUCKET = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("dropextinguisher.wav");
        SND_DROPEXTINGUISHER = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("dropladder.wav");
        SND_DROPLADDER = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("drop.wav");
        SND_DROP = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("pickupladder.wav");
        SND_PICKUPLADDER = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("pickup.wav");
        SND_PICKUP = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("die1.wav");
        SND_MANDIE[0] = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("die2.wav");
        SND_MANDIE[1] = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("extinguisher.wav");
        SND_EXTINGUISHER = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("win.wav");
        SND_WIN = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("bucket.wav");
        SND_BUCKET = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("steam.wav");
        SND_STEAM = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("enemydie.wav");
        SND_ENEMYDIE = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("water.wav");
        SND_WATER = CreateSoundBuffer(&wave);

        wave.LoadWaveFile("door.wav");
        SND_DOOR = CreateSoundBuffer(&wave);
    }
    
    return pDirectSoundObj != NULL;
}





LPDIRECTSOUNDBUFFER PlayDublicateSound(int buf)
{
    if (pDirectSoundObj)
    {
        for (int i = 0 ; i < MAXBUFFERS ; i++)
            if (!soundBuf[i])
            {
                pDirectSoundObj->DuplicateSoundBuffer(bufferPointers[buf], &soundBuf[i]);
                
                soundBuf[i]->SetCurrentPosition(0);
                soundBuf[i]->Play(0, 0, 0);
                
                return soundBuf[i];
            }
            
            return NULL;
    }
    else
        return NULL;
}






LPDIRECTSOUNDBUFFER PlayInfiniteSound(int buf)
{
    if (pDirectSoundObj)
    {
        LPDIRECTSOUNDBUFFER newBuf;
        pDirectSoundObj->DuplicateSoundBuffer(bufferPointers[buf], &newBuf);
        
        newBuf->SetCurrentPosition(0);
        //newBuf->Play(0, 0, DSBPLAY_LOOPING);
        
        return newBuf;
    }
    else
        return NULL;
}






BOOL DeInitSounds()
{
    if (pDirectSoundObj != NULL)
    {
        // Release all sound buffers.
        for (int x = 0 ; x < MAXBUFFERS ; x++)
            if (soundBuf[x])
                soundBuf[x]->Release();
            
        // Release all sound buffers.
        for (x = 0 ; x < currentBufferNum ; x++)
            //delete bufferPointers[x];//->Release();
            bufferPointers[x]->Release();
        
        // Release the DirectSound object.
        pDirectSoundObj->Release();
    }
    
    return TRUE;
}



