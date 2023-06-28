#ifndef PIROMANIA_H
#define PIROMANIA_H

extern HWND hWnd;
extern HINSTANCE hInst;

const int CHR = 16;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int SPECCY_WIDTH = 512;
const int SPECCY_HEIGHT = 384;

const int CHARS_NUM = SPECCY_WIDTH / CHR;

const int X0 = (SCREEN_WIDTH - SPECCY_WIDTH) / 2;
const int Y0 = (SCREEN_HEIGHT - SPECCY_HEIGHT) / 2;

const int DELAY = 10;


// выйти из программы
void OnClose();


#endif