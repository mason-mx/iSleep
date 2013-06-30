#pragma once
#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 
#include "UserDef.h"

// Returns number of elements
#define dim(x) (sizeof(x) / sizeof(x[0]))
#define MulDiv(a,b,c)       (((a)*(b))/(c))

//----------------------------------------------------------------------
// Generic defines and data types
//
struct decodeUINT {                             // Structure associates
    UINT Code;                                  // messages
                                                // with a function.
    LRESULT (*Fxn)(HWND, UINT, WPARAM, LPARAM);
};
struct decodeCMD {                              // Structure associates
    UINT Code;                                  // menu IDs with a
    LRESULT (*Fxn)(HWND, WORD, HWND, WORD);     // function.
};

HFONT CreateFont(LONG fHeight,LONG fWidth, LONG fEscapement, LONG fOrientation, LONG lfWeight,BYTE fItalic,BYTE fUnderline, BYTE fStrikeOut, 
		   BYTE fCharset,BYTE fOutPrecision, BYTE fClipPrecision, BYTE fQuality, BYTE fPitchAndFamily, HWND hWnd);

BOOL TWRegisterClass(HINSTANCE hInstance);
BOOL AWRegisterClass(HINSTANCE hInstance);
BOOL PVRegisterClass(HINSTANCE hInstance);
BOOL MediaRegisterClass(HINSTANCE hInstance);
BOOL XACRegisterClass(HINSTANCE hInstance);

LRESULT CALLBACK TWWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AWWndProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TSTDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK STDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SDDlgProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SetupDlgProc (HWND, UINT, WPARAM, LPARAM);

HWND ShowTimeWnd(HWND hWnd);
HWND ShowImmeTimeWnd(HWND hWnd,SYSTEMTIME immeTime);
BOOL ShowAlertWnd(HWND hWnd);
BOOL UpdateImmeTimeWnd(SYSTEMTIME immeTime);
BOOL SetAlert(SYSTEMTIME st);
BOOL SetAlertIntervalLoop(int Interval);
BOOL SetAlertIntervaltoOff(int Interval);
BOOL ShowPVWnd(HWND hWnd);
BOOL ShowFSWnd(HWND hWnd);
BOOL ShowMediaWnd(HWND hWnd);
void PlayPauseMedia();
void PlayMedia();
void PauseMedia();
UINT FindMediaFiles(int index);
UINT FindPictures(int index);
BOOL ShowImmeXACWnd(HWND hWnd,SYSTEMTIME immeTime);
BOOL ShowXACWnd(HWND hWnd);
void XACStop();
void XACStart();