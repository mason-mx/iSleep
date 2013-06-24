#pragma once

#ifdef AIMAGE_EXPORTS
#define AIMAGE_API __declspec(dllexport)
#else
#define AIMAGE_API  __declspec(dllimport)
#endif

//
// module definition macros
//
// GDI bitmap functions
#define INC_BITMAP 1
// com image function wrapper class
#define INC_COIMAGE 1
// direct draw overlay function wrapper class
#define INC_OVERLAY 1
// direct draw flip GDI function wrapper class
#define INC_FLIPGDI 1


#ifdef INC_BITMAP
#include <windows.h>

// bitmap functions
#ifdef AIMAGE_EXPORTS
HBITMAP AIMAGE_API Screen2Bitmap(int x, int y, int cx, int cy);
void AIMAGE_API DrawBitmap(HDC hdc, wchar_t *lpwszName, int x, int y, int cx, int cy);
void AIMAGE_API DrawBitmap(HDC hdc, HBITMAP hBitmap, int x, int y, int cx, int cy);
void AIMAGE_API DrawBitmap(HDC hdc, HBITMAP hBitmap, int x, int y, int cx, int cy, COLORREF rgbTrans);
void AIMAGE_API SemiTransparentBitmap(HDC hdcDest, int x, int y, int cx, int cy, HDC hdcSrc, int xSrc, int ySrc);
#else
HBITMAP Screen2Bitmap(int x, int y, int cx, int cy);
void DrawBitmap(HDC hdc, wchar_t *lpwszName, int x, int y, int cx, int cy);
void DrawBitmap(HDC hdc, HBITMAP hBitmap, int x, int y, int cx, int cy);
void DrawBitmap(HDC hdc, HBITMAP hBitmap, int x, int y, int cx, int cy, COLORREF rgbTrans);
void SemiTransparentBitmap(HDC hdcDest, int x, int y, int cx, int cy, HDC hdcSrc, int xSrc, int ySrc);
#endif // AIMAGE_EXPORTS
#endif // INC_BITMAP

#ifdef INC_COIMAGE
#include <imaging.h>
#include <GdiPlusEnums.h>

//
// macro definition
//
// zoom image
#define COIMG_ZOOM_SCALE	100
#define COIMG_ZOOM_100X	100		// 1x
#define COIMG_ZOOM_150X	150		// 1.5x
#define COIMG_ZOOM_200X	200		// 2x
#define COIMG_ZOOM_300X	300		// 3x
#define COIMG_ZOOM_400X	400		// 4x
#define COIMG_ZOOM_MAX	COIMG_ZOOM_400X

// pan image
#define COIMG_PAN_DERECT_UP	0
#define COIMG_PAN_DERECT_DOWN	1
#define COIMG_PAN_DERECT_LEFT	2
#define COIMG_PAN_DERECT_RIGHT	3

#define COIMG_PAN_OVER_UP		0x0100
#define COIMG_PAN_OVER_DOWN		0x0200
#define COIMG_PAN_OVER_LEFT	0x0400
#define COIMG_PAN_OVER_RIGHT	0x0800

#define COIMG_PAN_STEP_100X		0
#define COIMG_PAN_STEP_150X		20
#define COIMG_PAN_STEP_200X		30
#define COIMG_PAN_STEP_300X		40
#define COIMG_PAN_STEP_400X		60

// thumbnail
#define COIMG_THUMBNAIL_WIDTH		160
#define COIMG_THUMBNAIL_HEIGHT		120
#define COIMG_THUMB_DEVICE_SCALE	75

// image format
#define COIMG_IMAGEFORMAT_UNSUPPORT	0x0000
#define COIMG_IMAGEFORMAT_BMP	0x0001
#define COIMG_IMAGEFORMAT_JPG	0x0002
#define COIMG_IMAGEFORMAT_EXIF	0x0004
#define COIMG_IMAGEFORMAT_PNG	0x0008
#define COIMG_IMAGEFORMAT_GIF		0x0010
#define COIMG_IMAGEFORMAT_ALL	0x00FF

// error code
#define COIMG_ERROR_SUCCESS		0x0000
#define COIMG_ERROR_CREATEIMAGE	0x0001
#define COIMG_ERROR_DRAWIMAGE	0x0002
#define COIMG_ERROR_CREATEBITMAP	0x0003
#define COIMG_ERROR_QUERYINTERFACE	0x0004
#define COIMG_ERROR_ROTATEIMAGE	0x0005
#define COIMG_ERROR_CREATEDECODER	0x0006
#define COIMG_ERROR_CREATSTREAM	0x0007
#define COIMG_ERROR_GETEXIF	0x0008
#define COIMG_ERROR_DONOTREDRAW	0x0009
#define COIMG_ERROR_FILEISNOTEXIF	0x000A
#define COIMG_ERROR_GETTHUMBNAIL	0x000B

// image info struct
struct COIMG_IMG_INFO{
	char *lpszMaker;
	char *lpszModel;
	char *lpszDatetime;
	ULONG lAperture[2];
	ULONG lExptime[2];
	LONG lShutterspeed[2];
    UINT nWidth;
	UINT nHeight;
	UINT nSize;
};

//
// com image function wrapper class
//
#ifdef AIMAGE_EXPORTS
class AIMAGE_API CCoImage
#else
class CCoImage
#endif // AIMAGE_EXPORTS
{
public:
	CCoImage();
	~CCoImage();

public:
	UINT CoImgDisplayIcon(wchar_t *lpwszFileName, HDC hdc, RECT *lprc);
	UINT CoImgDisplayIcon(HINSTANCE hInst, wchar_t *lpwszResType, UINT nResID, HDC hdc, RECT *lprc);
	UINT CoImgDisplayImage(wchar_t *lpwszFileName, HDC hdc, RECT *lprc);
	UINT CoImgDisplayThumbnail(wchar_t *lpwszFileName, HDC hdc, RECT *lprc);
	UINT CoImgDisplayZoomImage(HDC hdc, RECT *lprc, UINT nZoomFactor, int *px, int *py);
	UINT CoImgDisplayPan(HDC hdc, RECT *lprc, int nZoomFactor, int *px, int *py);
	 // Rotate Angle value can only be 90, 180, 270, or 360 degrees
	UINT CoImgDisplayRotateImage(HDC hdc, RECT *lprc, UINT nRotateAngle);

	UINT CoImgGetImageInfo(wchar_t *lpwszFileName, COIMG_IMG_INFO *imgInfo);
	bool CoImgGetImageExif(IImageDecoder *ppDecoder, PROPID exifID);
	void CoImgGetImageSize(UINT *lpcx, UINT *lpcy);
	UINT CoImgCheckFileFormat(wchar_t *lpwszFileName, UINT nFormat);
	
	void Clean();

private:
	HRESULT CreateStreamOnFile(wchar_t *lpwszFileName, IStream **ppStream, DWORD *lpdwFileSize);
	UINT CoImgDrawImage(IImage *pImage, HDC hdc, RECT *lprc);
	bool CoImgScaleImage(IImage *pImage, RECT *lprcSrc, RECT *lprcDest);
	bool CoImgZoomImage(RECT *lprcSrc, RECT *lprcDest, int nZoomFactor);
	
private:
	IImagingFactory *m_pImagingFactory;
	IImage *m_pImage;
	IImage *m_lpImageZoom;
	PropertyItem *m_lpPropItem;
	HGLOBAL m_hGlobal;

	UINT m_cxPixel;
	UINT m_cyPixel;
	bool m_bInitialized;
};
#endif // INC_COIMAGE

#ifdef INC_FLIPGDI
#include <windows.h>
#include <ddraw.h>

//
// Direct Draw flip GDI function wrapper class
//
#ifdef AIMAGE_EXPORTS
class AIMAGE_API CFlipGDI
#else
class CFlipGDI
#endif // AIMAGE_EXPORTS
{
public:
	CFlipGDI(void);
	~CFlipGDI(void);

public:
	void Init(HWND hWndBG, HWND hWndFG);
	void Clean(void);
	void Update(HWND hWnd);

private:
	void FillBackBuffer(void);

private:
	HWND m_hWndBG; // back ground window
	HWND m_hWndFG; // window to be flipped to foreground
	bool m_bWindowed; // whether driver is capable of rendering in windowed mode

	IDirectDraw *m_lpDDraw;
	IDirectDrawSurface *m_lpDDPrimarySurface;
	IDirectDrawSurface *m_lpDDBackBuffer;
	IDirectDrawClipper *m_lpDDClipper;
};
#endif // INC_FLIPGDI

#ifdef INC_OVERLAY
#include <windows.h>
#include <ddraw.h>

// macro definitions
#ifdef _WIN32_WCE // WINCE
#define LPPrimarySurface LPDIRECTDRAWSURFACE5
#define LPOverlaySurface LPDIRECTDRAWSURFACE5
#define DDrawInterfaceID IID_IDirectDraw4
#define LPDDirectDraw LPDIRECTDRAW4
#else // WIN32
#define RETAILMSG(cond, printf_exp)
#define LPPrimarySurface LPDIRECTDRAWSURFACE7
#define LPOverlaySurface LPDIRECTDRAWSURFACE7
#define DDrawInterfaceID IID_IDirectDraw7
#define LPDDirectDraw LPDIRECTDRAW7
#endif //_WIN32_WCE

//
// direct draw overlay function wrapper class
//
#ifdef AIMAGE_EXPORTS
class AIMAGE_API COverlay
#else
class COverlay
#endif // AIMAGE_EXPORTS
{
public:
	COverlay(void);
	~COverlay(void);

public:
	bool Create(HWND hWnd, HBITMAP hBmp, int x, int y, int cx, int cy, DWORD dwAlphaSrcConst);
	bool Create(HWND hWnd, HBITMAP hBmp, int x, int y, int cx, int cy, DWORD dwColorSpaceLowValue, DWORD dwColorSpaceHighValue);

	void Clean(void);
	bool Show(void);
	void Hide(void);
	bool Move(int x, int y);
	HRESULT RestoreSurfaces(void);

private:
	bool CreatePrimarySurface(void);
	bool CreateOverlaySurface(void);
	bool CopyRGBToYUVSurface(void);
	
private:
	bool m_bAlphaBlend;
	bool m_bColorKey;
	DWORD m_dwAlphaSrcConst;
	DWORD m_dwColorSpaceLowValue;
	DWORD m_dwColorSpaceHighValue;

	HWND m_hWnd;
	HDC m_hdcMem;
	HBITMAP m_hBmpOld;
	RECT m_rcOverlay;
	RECT m_rcDisplay;

	DWORD m_dwDestAlignment;
	LPDDirectDraw m_lpDDraw;
	LPPrimarySurface m_lpPrimarySurface; 
	LPOverlaySurface m_lpOverlaySurface;
};
#endif // INC_OVERLAY