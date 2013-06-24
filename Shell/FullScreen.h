/*
Copyright:
    Copyright (c) 2010 Netlogic Microsystems.  All rights reserved.

Abstract:

Author:
    Franklin January 2010
    
Revision History:
*/

#pragma once

// create full screen view window
HWND CreateFullScreenViewWindow(int x, int y, int cx, int cy, HWND hWndParent, HINSTANCE hInstance);
void SetFilePath(wchar_t *lpwszFile);