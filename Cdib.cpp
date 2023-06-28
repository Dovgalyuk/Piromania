///////////////////////////////////////////////////////////
// CDIB.CPP: Implementation file for the DIB class.
///////////////////////////////////////////////////////////

#include "cdib.h"
#include "graphics.h"

///////////////////////////////////////////////////////////
// CDib::CDib()
///////////////////////////////////////////////////////////
CDib::CDib(const char* fileName)
{
  m_pBmFileHeader = 0;
  m_pBmInfo = 0;
  m_pBmInfoHeader = 0;
  m_pRGBTable = 0;
  m_pDibBits = 0;
  m_numColors = 0;
  
  // Load the bitmap and initialize the class's data members.
  LoadBitmapFile(fileName);
}



///////////////////////////////////////////////////////////
// CDib::CDib()
///////////////////////////////////////////////////////////
CDib::CDib()
{
  m_pBmFileHeader = 0;
  m_pBmInfo = 0;
  m_pBmInfoHeader = 0;
  m_pRGBTable = 0;
  m_pDibBits = 0;
  m_numColors = 0;
}


///////////////////////////////////////////////////////////
// CDib::~CDib()
///////////////////////////////////////////////////////////
CDib::~CDib()
{
  // Free the memory assigned to the bitmap.
  if (m_pBmInfo)
    delete[] m_pBmInfo;
}




///////////////////////////////////////////////////////////
// CDib::LoadBitmapFile()
//
// This function loads a DIB from disk into memory. It also
// initializes the various class data members.
///////////////////////////////////////////////////////////
void CDib::LoadBitmapFile(HANDLE hFile)
{
  DWORD bytes;
  
  // Read the bitmap's file header into memory.
  BITMAPFILEHEADER bmFileHeader;
  
  ReadFile(hFile, (void*)&bmFileHeader, sizeof(BITMAPFILEHEADER),
    &bytes, NULL);
  
  // Check whether the file is really a bitmap.
  if (bmFileHeader.bfType == 0x4d42)
    // If the file checks out okay, continue loading.
  {
    // Calculate the size of the DIB, which is the
    // file size minus the size of the file header.
    DWORD dibSize = bmFileHeader.bfSize - sizeof(BITMAPFILEHEADER);
    
    // Allocate enough memory to fit the bitmap.
    BYTE* pDib = new BYTE[dibSize];
    
    // Read the bitmap into memory and close the file.
    ReadFile(hFile, (void*)pDib, dibSize, &bytes, NULL);
    
    // Initialize pointers to the bitmap's BITMAPINFO
    // and BITMAPINFOHEADER structures.
    m_pBmInfo = (LPBITMAPINFO) pDib;
    m_pBmInfoHeader = (LPBITMAPINFOHEADER) pDib;
    
    // Calculate a pointer to the bitmap's color table.
    m_pRGBTable = (RGBQUAD*)(pDib + m_pBmInfoHeader->biSize);
    
    // Get the number of colors in the bitmap.
    int m_numColors = GetDibNumColors();
    
    // Calculate the bitmap image's size.
    m_pBmInfoHeader->biSizeImage = GetDibSizeImage();
    
    // Make sure the biClrUsed field is initialized properly.
    if (m_pBmInfoHeader->biClrUsed == 0)
      m_pBmInfoHeader->biClrUsed = m_numColors;
    
    // Calculate a pointer to the bitmap's actual data.
    DWORD clrTableSize = m_numColors * sizeof(RGBQUAD);
    m_pDibBits = pDib + m_pBmInfoHeader->biSize + clrTableSize;
  }
}





///////////////////////////////////////////////////////////
// CDib::LoadBitmapFile()
//
// This function loads a DIB from disk into memory.
///////////////////////////////////////////////////////////
void CDib::LoadBitmapFile(const char* fileName)
{
  HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  
  if (hFile != INVALID_HANDLE_VALUE)
  {
    LoadBitmapFile(hFile);
    CloseHandle(hFile);
  }
}

///////////////////////////////////////////////////////////
// CDib::GetDibSizeImage()
//
// This function calculates and returns the size of the
// bitmap's image in bytes.
///////////////////////////////////////////////////////////
DWORD CDib::GetDibSizeImage()
{
    // If the bitmap's biSizeImage field contains
    // invalid information, calculate the correct size.
    if (m_pBmInfoHeader->biSizeImage == 0)
    {
        // Get the width in bytes of a single row.
        DWORD byteWidth = (DWORD) GetDibWidth();

        // Get the height of the bitmap.
        DWORD height = (DWORD) GetDibHeight();

        // Multiply the byte width by the number of rows.
        DWORD imageSize = byteWidth * height;

        return imageSize;
    }
    // Otherwise, just return the size stored in
    // the BITMAPINFOHEADER structure.
    else
        return m_pBmInfoHeader->biSizeImage;
}

///////////////////////////////////////////////////////////
// CDib::GetDibWidth()
//
// This function returns the width in bytes of a single
// row in the bitmap.
///////////////////////////////////////////////////////////
UINT CDib::GetDibWidth()
{
    return (UINT) m_pBmInfoHeader->biWidth;
}

///////////////////////////////////////////////////////////
// CDib::GetDibHeight()
//
// This function returns the bitmap's height in pixels.
///////////////////////////////////////////////////////////
UINT CDib::GetDibHeight()
{
    return (UINT) m_pBmInfoHeader->biHeight;
}

///////////////////////////////////////////////////////////
// CDib::GetDibNumColors()
//
// This function returns the number of colors in the bitmap.
///////////////////////////////////////////////////////////
UINT CDib::GetDibNumColors()
{
    if ((m_pBmInfoHeader->biClrUsed == 0) &&
          (m_pBmInfoHeader->biBitCount < 9))
        return (1 << m_pBmInfoHeader->biBitCount);
    else
        return (int) m_pBmInfoHeader->biClrUsed;
}

///////////////////////////////////////////////////////////
// CDib::GetDibInfoHeaderPtr()
//
// This function returns a pointer to the bitmap's
// BITMAPINFOHEADER structure.
///////////////////////////////////////////////////////////
LPBITMAPINFOHEADER CDib::GetDibInfoHeaderPtr()
{
    return m_pBmInfoHeader;
}

///////////////////////////////////////////////////////////
// CDib::GetDibInfoPtr()
//
// This function returns a pointer to the bitmap's
// BITMAPINFO structure.
///////////////////////////////////////////////////////////
LPBITMAPINFO CDib::GetDibInfoPtr()
{
    return m_pBmInfo;
}

///////////////////////////////////////////////////////////
// CDib::GetDibRGBTablePtr()
//
// This function returns a pointer to the bitmap's
// color table.
///////////////////////////////////////////////////////////
LPRGBQUAD CDib::GetDibRGBTablePtr()
{
    return m_pRGBTable;
}

///////////////////////////////////////////////////////////
// CDib::GetDibBitsPtr()
//
// This function returns a pointer to the bitmap's
// actual image data.
///////////////////////////////////////////////////////////
BYTE* CDib::GetDibBitsPtr()
{
    return m_pDibBits;
}








///////////////////////////////////////////////////////////
// CDib::CopyDibToSurface()
//
// Copies Dib to surface
///////////////////////////////////////////////////////////
BOOL CDib::CopyDibToSurface(LPDIRECTDRAWSURFACE pSurf, RECT* pRect, int x0, int y0)
{
  // скопировать растр
  DDSURFACEDESC ddSurfaceDesc;
  
  // Initialize the DDSURFACEDESC structure.
  memset(&ddSurfaceDesc, 0, sizeof(DDSURFACEDESC));
  ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
  
  // Lock the destination surface.
  HRESULT result;
  BOOL exitLoop = FALSE;
  do
  {
    // Attempt to lock the surface.
    result = pSurf->Lock(NULL, &ddSurfaceDesc,
      DDLOCK_SURFACEMEMORYPTR, NULL);

    // If the lock failed due to the loss
    // of the surface, restore the surface
    // and try again.
    if (result == DDERR_SURFACELOST)
      RestoreSurfaces();
    else if (result != DDERR_WASSTILLDRAWING)
      exitLoop = TRUE;
  }
  while (!exitLoop);
  
  // If the surface was locked successfully...
  if (result == DD_OK && GetDibBitsPtr())
  {
    // Get the surface width.
//    UINT surfaceWidth = ddSurfaceDesc.dwWidth;
    
    // Get a pointer to the surface's memory, adjusting
    // for the requested destination point.
    char* pSurfaceBits = (char*)ddSurfaceDesc.lpSurface
      + y0*ddSurfaceDesc.lPitch + x0;
    
    // Get the bitmap's width and height.
    UINT width = min(GetDibWidth(), (UINT)pRect->right);
    UINT height = min(GetDibHeight(), (UINT)pRect->bottom);
    
    // Get a pointer to the bitmap's image, adjusting
    // for bottom-up and for the source rectangle.
    char* pDibBits = (((char*)GetDibBitsPtr()) + 
      (GetDibHeight()-1) * GetDibWidth());
    
    // Copy the source rectangle to the
    // destination point.
    for (UINT x = 0 ; x < height ; ++x)
    {
      // Copy one line.
      memcpy(pSurfaceBits, pDibBits, width);
      
      // Calculate pointers to the next line.
      pSurfaceBits += ddSurfaceDesc.lPitch;
      pDibBits -= GetDibWidth();
    }
    
    // Unlock the destination surface.
    pSurf->Unlock(NULL);
  }
  else
    return FALSE;

  return result == DD_OK;
}







///////////////////////////////////////////////////////////
// CDib::CreatePalette()
//
// Creates ddraw palette
///////////////////////////////////////////////////////////
BOOL CDib::CreatePalette(LPDIRECTDRAWPALETTE &pPal)
{
  PALETTEENTRY palEntries[256];
  LPRGBQUAD pColorTable = GetDibRGBTablePtr();
  
  // Copy the bitmap's color table
  // into the palette entries array.
  for (int x = 0; x < 256; ++x)
  {
    palEntries[x].peRed = pColorTable[x].rgbRed;
    palEntries[x].peGreen = pColorTable[x].rgbGreen;
    palEntries[x].peBlue = pColorTable[x].rgbBlue;
  }
  
  // Create a DirectDrawPalette object.
  // If the palette is created unsuccessfully,
  // make the palette's pointer NULL.
  if (pDDrawObj->CreatePalette(DDPCAPS_8BIT,
    palEntries, &pPal, NULL) != DD_OK)
    pPal = NULL;

  return pPal != NULL;
}
