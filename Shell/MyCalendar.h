//
// Following algorithms were taken from www.tondering.dk/claus/cal/node3.html:
// 1) CalculateDayOfWeek
// 2) GetPaschalFullMoon
// 3) GetEasterDate
//
// Here, author would like to thank to the person who wrote the article.
//


/* MyCalendar includes */
#include "windows.h"
#include "wingdi.h"
#include "stdio.h"


/* MyCalendar definitions */
#define MC_MAXCAL           10
#define MC_MONTH_TXTCOLOR   RGB(255,255,50)
#define MC_WEEK_TXTCOLOR    RGB(120,160,160)
#define MC_DATE_TXTCOLOR    RGB(150,150,150)
#define MC_BK_COLOR         RGB(255,255,255)
#define MC_SUNDAY_COLOR     RGB(200,0,0)
#define MC_EASTER_TXTCOLOR  RGB(0,120,0)


/* MyCalendar messages */
#define MC_DATECHANGED      0x0101
#define MC_EXPANDED         0x0102
#define MC_COLLAPSED        0x0103


/* MyCalendar enums */
enum MC_STATE {SELECTED, UNSELECTED, EMPTY};


/*MyDate definition*/
typedef struct _MC_DATE
{
      int mcDay;
      int mcState;
      bool mcToday;
} MC_DATE;


/* MYCalendar definition */
typedef struct _MC_LIST
{
       HWND mc_hWnd;
       HWND mc_hDateWnd;
       HWND mc_hCalWnd;
       HWND mc_hParent;
       HWND mc_hYearWnd;
       HBITMAP mc_hMemBitmap;
       HDC mc_hMemDC;
       HBITMAP mc_hOldMemBitmap;
       HBRUSH mc_hBkBrush;
       HWND mc_hButtonMonthBack;
       HWND mc_hButtonMonthForward;
       HWND mc_hButtonYearBack;
       HWND mc_hButtonYearForward;
       HFONT mc_hHeaderFont;
       HFONT mc_hWeekFont;
       HFONT mc_hBodyFont;
       HFONT mc_hDateFont;
       int mc_Day;
       int mc_Month;
       int mc_Year;
       MC_DATE mc_Dates[42];
       BOOL mc_Expand;
       BOOL mc_Edit;
       int mc_StartYear;
       int mc_EndYear;
       BOOL mc_MarkSunday;
       BOOL mc_MarkToday;
       BOOL mc_MarkEaster;
       int mc_EasterMonth;
       int mc_EasterDay;
       BOOL mc_GregorianEaster;
       struct _MC_LIST* next;
} MC_LIST;


/* MyCalendar global procedures */
LRESULT CALLBACK MyCalendarProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL AddMCCal(HWND hWnd, HWND hParent, HWND hDate, HWND hCal, HWND hYear);
void DestroyMCCal(HWND hWnd);
MC_LIST* GetMCCal(HWND hWnd);
MC_LIST* DetachMCCal(HWND hWnd);
void DrawHeader(HWND hWnd);
void DrawBody(HWND hWnd);
void InitMCGlobals(HWND hParent, HWND hWnd);
void TransBlt(HDC dstDC, int dstX, int dstY, int width, int height, HDC srcDC, int srcX, int srcY, COLORREF transColor);
void CheckCellClick(HWND hWnd, POINT pt);
void FillDates(HWND hWnd);
void SetDateWndText(HWND hWnd, int day, int month, int year);
bool IsLeapYear(int year, BOOL bGregorian);
int GetNumberOfDays(int month, int year);

/* Algorithms taken from upper reference */
int CalculateDayOfWeek(int day, int month, int year, BOOL bGregorian);
void GetPaschalFullMoon(int epact, int golden_number, int* month, int* day, BOOL bGregorian);
void GetEasterDate(int year, int*month, int* day, BOOL bGregorian);


/* CMyCalendar class definition */
class CMyCalendar
{
private:
     HWND m_hWnd;
     HWND m_hParentWnd;
     HWND m_hDateWnd;
     HWND m_hCalWnd;
     HWND m_hYearWnd;
     
public:
     CMyCalendar();
     virtual ~CMyCalendar();
     BOOL Create(RECT wndRect, HWND hParentWnd);
     HWND GetWindowHandle();
     void SetDate(int day, int month, int year);
     void GetDate(int* day, int* month, int* year);
     void SetRange(int startYear, int endYear);
     void GetRange(int* startYear, int* endYear);
     void SetBgrColor(COLORREF bgColor);
     COLORREF GetBgrColor();
     void Expand(BOOL expand);
     void MarkSunday(BOOL markSunday);
     void MarkToday(BOOL markToday);
     void MarkEaster(BOOL markEaster);
     void SetEasterDate(BOOL bGregorian);
     BOOL GetEasterDate();
};
