/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/

#pragma once
#include "..\\Common\\AImage\\AImage.h"

//
// macro definitions
//

#define dim(x) (sizeof(x) / sizeof(x[0]))
#define SAFE_DELETE_OBJECT(x) if (x) {DeleteObject(x); x = NULL;}

#define TITLE_COLOR RGB(255,255,255) // title color
#define PNG_IMAGE_FOCUS TEXT("USB Disk\\Resource\\APictureViewer\\focus.png") 

// structure associates message with functions
struct decodeUINT {
	UINT nID;  
	LRESULT (*Fxn)(HWND, UINT, WPARAM, LPARAM);
};

// control buttons struct
struct PICTURE_CTRL_BUTTON {
	UINT nIDCtrl; // control item identifier
	RECT rcCtrl; // control item position rectangle
	wchar_t *lpwszImage; // control button image
};

// picture list item struct
struct PICTURE_LIST_ITEM {
	UINT nIndex; // item index identifier
	wchar_t wszName[MAX_PATH]; // picture name
	wchar_t wszPath[MAX_PATH]; // pictur path
};

// function for invoking externally
bool NextImage();
bool PreviousImage();