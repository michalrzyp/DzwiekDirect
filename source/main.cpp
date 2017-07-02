#include<Windows.h>
#include<stdio.h>
#include "res.h"
#include <cstdlib>
#include <ctime>
#include <math.h>

#pragma comment(lib,"dsound.lib")

#include"dsound.h"
#define Pi 3.141592
#define czas 35
char snotes[100] = { 'a','b','c','c','c','a','d','c','b','a','g','c','b','b','a','a','b','c','c','c','a','d','c','b','a','g','g','c','e','g','f','g','g','b','b','a','b','b','b','d','d','c' };

double stime[100] = { 24,24,24,4,24,24,24,24,
24,24,24,4,24,24,24,24,
24,24,24,4,24,24,24,24,
24,24,24,4,24,24,24,24 };
void muzyka(BYTE *pBufferForAudio, WAVEFORMATEX pcmWaveFormat, char *snotes, double *stime);
int Note(BYTE* pBufferForAudio, int iStart, int iDuration, float fNote, float fDiv);
int echo(BYTE* pBufferForAudio, int iStart, int iDuration, int iDelay, float iAttenaute);
int echo3(BYTE* pBufferForAudio, int iStart, int iDuration, int iDelay1, float fAttenaute1, int iDelay2, float fAttenaute2, int iDelay3, float fAttenaute3);
CHAR szText[5];
float f1 = 440;
float f2 = 880;
float f;

float fa1 = 440.0;
float fb1 = 493.88;
float fe1 = 329.6;
float ff1 = 349.2;
float fg1 = 392;
float fc2 = 523.25;
float fd2 = 587.32;
float fe2 = 659.25;
float ff2 = 698.45;

INT_PTR CALLBACK DialogProc(HWND hwndDig, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_COMMAND:
    switch (HIWORD(wParam))
    {
    case BN_CLICKED:
      switch (LOWORD(wParam))
      {
      case IDC_BUTTON2:
        //gra(hwndDig);
        return TRUE;
      }
      return FALSE;
    case EN_CHANGE:
      switch (LOWORD(wParam))
      {
      case IDC_EDIT1:
        //HWND hwndEditBox = GetDlgItem(hwndDig, IDC_EDIT1);
        //GetWindowText(hwndEditBox, szText, 3);
        return TRUE;
      }
      return FALSE;
    }
    return FALSE;
  case WM_CLOSE:
    DestroyWindow(hwndDig);
    PostQuitMessage(0);
    return TRUE;
  }
  return FALSE;
}

//utwor na conajmniej 35sek 


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, PSTR szCmdLIne, int iCmdShow)
{
  srand((unsigned int)time(NULL));
  HWND hwndMainWindow = CreateDialog(hinstance, MAKEINTRESOURCE(IDD_MAINVIEW), NULL, DialogProc);
  ShowWindow(hwndMainWindow, iCmdShow);
  MSG msg = {};

  WAVEFORMATEX pcmWaveFormat;
  pcmWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
  pcmWaveFormat.nChannels = 1;
  pcmWaveFormat.nSamplesPerSec = 44100L;
  pcmWaveFormat.wBitsPerSample = 8;
  pcmWaveFormat.nAvgBytesPerSec = pcmWaveFormat.nSamplesPerSec*pcmWaveFormat.wBitsPerSample / 8;
  pcmWaveFormat.nBlockAlign = 1; //2 jak na 16 bitow
  pcmWaveFormat.cbSize = 0;

  MMRESULT mmResult;
  HWAVEOUT hwo = 0;
  UINT devId;
  IDirectSound8 *DS8 = NULL;
  /*for (devId = 0; devId < waveOutGetNumDevs(); devId++)
  {
    mmResult = waveOutOpen(&hwo, devId, &pcmWaveFormat, 0, 0, CALLBACK_NULL);

    if (mmResult == MMSYSERR_NOERROR) break;
  }

  if (mmResult != MMSYSERR_NOERROR)//gdy sie nie udalo
  {
    MessageBox(hwndMainWindow, TEXT("Nie znaleziono karty"), TEXT("Error"), MB_OK);
    return mmResult;
  }
  */
  BYTE *pBufferForAudio = new BYTE[pcmWaveFormat.nAvgBytesPerSec * czas];


  WAVEHDR whdr;

  ZeroMemory(&whdr, sizeof(WAVEHDR));
  whdr.lpData = (LPSTR)pBufferForAudio;
  whdr.dwBufferLength = czas * pcmWaveFormat.nAvgBytesPerSec;

  /*mmResult = waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));
  mmResult = waveOutWrite(hwo, &whdr, sizeof(WAVEHDR));

  while ((whdr.dwFlags & WHDR_DONE) != WHDR_DONE) Sleep(100);
  //czekanie
  mmResult = waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));

  */
  muzyka(pBufferForAudio, pcmWaveFormat, snotes, stime);
 // echo(pBufferForAudio, 0, whdr.dwBufferLength, 44000, 0.5);
  echo3(pBufferForAudio, 0, whdr.dwBufferLength, 44000 / 3.0, 0.5, 44000/ 2.0, 0.4, 34000, 0.3);


  DirectSoundCreate8(NULL, &DS8, NULL);
  if (DS8 == NULL)
  {
    MessageBox(0, TEXT("Nie mozna  ..."), TEXT("Error"), MB_OK);
  }
  if (DS8->SetCooperativeLevel(hwndMainWindow, DSSCL_NORMAL) != DS_OK)
  {
    MessageBox(0, TEXT("qqq"), TEXT("Komunikat"), MB_OK);
    return true;
  }

  IDirectSoundBuffer *DSB;
  DSBUFFERDESC DSBD;
  memset(&DSBD, 0, sizeof(DSBUFFERDESC));
  DSBD.dwSize = sizeof(DSBUFFERDESC);
  DSBD.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;//proporcja miedzy lewym a prawym kanalem
  DSBD.dwBufferBytes = whdr.dwBufferLength;
  DSBD.lpwfxFormat = &pcmWaveFormat;
  if (DS8->CreateSoundBuffer(&DSBD, &DSB, NULL) != DS_OK)
  {
    MessageBox(0, TEXT("aaa"), TEXT("Komunikat"), MB_OK);
    return true;
  }

  DSB->SetVolume(0xFFFF);
  void *ptr1 = NULL, *ptr2 = NULL;
  DWORD l1, l2;
  DSB->Lock(0, whdr.dwBufferLength, &ptr1, &l1, &ptr2, &l2, DSBLOCK_ENTIREBUFFER);//blokowanie bufora
  if (ptr1) memcpy(ptr1, pBufferForAudio, l1);
  if (ptr2) memcpy(ptr2, pBufferForAudio + l1, l2);
  DSB->Unlock(ptr1, l1, ptr2, l2);

  DSB->Play(0, 0, 0);
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  /*for (int i = 0; i < 4 * pcmWaveFormat.nAvgBytesPerSec; i++)
  {
  float t = i / (float)pcmWaveFormat.nSamplesPerSec;
  f = f1 + (f2 - f1)*t;
  pBufferForAudio[i] = (128 * cos(2 * Pi*f*t) + 128);
  }

  ZeroMemory(&whdr, sizeof(WAVEHDR));
  whdr.lpData = (LPSTR)pBufferForAudio;
  whdr.dwBufferLength = 4 * pcmWaveFormat.nAvgBytesPerSec;

  mmResult = waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));
  mmResult = waveOutWrite(hwo, &whdr, sizeof(WAVEHDR));

  while ((whdr.dwFlags & WHDR_DONE) != WHDR_DONE) Sleep(100);
  //czekanie
  */
  DSB->Release();
  delete[] pBufferForAudio;

  return 0;
}
void muzyka(BYTE *pBufferForAudio, WAVEFORMATEX pcmWaveFormat, char *snotes, double  *stime)
{
  float S = pcmWaveFormat.nAvgBytesPerSec;
  float nuta = 0.0;
  float czasNuty;
  int k = 0;
  for (int j = 0; j < 100; j++)
  {
    czasNuty = (stime[j]) / 32.0;
    switch (snotes[j])
    {
    case 'a': nuta = fa1; break;
    case 'b': nuta = fb1; break;
    case 'c': nuta = fc2; break;
    case 'd': nuta = fd2; break;
    case 'e': nuta = fe2; break;
    case 'g': nuta = fg1; break;
    case 'f': nuta = ff2; break;
    }
    k = Note(pBufferForAudio, k, czasNuty*S, nuta, pcmWaveFormat.nSamplesPerSec);
  }
}

int Note(BYTE* pBufferForAudio, int iStart, int iDuration, float fNote, float fDiv)
{
  int i;
  for (i = iStart; i < iStart + iDuration - 200; i++)
  {
    float t = i / fDiv;
    // pBufferForAudio[i] = ((128 * sin(2 * Pi*fNote*t) + 128) + (128 * sin(2 * Pi*fNote*t*2.0 / 3.0) + 128)) / 2;//dwa rozne tony
    pBufferForAudio[i] = ((128 * sin(2 * Pi*fNote*t) + 128) + (128 * sin(2 * Pi*fNote*t*1.0 / 2.0) + 128)) / 2;//dwie rozne gamy
  }
  for (; i < iStart + iDuration; i++)
  {
    float t = i / fDiv;
    pBufferForAudio[i] = 128;
  }
  return i;
}

int echo(BYTE* pBufferForAudio, int iStart, int iDuration, int iDelay, float iAttenaute)
{
  //zrobic 3 echa
  BYTE *tmp = new BYTE[iDuration];
  memcpy(tmp, pBufferForAudio, iDuration);
  for (int i = 0; i < iDuration; i++)
  {
    if (i - iDelay > 0)
      //pBufferForAudio[i] = (tmp[i] + tmp[i - iDelay] * iAttenaute / (1.0 + iAttenaute));
      pBufferForAudio[i] = ((tmp[i] - 128.0) + (tmp[i - iDelay] - 128.0) * iAttenaute) / (1.0 + iAttenaute) + 128;
    else
    {
      pBufferForAudio[i] = tmp[i];
    }
  }
  delete[]tmp;
  return 0;
}
int echo3(BYTE* pBufferForAudio, int iStart, int iDuration, int iDelay1, float fAttenaute1,
  int iDelay2, float fAttenaute2, int iDelay3, float fAttenaute3)
{
  BYTE *tmp = new BYTE[iDuration];
  int iSample1 = 0;
  int iSample2 = 0;
  int iSample3 = 0;
  memcpy(tmp, pBufferForAudio, iDuration);
  for (int i = 0; i < iDuration; i++)
  {
    if (i - iDelay1 > 0)
      iSample1 = tmp[i - iDelay1] - 128.0;
    if (i - iDelay2 > 0)
    {
      iSample2 = tmp[i - iDelay2] - 128.0;
    }
    if (i - iDelay3 > 0)
    {
      iSample3 = tmp[i - iDelay3] - 128.0;
    }
    //pBufferForAudio[i] = (tmp[i] + tmp[i - iDelay] * iAttenaute / (1.0 + iAttenaute));
    pBufferForAudio[i] = ((tmp[i] - 128.0) + iSample1*fAttenaute1 + iSample2*fAttenaute2 + iSample3*fAttenaute3)
      / (1.0 + fAttenaute1 + fAttenaute2 + fAttenaute3) + 128;
  }
  delete[]tmp;
  return 0;
}
//do kolka i krzyzk w openGL dodac dzwiek chociaz 1 dzwiek przy wstawianiu 