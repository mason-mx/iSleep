#ifndef CDCBUFFER_H
#define CDCBUFFER_H

class CDCBuffer
{
public:
	CDCBuffer()
	{
		m_bValid = FALSE;
		m_hOldBitmap = NULL;
		m_hBitmap = NULL;
		m_nBitmapID = 0;
		m_sizeBitmap.cx = m_sizeBitmap.cy = 0;
	}

	~CDCBuffer()
	{
		DeleteObject();
	}

	void DeleteObject()
	{
		if (m_hOldBitmap)
			SelectObject(m_dcMemory, m_hOldBitmap);
		m_hOldBitmap = NULL;

		if (m_hBitmap)
			::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;

		m_bValid = FALSE;
	}

	BOOL LoadBitmap(UINT nBitmapID, HDC pDC)
	{
		BOOL ok = FALSE;

		DeleteObject();

		ASSERT(pDC);
		ASSERT(nBitmapID);

		m_hBitmap = (HBITMAP)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(nBitmapID),
							IMAGE_BITMAP, 0, 0, 0);
		ASSERT(m_hBitmap);

		if (m_hBitmap && pDC)
		{
			DeleteDC(m_dcMemory);
			m_dcMemory = CreateCompatibleDC(pDC);
			if (m_dcMemory)
			{
				m_hOldBitmap = (HBITMAP)::SelectObject(m_dcMemory, m_hBitmap);
				m_nBitmapID = nBitmapID;
				m_bValid = TRUE;

				m_sizeBitmap.cx = m_sizeBitmap.cy = 0;

				BITMAP bm;
				if (GetBitmap(&bm))
				{
					m_sizeBitmap.cx = bm.bmWidth;
					m_sizeBitmap.cy = bm.bmHeight;
				}				

				ok = TRUE;
			}
		}

		return ok;
	}

	BOOL AttachBitmap(HBITMAP hBitmap, HDC pDC)
	{
		BOOL ok = FALSE;

		DeleteObject();

		ASSERT(hBitmap);

		m_hBitmap = hBitmap;

		if (m_hBitmap && pDC)
		{
			DeleteDC(m_dcMemory);

			m_dcMemory = CreateCompatibleDC(pDC);
			if (m_dcMemory)
			{
				m_hOldBitmap = (HBITMAP)::SelectObject(m_dcMemory, m_hBitmap);
				m_nBitmapID = 0;
				m_bValid = TRUE;
				ok = TRUE;
			}
		}

		return ok;
	}

	BOOL GetBitmap(BITMAP * pBM)
	{
		BOOL ok = FALSE;
		if (m_bValid && m_hBitmap)
		{
			ok = ::GetObject(m_hBitmap, sizeof(BITMAP), pBM);
		}
		return ok;
	}

	BOOL IsValid()
	{
		return m_bValid;
	}

	HDC			m_dcMemory;
	HBITMAP		m_hBitmap;
	SIZE		m_sizeBitmap;

protected:
	BOOL		m_bValid;
	HBITMAP		m_hOldBitmap;
	UINT		m_nBitmapID;
};

#endif //CDCBUFFER_H
