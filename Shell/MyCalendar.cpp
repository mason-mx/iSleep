#include "stdafx.h"
#include "MyCalendar.h"
#include "shell.h"

/* MyCalendar texts */
TCHAR g_Months[][20] = { L"Jan", L"Feb", L"Mar", L"April", L"May", L"June", L"July", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};
TCHAR g_Days[][10] = {L"SU", L"MO", L"TU", L"WE", L"TH", L"FR", L"SA"};
TCHAR g_Today[] = L"Today";//L"Danas:";
TCHAR g_Easter[] = L"(U)skrs";
TCHAR g_EasterMark[] = L"U";
TCHAR g_Calendar[] = L"Calendar";

/* MyCalendar global variables */
MC_LIST *g_MCList = NULL;
int g_MCCalNumber = 0;
HBITMAP g_Marker = NULL;


CMyCalendar::CMyCalendar()
{
     // Init window handle
     m_hWnd = NULL;
}


CMyCalendar::~CMyCalendar()
{
}


BOOL CMyCalendar::Create(RECT wndRect, HWND hParentWnd)
{
     BOOL result = FALSE;
     
     // Set MyCalendar parent window handle
     m_hParentWnd = hParentWnd;
     
     /*WNDCLASS wincl;
     wincl.hInstance = GetModuleHandle(NULL);
     wincl.lpszClassName = "MYCALENDAR";
     wincl.lpfnWndProc = MyCalendarProc;
     wincl.style = CS_DBLCLKS;
     wincl.cbSize = sizeof (WNDCLASSEX);
     wincl.hIcon = LoadIcon( NULL, IDI_APPLICATION );
     wincl.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
     wincl.hCursor = LoadCursor( NULL, IDC_ARROW );
     wincl.lpszMenuName = NULL;
     wincl.cbClsExtra = 0;
     wincl.cbWndExtra = 0;
     wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);*/

	 WNDCLASS wndclass = {0}; 
	DWORD    wStyle   = 0;

	wndclass.style         = 0;//CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) MyCalendarProc; 
	wndclass.hInstance     = GetModuleHandle(NULL); 
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//CreateSolidBrush(RGB(0,0,20));;//(HBRUSH) GetStockObject (DKGRAY_BRUSH);
	wndclass.lpszClassName = TEXT ("MYCALENDAR"); 
     
     ATOM reg = RegisterClass(&wndclass);
     DWORD error = GetLastError();
     
     // Register MyCalendar window class
     if ( ( reg ) || ( error == ERROR_CLASS_ALREADY_EXISTS ) )
     {
          // Create MyCalendar window
          m_hWnd = CreateWindowEx( 0, L"MYCALENDAR", L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 
                 wndRect.left, wndRect.top+18, 200, 20, hParentWnd, NULL, GetModuleHandle(NULL), NULL );
          
          // Create Date window
          m_hDateWnd = CreateWindowEx( WS_EX_STATICEDGE, L"STATIC", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
                 0, 0, 130, 20, m_hWnd, NULL, GetModuleHandle(NULL), NULL );
          
          // Create Year window
          //m_hYearWnd = CreateWindowEx( 0, L"EDIT", L"", WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | ES_CENTER, 
          //       146, 23, 32, 18, m_hWnd, NULL, GetModuleHandle(NULL), NULL );
                 
          // Create Calendar button window
          m_hCalWnd = CreateWindowEx( WS_EX_STATICEDGE , L"BUTTON", g_Calendar, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
                 130, 0, 71, 20, m_hWnd, NULL, GetModuleHandle(NULL), NULL );
                 
          if ( m_hWnd )
          {
               AddMCCal( m_hWnd, m_hParentWnd, m_hDateWnd, m_hCalWnd, m_hYearWnd );
               
               // Init MyCalendar GDI objects
               InitMCGlobals( hParentWnd, m_hWnd );
               
               MC_LIST* mcList = GetMCCal(m_hWnd);
               if ( mcList != NULL )
               {
                    SendMessage( m_hDateWnd, WM_SETFONT, (WPARAM)mcList->mc_hDateFont, MAKELPARAM(TRUE,0) );
                    //SendMessage( m_hCalWnd, WM_SETFONT, (WPARAM)mcList->mc_hHeaderFont, MAKELPARAM(TRUE,0) );
                    //SendMessage( m_hYearWnd, WM_SETFONT, (WPARAM)mcList->mc_hBodyFont, MAKELPARAM(TRUE,0) );
                    //SendMessage( m_hYearWnd, EM_SETLIMITTEXT, (WPARAM)4, 0 );
                    //SendMessage( m_hYearWnd, EM_SETMARGINS, (WPARAM)(EC_LEFTMARGIN | EC_RIGHTMARGIN), (LPARAM)MAKELONG(0,0) );
               }
               
               result = TRUE;
          }
          
          //if ( g_Marker == NULL )
          //     g_Marker = (HBITMAP)LoadImage( NULL, "Marker.bmp", IMAGE_BITMAP, 15, 15, LR_DEFAULTCOLOR | LR_LOADFROMFILE );
     }
     
     return result;
}


LRESULT CALLBACK MyCalendarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
        case WM_PAINT:
        {
             PAINTSTRUCT ps;
             BeginPaint( hwnd, &ps );
             
             MC_LIST* mcList = GetMCCal(hwnd);
             if ( mcList != NULL )
             {
                  if ( mcList->mc_Expand == TRUE )
                  {
                       // Draw MyCalendar header part
                       DrawHeader(mcList->mc_hWnd);
                       
                       // Draw MyCalendar body part
                       DrawBody(mcList->mc_hWnd);
                  
                       RECT clientRect;
                       GetClientRect( mcList->mc_hWnd, &clientRect );
                       BitBlt( ps.hdc, 0, 0, (clientRect.right-clientRect.left), (clientRect.bottom-clientRect.top), mcList->mc_hMemDC, 0, 0, SRCCOPY );
                  }
             }
             
             EndPaint( hwnd, &ps );
        }
        break;
        
        case WM_LBUTTONDOWN:
        {
             MC_LIST* mcList = GetMCCal(hwnd);
             if ( mcList != NULL )
             {
                  POINT pt = {LOWORD(lParam), HIWORD(lParam)};
                  CheckCellClick( mcList->mc_hWnd, pt );
                  InvalidateRect( hwnd, NULL, TRUE );
                  UpdateWindow(hwnd);
             }
        }
        break;
        
        case WM_ERASEBKGND:
        {
             MC_LIST* mcList = GetMCCal(hwnd);
             if ( mcList != NULL )
             {
                  RECT clientRect;
                  GetClientRect( mcList->mc_hWnd, &clientRect );
                  FillRect( mcList->mc_hMemDC, &clientRect, mcList->mc_hBkBrush );
             }
        }
        break;

        case WM_CREATE:
        {
             MC_LIST* mcList = GetMCCal(hwnd);
             if ( mcList != NULL )
                  SetFocus(mcList->mc_hWnd);
        }
        break;
        
        case WM_DESTROY:
             DestroyMCCal(hwnd);
             break;

        case WM_SIZE:
        {
             MC_LIST* mcList = GetMCCal(hwnd);
             if ( mcList != NULL )
             {
                  // Delete memory device context
                  if ( mcList->mc_hMemDC )
                  {
                       SelectObject( mcList->mc_hMemDC, mcList->mc_hOldMemBitmap );
                       DeleteDC(mcList->mc_hMemDC);
                       mcList->mc_hMemDC = NULL;
                  }
                  // Delete memory bitmap
                  if ( mcList->mc_hMemBitmap )
                  {
                       DeleteObject(mcList->mc_hMemBitmap);
                       mcList->mc_hMemBitmap = NULL;
                  }
                  
                  HDC hDC = GetDC(mcList->mc_hWnd);
                  RECT rectClient;
                  GetClientRect( mcList->mc_hWnd, &rectClient );
                  mcList->mc_hMemBitmap = CreateCompatibleBitmap( hDC, (rectClient.right-rectClient.left), (rectClient.bottom-rectClient.top) );
                  mcList->mc_hMemDC = CreateCompatibleDC(hDC);
                  mcList->mc_hOldMemBitmap = (HBITMAP)SelectObject( mcList->mc_hMemDC, mcList->mc_hMemBitmap );
                  ReleaseDC( mcList->mc_hWnd, hDC );
             }
        }
	    break;
	    
	    case WM_COMMAND:
        {
             MC_LIST* mcList = GetMCCal(hwnd);
             if ( mcList != NULL )
             {
                  /* Month/Year forward/backward buttons pressed */
                  if ( (HWND)lParam == mcList->mc_hButtonMonthBack )
                  {
                       if ( mcList->mc_Month == 1 )
                            mcList->mc_Month = 12;
                       else
                            mcList->mc_Month--;
                  }
                  else if ( (HWND)lParam == mcList->mc_hButtonMonthForward )
                  {
                       if ( mcList->mc_Month == 12 )
                            mcList->mc_Month = 1;
                       else
                            mcList->mc_Month++;
                  }
                  if ( (HWND)lParam == mcList->mc_hButtonYearBack )
                  {
                       if ( mcList->mc_Year == mcList->mc_StartYear )
                            mcList->mc_Year = mcList->mc_EndYear;
                       else
                            mcList->mc_Year--;
                  }
                  if ( (HWND)lParam == mcList->mc_hButtonYearForward )
                  {
                       if ( mcList->mc_Year == mcList->mc_EndYear )
                            mcList->mc_Year = mcList->mc_StartYear;
                       else
                            mcList->mc_Year++;
                  }
                  
                  /* Calendar button pressed */
                  if ( (HWND)lParam == mcList->mc_hCalWnd )
                  {
                       // Get MyCalendar window rectangle
                       RECT wndRect;
                       GetWindowRect( mcList->mc_hWnd, &wndRect );
                       
                       // Expand/Collapse MyCalendar window
                       if ( mcList->mc_Expand == FALSE )
                       {
                            mcList->mc_Expand = TRUE;
                            SetWindowPos( mcList->mc_hWnd, HWND_NOTOPMOST, 0, 0, wndRect.right-wndRect.left, 210, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
                            
                            // Send notification message
                            SendMessage( mcList->mc_hParent, WM_COMMAND, MAKEWPARAM(0,MC_EXPANDED), (LPARAM)mcList->mc_hWnd );
                       }
                       else
                       {
                            mcList->mc_Expand = FALSE;
                            SetWindowPos( mcList->mc_hWnd, HWND_NOTOPMOST, 0, 0, wndRect.right-wndRect.left, 20, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
                            
                            // Send notification message
                            SendMessage( mcList->mc_hParent, WM_COMMAND, MAKEWPARAM(0,MC_COLLAPSED), (LPARAM)mcList->mc_hWnd );
                       }
                  }
                  
                  /* Year is directly edited */
                  if ( (HWND)lParam == mcList->mc_hYearWnd )
                  {
                       if ( HIWORD(wParam) == EN_CHANGE )
                       {
                            char year[5];
                            SendMessage( mcList->mc_hYearWnd, WM_GETTEXT, (WPARAM)5, (LPARAM)year );
                            if ( strlen(year) == 4 )
                            {
                                 ShowWindow( mcList->mc_hYearWnd, SW_HIDE );
                                 int y = atoi(year);
                                 if ( y < mcList->mc_StartYear )
                                      y = mcList->mc_StartYear;
                                 if ( y > mcList->mc_EndYear )
                                      y = mcList->mc_EndYear;
                                 mcList->mc_Year = y;
                                 FillDates(mcList->mc_hWnd);
                            }
                       }
                       else if ( HIWORD(wParam) == EN_KILLFOCUS )
                       {
                            ShowWindow( mcList->mc_hYearWnd, SW_HIDE );
                            FillDates(mcList->mc_hWnd);
                       }
                  }
                  
                  FillDates(hwnd);
                  InvalidateRect( hwnd, NULL, TRUE );
                  UpdateWindow(hwnd);
             }
        }
        break;
             
        default:
             return DefWindowProc( hwnd, message, wParam, lParam );
     }

     return 0;
}


void InitMCGlobals(HWND hParent, HWND hWnd)
{
     MC_LIST* mcList = GetMCCal(hWnd);
     if ( mcList != NULL )
     {
          // Create MyCalendar device context
          HDC hDC = GetDC(mcList->mc_hWnd);
          RECT rectClient;
          GetClientRect( mcList->mc_hWnd, &rectClient );
          mcList->mc_hMemBitmap = CreateCompatibleBitmap( hDC, (rectClient.right-rectClient.left), (rectClient.bottom-rectClient.top) );
          mcList->mc_hMemDC = CreateCompatibleDC(hDC);
          mcList->mc_hOldMemBitmap = (HBITMAP)SelectObject( mcList->mc_hMemDC, mcList->mc_hMemBitmap );
          ReleaseDC( mcList->mc_hWnd, hDC );
                  
          // Create MyCalendar background brush
          mcList->mc_hBkBrush = CreateSolidBrush(MC_BK_COLOR);
          
         // Create MyCalender header font
		 HDC hParentDC = GetDC(mcList->mc_hParent);
         /*LOGFONT lf;
         lf.lfHeight = -MulDiv( 9, GetDeviceCaps(hParentDC,LOGPIXELSY), 72 );
         lf.lfWidth = 0;
         lf.lfEscapement = 0;
         lf.lfOrientation = 0;
         lf.lfWeight = FW_NORMAL;
         lf.lfItalic = FALSE;
         lf.lfUnderline = FALSE;
         lf.lfStrikeOut = FALSE;
         lf.lfCharSet = ANSI_CHARSET;
         lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
         lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
         lf.lfQuality = DEFAULT_QUALITY;
         lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
         strcpy( lf.lfFaceName, "Arial" );*/
         mcList->mc_hHeaderFont = CreateFont(9, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
         /*lf.lfHeight = -MulDiv( 8, GetDeviceCaps(hParentDC,LOGPIXELSY), 72 );
         lf.lfItalic = TRUE;*/
         mcList->mc_hBodyFont = CreateFont(8, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
         /*lf.lfWidth = 4;
         lf.lfHeight = -MulDiv( 9, GetDeviceCaps(hParentDC,LOGPIXELSY), 72 );
         lf.lfWeight = FW_NORMAL;
         lf.lfItalic = FALSE;*/
         mcList->mc_hWeekFont = CreateFont(9, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
         /*lf.lfHeight = -MulDiv( 11, GetDeviceCaps(hParentDC,LOGPIXELSY), 72 );
         lf.lfWidth = 0;*/
         mcList->mc_hDateFont = CreateFont(11, 0, 0, 0, 0, FALSE, FALSE, FALSE, 1, 0, 0, 0, 0,hWnd);
         ReleaseDC( mcList->mc_hParent, hParentDC );
          
          // Create MyCalendar buttons
          mcList->mc_hButtonMonthBack = CreateWindowEx( 0, L"BUTTON", L"<", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON , 
                 5, 24, 15, 15, mcList->mc_hWnd, NULL, GetModuleHandle(NULL), NULL );
          mcList->mc_hButtonMonthForward = CreateWindowEx( 0, L"BUTTON", L">", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON , 
                 88, 24, 15, 15, mcList->mc_hWnd, NULL, GetModuleHandle(NULL), NULL );
          mcList->mc_hButtonYearBack = CreateWindowEx( 0, L"BUTTON", L"<", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON , 
                 rectClient.right-71, 24, 15, 15, mcList->mc_hWnd, NULL, GetModuleHandle(NULL), NULL );
          mcList->mc_hButtonYearForward = CreateWindowEx( 0, L"BUTTON", L">", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON , 
                 rectClient.right-20, 24, 15, 15, mcList->mc_hWnd, NULL, GetModuleHandle(NULL), NULL );
          SendMessage( mcList->mc_hButtonMonthBack, WM_SETFONT, (WPARAM)mcList->mc_hHeaderFont, MAKELPARAM(TRUE,0) );
          SendMessage( mcList->mc_hButtonMonthForward, WM_SETFONT, (WPARAM)mcList->mc_hHeaderFont, MAKELPARAM(TRUE,0) );
          SendMessage( mcList->mc_hButtonYearBack, WM_SETFONT, (WPARAM)mcList->mc_hHeaderFont, MAKELPARAM(TRUE,0) );
          SendMessage( mcList->mc_hButtonYearForward, WM_SETFONT, (WPARAM)mcList->mc_hHeaderFont, MAKELPARAM(TRUE,0) );
     }
}


BOOL AddMCCal(HWND hWnd, HWND hParent, HWND hDate, HWND hCal, HWND hYear)
{
     BOOL result = FALSE;
     
     // Check number of created MyCalendars
     if ( g_MCCalNumber < MC_MAXCAL )
     {
          // Get current local time
          SYSTEMTIME st;
          GetLocalTime(&st);
          
          // Create new MyCalendar
          MC_LIST* newMCCal = new MC_LIST;
          newMCCal->mc_hWnd = hWnd;
          newMCCal->mc_hDateWnd = hDate;
          newMCCal->mc_hCalWnd = hCal;
          newMCCal->mc_hYearWnd = hYear;
          newMCCal->mc_hParent = hParent;
          newMCCal->mc_hMemBitmap = NULL;
          newMCCal->mc_hMemDC = NULL;
          newMCCal->mc_hOldMemBitmap = NULL;
          newMCCal->mc_hBkBrush = NULL;
          newMCCal->mc_hButtonMonthBack = NULL;
          newMCCal->mc_hButtonMonthForward = NULL;
          newMCCal->mc_hButtonYearBack = NULL;
          newMCCal->mc_hButtonYearForward = NULL;
          newMCCal->mc_hHeaderFont = NULL;
          newMCCal->mc_hBodyFont = NULL;
          newMCCal->mc_hWeekFont = NULL;
          newMCCal->mc_hBodyFont = NULL;
          newMCCal->mc_hDateFont = NULL;
          newMCCal->mc_Day = st.wDay;
          newMCCal->mc_Month = st.wMonth;
          newMCCal->mc_Year = st.wYear;
          newMCCal->mc_Expand = FALSE;
          newMCCal->mc_Edit = FALSE;
          newMCCal->mc_StartYear = 1900;
          newMCCal->mc_EndYear = 2099;
          newMCCal->mc_MarkSunday = TRUE;
          newMCCal->mc_MarkToday = TRUE;
          newMCCal->mc_MarkEaster = TRUE;
          newMCCal->mc_GregorianEaster = TRUE;
          newMCCal->next = NULL;
          
          MC_LIST* curr = g_MCList;
          if ( curr == NULL )
               g_MCList = newMCCal;
          else
          {
               while ( curr->next != NULL )
                     curr = curr->next;
               curr->next = newMCCal;
          }
          
          g_MCCalNumber++;
          result = TRUE;
          
          // Fill dates list
          FillDates(hWnd);
     }
     
     return result;
}


MC_LIST* GetMCCal(HWND hWnd)
{
     MC_LIST* result = NULL;
     
     if ( g_MCCalNumber > 0 )
     {
          // Find MyCalendar
          MC_LIST *mcList = g_MCList;
          while ( ( mcList != NULL ) && ( mcList->mc_hWnd != hWnd ) )
               mcList = mcList->next;   
          
          result = mcList;
     }
     
     return result;
}


MC_LIST* DetachMCCal(HWND hWnd)
{
     MC_LIST* result = NULL;
     
     if ( g_MCCalNumber > 0 )
     {
          // Find MyCalendar
          MC_LIST *curr, *prev;
          curr = g_MCList;
          while ( ( curr != NULL ) && ( curr->mc_hWnd != hWnd ) )
          {
                prev = curr;
                curr = curr->next;
          }
          
          if ( curr == g_MCList )
               g_MCList = g_MCList->next;
          else
               prev->next = curr->next;    
          
          result = curr;
     }
     
     return result;
}


void DestroyMCCal(HWND hWnd)
{
     MC_LIST* mcList = DetachMCCal(hWnd);
     if ( mcList != NULL )
     {
          // Delete background brush
          if ( mcList->mc_hBkBrush )
          {
               DeleteObject(mcList->mc_hBkBrush);
               mcList->mc_hBkBrush = NULL;
          }
          // Delete memory device context
          if ( mcList->mc_hMemDC )
          {
               SelectObject( mcList->mc_hMemDC, mcList->mc_hOldMemBitmap );
               DeleteDC(mcList->mc_hMemDC);
               mcList->mc_hMemDC = NULL;
          }
          // Delete memory bitmap
          if ( mcList->mc_hMemBitmap )
          {
               DeleteObject(mcList->mc_hMemBitmap);
               mcList->mc_hMemBitmap = NULL;
          }
          // Delete header font
          if ( mcList->mc_hHeaderFont )
          {
               DeleteObject(mcList->mc_hHeaderFont);
               mcList->mc_hHeaderFont = NULL;
          }
          // Delete body font
          if ( mcList->mc_hBodyFont )
          {
               DeleteObject(mcList->mc_hBodyFont);
               mcList->mc_hBodyFont = NULL;
          }
          // Delete week font
          if ( mcList->mc_hWeekFont )
          {
               DeleteObject(mcList->mc_hWeekFont);
               mcList->mc_hWeekFont = NULL;
          }
         
          delete mcList;
          g_MCCalNumber--;
     }
}


void DrawHeader(HWND hWnd)
{
     // Find MyCalendar
     MC_LIST* mcList = GetMCCal(hWnd);

     // Get MyCalendar DC
     HDC hDC = mcList->mc_hMemDC;
     HFONT hOldFont = (HFONT)SelectObject( hDC, mcList->mc_hHeaderFont );
     HPEN hWeekDayPen = CreatePen( PS_SOLID, 1, MC_WEEK_TXTCOLOR );
     HPEN hOldPen = (HPEN)SelectObject( hDC, hWeekDayPen );
     HBRUSH hOldBrush = (HBRUSH)SelectObject( hDC, mcList->mc_hBkBrush );
     
     // Get MyCalendar window rect
     RECT clientRect;
     GetClientRect( mcList->mc_hWnd, &clientRect );
     Rectangle( hDC, 0, clientRect.top+24, clientRect.right, clientRect.bottom );
     
     //  Adjust client rectangle
     clientRect.top += 20;
     clientRect.bottom = clientRect.top + 24;
     
     // Draw header part background
     DrawEdge( hDC, &clientRect, EDGE_RAISED, BF_ADJUST | BF_MIDDLE | BF_RECT | BF_FLAT );
     SetBkMode( hDC, TRANSPARENT );
     COLORREF oldTextColor = SetTextColor( hDC, MC_MONTH_TXTCOLOR );
     //SetTextAlign( hDC, TA_CENTER );
	 RECT rcMonths = {30, 25, 90, 40};
	 DrawText(hDC, g_Months[mcList->mc_Month-1], -1, &rcMonths, DT_CENTER);
     //TextOut( hDC, 54, 24, g_Months[mcList->mc_Month-1], wcslen(g_Months[mcList->mc_Month-1]) );
     //SetTextAlign( hDC, TA_RIGHT );
     TCHAR year[4];
	 RECT rcYear = {143, 25, 177, 40};
     _itow( mcList->mc_Year, year, 10 );
	  DrawText(hDC,year, -1, &rcYear, DT_CENTER);
     //TextOut( hDC, clientRect.right-23, 24, year, 4 );
     
     // Draw month table header
     //SetTextAlign( hDC, TA_CENTER );
     SetTextColor( hDC, MC_WEEK_TXTCOLOR );
     SelectObject( hDC, mcList->mc_hWeekFont );
     RECT rect = {8, 48, 35, 80};
     for ( int d=0; d<7; d++ )
     {
		 DrawText(hDC, g_Days[d], -1, &rect, 0);
         //TextOut( hDC, (rect.left+rect.right-3)/2, 48, g_Days[d], strlen(g_Days[d]) );
         rect.left = rect.right;
         rect.right += 27;
     }
     // Line borders
     MoveToEx( hDC, clientRect.left+5, 62, NULL );
     LineTo( hDC, clientRect.right-5, 62 );
     GetClientRect( mcList->mc_hWnd, &clientRect );
     MoveToEx( hDC, clientRect.left+5, clientRect.bottom-20, NULL );
     LineTo( hDC, clientRect.right-5, clientRect.bottom-20 );
     
	 clientRect.left += 8;
	 clientRect.right -= 8;
	 clientRect.bottom -= 5;
     if ( mcList->mc_MarkToday == TRUE )
     {
          // Today
          SYSTEMTIME st;
          GetLocalTime(&st);//to be fix when set time before
          TCHAR strToday[100];
          wsprintf( strToday, L"%s %d.%d.%d", g_Today, st.wDay, st.wMonth, st.wYear );
          //SetTextAlign( hDC, TA_LEFT );
          SetTextColor( hDC, MC_MONTH_TXTCOLOR );
          SelectObject( hDC, mcList->mc_hHeaderFont );
		  DrawText(hDC,strToday, -1, &clientRect, DT_LEFT | DT_BOTTOM);
          //TextOut( hDC, clientRect.left+22, clientRect.bottom-18, strToday, strlen(strToday) );
          // Today marker
          HDC hMemDC = CreateCompatibleDC(hDC);
          HBITMAP hOldBitmap = (HBITMAP)SelectObject( hMemDC, g_Marker );
          TransBlt( hDC, clientRect.left+5, clientRect.bottom-18, 15, 15, hMemDC, 0, 0, RGB(0,0,0) );
          SelectObject( hMemDC, hOldBitmap );
          DeleteDC(hMemDC);
     }

    // if ( mcList->mc_MarkEaster == TRUE )
    // {     
    //      // Easter marker
    //      TCHAR strEaster[100];
    //      wsprintf( strEaster, L"%s", g_Easter );
    //      //SetTextAlign( hDC, TA_RIGHT );
    //      SetTextColor( hDC, MC_EASTER_TXTCOLOR );
		  //DrawText(hDC,strEaster, -1, &clientRect, DT_RIGHT | DT_BOTTOM);
    //      //TextOut( hDC, clientRect.right-5, clientRect.bottom-18, strEaster, strlen(strEaster) );
    // }
     
     //SetTextAlign( hDC, TA_LEFT );
     SetTextColor( hDC, oldTextColor );
     SetBkMode( hDC, OPAQUE );
     SelectObject( hDC, hOldPen );
     SelectObject( hDC, hOldFont );
     SelectObject( hDC, hOldBrush );
     
     DeleteObject(hWeekDayPen);
}


void DrawBody(HWND hWnd)
{
     // Find MyCalendar
     MC_LIST* mcList = GetMCCal(hWnd);

     // Get MyCalendar DC
     HDC hDC = mcList->mc_hMemDC;
     HFONT hOldFont = (HFONT)SelectObject( hDC, mcList->mc_hBodyFont );
     COLORREF oldTextColor = GetTextColor(hDC);
     
     // Get MyCalendar window rect
     RECT clientRect;
     GetClientRect( mcList->mc_hWnd, &clientRect );
     
     // Draw date's table
     TCHAR buffer[10];
     RECT rect = {clientRect.left+9, clientRect.top+66, clientRect.left+36, clientRect.top+86};
     for ( int i=1; i<=42; i++ )
     {
         // If date is selected
         if ( mcList->mc_Dates[i-1].mcState == SELECTED )
         {
              DrawEdge( hDC, &rect, EDGE_SUNKEN, BF_RECT | BF_MIDDLE );
              SetTextColor( hDC, MC_DATE_TXTCOLOR );
         }
         else
         {
              DrawEdge( hDC, &rect, EDGE_ETCHED, BF_RECT | BF_MIDDLE );
              SetTextColor( hDC, MC_MONTH_TXTCOLOR );
         }
         
         // If date is on Sunday
         if ( ( mcList->mc_MarkSunday == TRUE ) && ( i % 7 == 1 ) )
         {
              if ( mcList->mc_Dates[i-1].mcState == UNSELECTED )
                   SetTextColor( hDC, MC_SUNDAY_COLOR );
              else if ( mcList->mc_Dates[i-1].mcState == SELECTED )
                   SetTextColor( hDC, RGB(200,120,120) );
         }
         
         // Draw date value
         SetTextAlign( hDC, TA_CENTER );
         SetBkMode( hDC, TRANSPARENT );
         if ( mcList->mc_Dates[i-1].mcState != EMPTY )
         {
              // If date is Easter date
              if ( ( mcList->mc_MarkEaster == TRUE ) && ( mcList->mc_EasterMonth == mcList->mc_Month ) && ( mcList->mc_EasterDay == mcList->mc_Dates[i-1].mcDay ) )
              {
                   wsprintf( buffer, L"%s", g_EasterMark );
				   DrawText(hDC,buffer, -1, &rect, DT_RIGHT | DT_VCENTER);
                   //TextOut( hDC, (rect.left+rect.right)/2, rect.top+3, buffer, strlen(buffer) );
              }
              else
              {
                   _itow( mcList->mc_Dates[i-1].mcDay, buffer, 10 );
				   DrawText(hDC,buffer, -1, &rect, DT_RIGHT | DT_VCENTER);
                   //TextOut( hDC, (rect.left+rect.right)/2, rect.top+3, buffer, strlen(buffer) );
              }
         }
         SetBkMode( hDC, OPAQUE );

         if ( mcList->mc_MarkToday == TRUE )
         {
              // Draw today marker 
              if ( ( mcList->mc_Dates[i-1].mcToday == true ) && ( mcList->mc_Dates[i-1].mcState != EMPTY ) )
              {
                   HDC hMemDC = CreateCompatibleDC(hDC);
                   HBITMAP hOldBitmap = (HBITMAP)SelectObject( hMemDC, g_Marker );
                   TransBlt( hDC, rect.left+5, rect.top+4, 15, 15, hMemDC, 0, 0, RGB(0,0,0) );
                   SelectObject( hMemDC, hOldBitmap );
                   DeleteDC(hMemDC);
              }
         }
         
         if ( i % 7 == 0 )
         {
              rect.left = clientRect.left + 9;
              rect.right = clientRect.left + 36;
              rect.top = rect.bottom - 1;
              rect.bottom += 20;
         }
         else
         {
              rect.left = rect.right - 1;
              rect.right += 26;
         }
     }
    
     SetTextColor( hDC, oldTextColor );
     SetTextAlign( hDC, TA_LEFT ); 
     SelectObject( hDC, hOldFont );
}


void TransBlt(HDC dstDC, int dstX, int dstY, int width, int height, HDC srcDC, int srcX, int srcY, COLORREF transColor)
{
     COLORREF pixel;
     int sX, sY;
     
     sX = srcX;
     for ( int x=dstX; x<dstX+width; x++ )
     {
         sY = srcY;
         for ( int y=dstY; y<dstY+height; y++ )
         {
             pixel = GetPixel( srcDC, sX, sY );
             if ( pixel != transColor )
                  SetPixel( dstDC, x, y, pixel );
             sY++;
         }
         sX++;
     }
}


void CheckCellClick(HWND hWnd, POINT pt)
{
     int selected = -1;
     bool found = false;
     
     // Find MyCalendar
     MC_LIST* mcList = GetMCCal(hWnd);

     // Get MyCalendar DC
     HDC hDC = mcList->mc_hMemDC;
     
     // Get MyCalendar window rect
     RECT clientRect;
     GetClientRect( mcList->mc_hWnd, &clientRect );
     
     // Clear date's table
     RECT rect = {clientRect.left+8, clientRect.top+65, clientRect.left+35, clientRect.top+85};
     for ( int i=1; i<=42; i++ )
     {
         if ( mcList->mc_Dates[i-1].mcState != EMPTY )
         {
              if ( mcList->mc_Dates[i-1].mcState == SELECTED )
                   selected = i-1;
              mcList->mc_Dates[i-1].mcState = UNSELECTED;
         }
     }
     
     // Check date's table
     for (int i=1; i<=42; i++ )
     {
         if ( mcList->mc_Dates[i-1].mcState != EMPTY )
         {
              // Check if cell is selected
              if ( ( PtInRect( &rect, pt ) ) && ( mcList->mc_Dates[i-1].mcDay != 0 ) )
              {
                   found = true;
                   mcList->mc_Dates[i-1].mcState = SELECTED;
                   mcList->mc_Day = mcList->mc_Dates[i-1].mcDay;
                   
                   // Set date window text
                   FillDates(mcList->mc_hWnd);
                   
                   // Send notification message
                   SendMessage( mcList->mc_hParent, WM_COMMAND, MAKEWPARAM(0,MC_DATECHANGED), (LPARAM)mcList->mc_hWnd );
              }
             
              if ( i % 7 == 0 )
              {
                   rect.left = clientRect.left + 8;
                   rect.right = clientRect.left + 35;
                   rect.top = rect.bottom - 1;
                   rect.bottom += 20;
              }
              else
              {
                   rect.left = rect.right - 1;
                   rect.right += 26;
              }
         }
         else
         {
              rect.left = rect.right - 1;
              rect.right += 26;
         }
     }
     
     if ( ( !found ) && ( selected != -1 ) )
     {
          mcList->mc_Dates[selected].mcState = SELECTED;
          mcList->mc_Day = mcList->mc_Dates[selected].mcDay;
     }
     
     if ( mcList->mc_MarkToday == TRUE )
     {
          // If today is selected
          RECT todayRect = {clientRect.left+5, clientRect.bottom-18, clientRect.left+20, clientRect.bottom};
          if ( PtInRect( &todayRect, pt ) )
          {
               // Get current local time
               SYSTEMTIME st;
               GetLocalTime(&st);
          
               // Set today's date
               mcList->mc_Year = st.wYear;
               mcList->mc_Month = st.wMonth;
               mcList->mc_Day = st.wDay;
               FillDates(mcList->mc_hWnd);
          
               // Send notification message
               SendMessage( mcList->mc_hParent, WM_COMMAND, MAKEWPARAM(0,MC_DATECHANGED), (LPARAM)mcList->mc_hWnd );
          }
     }
     
     // If year is selected
     RECT yearRect = {clientRect.right-50, 24, clientRect.right-5, 44};
     if ( PtInRect( &yearRect, pt ) )
     {
          mcList->mc_Edit = TRUE;
          ShowWindow( mcList->mc_hYearWnd, SW_SHOW );
          SendMessage( mcList->mc_hYearWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)"" );
          SetFocus(mcList->mc_hYearWnd);
     }
     else
     {
          mcList->mc_Edit = FALSE;
          ShowWindow( mcList->mc_hYearWnd, SW_HIDE );
     }
     
     if ( mcList->mc_MarkEaster == TRUE )
     {
         // If Easter is selected
         RECT easterRect = {clientRect.right-45, clientRect.bottom-18, clientRect.right-5, clientRect.bottom};
         if ( PtInRect( &easterRect, pt ) )
         {
              mcList->mc_Month = mcList->mc_EasterMonth;
              mcList->mc_Day = mcList->mc_EasterDay;
              FillDates(mcList->mc_hWnd);
              
              // Send notification message
              SendMessage( mcList->mc_hParent, WM_COMMAND, MAKEWPARAM(0,MC_DATECHANGED), (LPARAM)mcList->mc_hWnd );
         }
     }
}


int CalculateDayOfWeek(int day, int month, int year, BOOL bGregorian)
{
    int result = -1;
    
    // Calculate params
    int a = (int)((14-month) / 12);
    int y = year - a;
    int m = month + 12*a - 2;
    
    // Calculate day of the week
    if ( bGregorian == TRUE )
         result = (int)((day + y + y/4 - y/100 + y/400 + (31*m)/12) % 7);
    else
         result = (int)((5 + day + y + y/4 + (31*m)/12) % 7);
    
    return result;
}


bool IsLeapYear(int year, BOOL bGregorian)
{
     bool result = false;
     
     // Is year a leap year
     if ( bGregorian == TRUE )
     {
          if ( ( year % 400 == 0 ) || ( ( year % 4 == 0 ) && ( year % 100 != 0 ) ) )
               result = true;
     }
     else
     {
          if ( year % 4 == 0 )
               result = true;
     }
     
     return result;
}


int GetNumberOfDays(int month, int year)
{
    int result;
    
    // Calculate number of days in the month
    if ( month != 2 )
    {
         // In the first half of the year
         if ( month <= 7 )
         {
              if ( month % 2 == 1 )
                   result = 31;
              else
                   result = 30;
         }
         // In the second half of the year
         else
         {
              if ( month % 2 == 0 )
                   result = 31;
              else
                   result = 30;
         }
    }
    else
    {
        // If year is leap year
        if ( IsLeapYear(year, TRUE) )
             result = 29;
        else
             result = 28;
    }
    
    return result;
}


void GetEasterDate(int year, int*month, int* day, BOOL bGregorian)
{
     // Calculate Easter date in the current year
     int GoldenNumber = (year % 19) + 1;
     int GregorianEpact;
     if ( bGregorian == TRUE )
     {
         int JulianEpact = (11*(GoldenNumber-1)) % 30;
         int century = (year/100) + 1;
         int S = (3*century) / 4;
         int L = (8*century + 5) / 25;
         GregorianEpact = JulianEpact - S + L + 8;
         if ( GregorianEpact > 30 )
         {
              while ( GregorianEpact > 30 )
                   GregorianEpact -= 30;
         }
         else if ( GregorianEpact < 1 )
         {
              while ( GregorianEpact < 1 )
                   GregorianEpact += 30;
         }
     }
     GetPaschalFullMoon( GregorianEpact, GoldenNumber, month, day, bGregorian );
     int dayOfWeek = CalculateDayOfWeek( *day, *month, year, bGregorian );
     int dayDifference = 7 - dayOfWeek;
     
     // Calculate Julian day number from Paschal full mooon date
     int a = (14-(*month))/12;
     int y = year + 4800 - a;
     int m = (*month) + 12*a - 3;
     int JulianDayNumber;
     if ( bGregorian == TRUE )
          JulianDayNumber = (*day) + (153*m+2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;
     else
          JulianDayNumber = (*day) + (153*m+2)/5 + 365*y + y/4 - 32083;
     // Increment Julian day number
     JulianDayNumber += dayDifference;
     // Calculate date from Julian date number
     int b, c;
     if ( bGregorian == TRUE )
     {
          a = JulianDayNumber + 32044;
          b = (4*a+3)/146097;
          c = a - (146097*b)/4;
     }
     else
     {
         b = 0;
         c = JulianDayNumber + 32082;
     }
     int d = (4*c+3)/1461;
     int e = c - (1461*d)/4;
     m = (5*e+2)/153;
     
     if ( bGregorian == TRUE )
     {
         // Calculate Gregorian Easter date according Gregorian calendar
         *day = e - (153*m+2)/5 + 1;
         *month = m + 3 - 12*(m/10);
     }
     else
     {
         // Calculate Julian date from Julian day number
         *day = e - (153*m+2)/5 + 1;
         *month = m + 3 - 12*(m/10);
         
         // Diference between Gregorian and Julian calendar
		 int extra = 10;
		 if ( year > 1600 )
         {
              int tmp = year/100 - 16;
			  extra = extra + tmp - tmp/4;
		}
		*day += extra;

        // Calculate Julian Easter date according Gregorian calendar
		if ( ( *month == 3 ) && ( *day > 31 ) )
        {
             *month = 4;
			 *day = *day - 31;
		}

		if ( ( *month == 4 ) && ( *day > 30 ) )
        {
			*month = 5;
			*day = *day - 30;
		}
     }
}


void GetPaschalFullMoon(int epact, int golden_number, int* month, int* day, BOOL bGregorian)
{
     // For Gregorian calendar
     if ( bGregorian == TRUE )
     {
         // Calculate Paschal full moon date
         switch( epact )
         {
              case 1:
              {
                   *month = 4;
                   *day = 12;
              }
              break;
              case 2:
              {
                   *month = 4;
                   *day = 11;
              }
              break;
              case 3:
              {
                   *month = 4;
                   *day = 10;
              }
              break;
              case 4:
              {
                   *month = 4;
                   *day = 9;
              }
              break;
              case 5:
              {
                   *month = 4;
                   *day = 8;
              }
              break;
              case 6:
              {
                   *month = 4;
                   *day = 7;
              }
              break;
              case 7:
              {
                   *month = 4;
                   *day = 6;
              }
              break;
              case 8:
              {
                   *month = 4;
                   *day = 5;
              }
              break;
              case 9:
              {
                   *month = 4;
                   *day = 4;
              }
              break;
              case 10:
              {
                   *month = 4;
                   *day = 3;
              }
              break;
              case 11:
              {
                   *month = 4;
                   *day = 2;
              }
              break;
              case 12:
              {
                   *month = 4;
                   *day = 1;
              }
              break;
              case 13:
              {
                   *month = 3;
                   *day = 31;
              }
              break;
              case 14:
              {
                   *month = 3;
                   *day = 30;
              }
              break;
              case 15:
              {
                   *month = 3;
                   *day = 29;
              }
              break;
              case 16:
              {
                   *month = 3;
                   *day = 28;
              }
              break;
              case 17:
              {
                   *month = 3;
                   *day = 27;
              }
              break;
              case 18:
              {
                   *month = 3;
                   *day = 26;
              }
              break;
              case 19:
              {
                   *month = 3;
                   *day = 25;
              }
              break;
              case 20:
              {
                   *month = 3;
                   *day = 24;
              }
              break;
              case 21:
              {
                   *month = 3;
                   *day = 23;
              }
              break;
              case 22:
              {
                   *month = 3;
                   *day = 22;
              }
              break;
              case 23:
              {
                   *month = 3;
                   *day = 21;
              }
              break;
              case 24:
              {
                   *month = 4;
                   *day = 18;
              }
              break;
              case 25:
              {
                   *month = 4;
                   if ( golden_number > 11 )
                        *day = 17;
                   else
                        *day = 18;
              }
              break;
              case 26:
              {
                   *month = 4;
                   *day = 17;
              }
              break;
              case 27:
              {
                   *month = 4;
                   *day = 16;
              }
              break;
              case 28:
              {
                   *month = 4;
                   *day = 15;
              }
              break;
              case 29:
              {
                   *month = 4;
                   *day = 14;
              }
              break;
              case 30:
              {
                   *month = 4;
                   *day = 13;
              }
              break;
         }
     }
     // For Julian calendar
     else
     {
         // Calculate Paschal full moon date
         switch( golden_number )
         {
              case 1:
              {
                   *month = 4;
                   *day = 5;
              }
              break;
              case 2:
              {
                   *month = 3;
                   *day = 25;
              }
              break;
              case 3:
              {
                   *month = 4;
                   *day = 13;
              }
              break;
              case 4:
              {
                   *month = 4;
                   *day = 2;
              }
              break;
              case 5:
              {
                   *month = 3;
                   *day = 22;
              }
              break;
              case 6:
              {
                   *month = 4;
                   *day = 10;
              }
              break;
              case 7:
              {
                   *month = 3;
                   *day = 30;
              }
              break;
              case 8:
              {
                   *month = 4;
                   *day = 18;
              }
              break;
              case 9:
              {
                   *month = 4;
                   *day = 7;
              }
              break;
              case 10:
              {
                   *month = 3;
                   *day = 27;
              }
              break;
              case 11:
              {
                   *month = 4;
                   *day = 15;
              }
              break;
              case 12:
              {
                   *month = 4;
                   *day = 4;
              }
              break;
              case 13:
              {
                   *month = 3;
                   *day = 24;
              }
              break;
              case 14:
              {
                   *month = 4;
                   *day = 12;
              }
              break;
              case 15:
              {
                   *month = 4;
                   *day = 1;
              }
              break;
              case 16:
              {
                   *month = 3;
                   *day = 21;
              }
              break;
              case 17:
              {
                   *month = 4;
                   *day = 9;
              }
              break;
              case 18:
              {
                   *month = 3;
                   *day = 29;
              }
              break;
              case 19:
              {
                   *month = 4;
                   *day = 17;
              }
              break;
         }
     }
}


void FillDates(HWND hWnd)
{
     MC_LIST* mcList = GetMCCal(hWnd);
     if ( mcList != NULL )
     { 
          // Get current local time
          SYSTEMTIME st;
          GetLocalTime(&st);
          
          // Calculate day of the week
          int dayOfWeek = CalculateDayOfWeek( 1, mcList->mc_Month, mcList->mc_Year, true );
          // Check if current year is leap year
          bool isLeapYear = IsLeapYear( mcList->mc_Year, true );
          // Get number of days of the current month
          int numberOfDays = GetNumberOfDays( mcList->mc_Month, mcList->mc_Year );
          // Calculate Easter date for the current year
          GetEasterDate( mcList->mc_Year, &mcList->mc_EasterMonth, &mcList->mc_EasterDay, mcList->mc_GregorianEaster );
          
          int day = 0;
          for ( int i=0; i<42; i++ )
          {
               // Check starting day of the month
               if ( ( i < dayOfWeek ) || ( day >= numberOfDays ) )
                    mcList->mc_Dates[i].mcState = EMPTY;
               else
               {
                    day++;
                    mcList->mc_Dates[i].mcDay = day;
                    if ( mcList->mc_Day != day )
                         mcList->mc_Dates[i].mcState = UNSELECTED;
                    else
                         mcList->mc_Dates[i].mcState = SELECTED;
               }
               
               // If current day is today
               if ( ( day == st.wDay ) && ( mcList->mc_Month == st.wMonth ) && ( mcList->mc_Year == st.wYear ) )
                    mcList->mc_Dates[i].mcToday = true;
               else
                    mcList->mc_Dates[i].mcToday = false;
          }
          
          // Set date window text
          SetDateWndText( mcList->mc_hWnd, mcList->mc_Day, mcList->mc_Month, mcList->mc_Year );
     }
}


void CMyCalendar::SetDate(int day, int month, int year)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Set custom date
          mcList->mc_Year = year;
          mcList->mc_Month = month;
          mcList->mc_Day = day;
          SetDateWndText( mcList->mc_hWnd, mcList->mc_Day, mcList->mc_Month, mcList->mc_Year );
     }
}


void SetDateWndText(HWND hWnd, int day, int month, int year)
{
     MC_LIST* mcList = GetMCCal(hWnd);
     if ( mcList != NULL )
     {
          // Set date window text
          TCHAR date[20];
          if ( ( mcList->mc_Day >= 10 ) && ( mcList->mc_Month >= 10 ) )
               wsprintf( date, L"     %d / %d / %d", mcList->mc_Day, mcList->mc_Month, mcList->mc_Year );
          else if ( ( mcList->mc_Day >= 10 ) && ( mcList->mc_Month < 10 ) )
               wsprintf( date, L"     %d / 0%d / %d", mcList->mc_Day, mcList->mc_Month, mcList->mc_Year );     
          else if ( ( mcList->mc_Day < 10 ) && ( mcList->mc_Month >= 10 ) )
               wsprintf( date, L"     0%d / %d / %d", mcList->mc_Day, mcList->mc_Month, mcList->mc_Year );
          else if ( ( mcList->mc_Day < 10 ) && ( mcList->mc_Month < 10 ) )
               wsprintf( date, L"     0%d / 0%d / %d", mcList->mc_Day, mcList->mc_Month, mcList->mc_Year );
          SetWindowText( mcList->mc_hDateWnd, date );

          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


void CMyCalendar::GetDate(int* day, int* month, int* year)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Get selected date
          *year = mcList->mc_Year;
          *month = mcList->mc_Month;
          *day = mcList->mc_Day;
     }
}


HWND CMyCalendar::GetWindowHandle()
{
     // Return MyCalendar window handle
     return m_hWnd;
}


void CMyCalendar::Expand(BOOL expand)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Set MyCalendar expand flag
          mcList->mc_Expand = !expand;
          SendMessage( mcList->mc_hWnd, WM_COMMAND, 0, (LPARAM)mcList->mc_hCalWnd );
     }
}


void CMyCalendar::SetRange(int startYear, int endYear)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Check year range
          if ( startYear > endYear )
          {
               int tmpYear = startYear;
               startYear = endYear;
               endYear = tmpYear;
          }
          
          // Set MyCalendar date range
          if ( startYear >= 1582 )
               mcList->mc_StartYear = startYear;
          else
               mcList->mc_StartYear = 1582;
          if ( endYear <= 4099 )
               mcList->mc_EndYear = endYear;
          else
               mcList->mc_EndYear = 4099;
          
          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


void CMyCalendar::GetRange(int* startYear, int* endYear)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Get MyCalendar date range
          *startYear = mcList->mc_StartYear;
          *endYear = mcList->mc_EndYear;
     }
}


COLORREF CMyCalendar::GetBgrColor()
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Get MyCalendar background color
          LOGBRUSH lb;
          GetObject( mcList->mc_hBkBrush, sizeof(LOGBRUSH), &lb );
          return lb.lbColor;
     }
}


void CMyCalendar::SetBgrColor(COLORREF bgColor)
{
     // Set new MyCalendar background color
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          if ( mcList->mc_hBkBrush )
               DeleteObject(mcList->mc_hBkBrush);
          
          mcList->mc_hBkBrush = CreateSolidBrush(bgColor);
          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


void CMyCalendar::MarkSunday(BOOL markSunday)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Set sunday mark flag
          mcList->mc_MarkSunday = markSunday;
          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


void CMyCalendar::MarkToday(BOOL markToday)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Set today mark flag
          mcList->mc_MarkToday = markToday;
          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


void CMyCalendar::MarkEaster(BOOL markEaster)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Set Easter mark flag
          mcList->mc_MarkEaster = markEaster;
          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


void CMyCalendar::SetEasterDate(BOOL bGregorian)
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Set Easter date calculation (TRUE - Gregorian, FALSE - Julian )
          mcList->mc_GregorianEaster = bGregorian;
          FillDates(mcList->mc_hWnd);
          InvalidateRect( mcList->mc_hWnd, NULL, TRUE );
          UpdateWindow(mcList->mc_hWnd);
     }
}


BOOL CMyCalendar::GetEasterDate()
{
     MC_LIST* mcList = GetMCCal(m_hWnd);
     if ( mcList != NULL )
     {
          // Get Easter date calculation (TRUE - Gregorian, FALSE - Julian )
          return mcList->mc_GregorianEaster;
     }
}
