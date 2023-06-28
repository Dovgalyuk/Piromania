///////////////////////////////////////////////////////////
// CDIB.H: Header file for the DIB class.
///////////////////////////////////////////////////////////

#ifndef CDIB_H
#define CDIB_H


#include <ddraw.h>



class CDib 
{
protected:
  LPBITMAPFILEHEADER m_pBmFileHeader;
  LPBITMAPINFO m_pBmInfo;
  LPBITMAPINFOHEADER m_pBmInfoHeader;
  RGBQUAD* m_pRGBTable;
  BYTE* m_pDibBits;
  UINT m_numColors;
  
public:
  CDib();
  CDib(const char* fileName);
  ~CDib();
  
  DWORD GetDibSizeImage();
  UINT GetDibWidth();
  UINT GetDibHeight();
  UINT GetDibNumColors();
  LPBITMAPINFOHEADER GetDibInfoHeaderPtr();
  LPBITMAPINFO GetDibInfoPtr();
  LPRGBQUAD GetDibRGBTablePtr();
  BYTE* GetDibBitsPtr();
  BOOL CopyDibToSurface(LPDIRECTDRAWSURFACE pSurf,
    RECT* pRect, int x0 = 0, int y0 = 0);
  BOOL CreatePalette(LPDIRECTDRAWPALETTE &pPal);
  
  void LoadBitmapFile(const char* fileName);
  void LoadBitmapFile(HANDLE hFile);
};

#endif

