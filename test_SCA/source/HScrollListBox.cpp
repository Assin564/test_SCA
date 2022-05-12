/////////////////////////////////////////////////////////////////////////////
// HScrollListBox.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "framework.h"
#include "HScrollListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHScrollListBox, CListBox)
	//{{AFX_MSG_MAP(CHScrollListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(LB_ADDSTRING, OnAddString)
	ON_MESSAGE(LB_INSERTSTRING, OnInsertString)
	ON_MESSAGE(LB_DELETESTRING, OnDeleteString)
	ON_MESSAGE(LB_DIR, OnDir)
	ON_MESSAGE(LB_RESETCONTENT, OnResetContent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHScrollListBox
/////////////////////////////////////////////////////////////////////////////
CHScrollListBox::CHScrollListBox()
{
}

/////////////////////////////////////////////////////////////////////////////
CHScrollListBox::~CHScrollListBox()
{
}

/////////////////////////////////////////////////////////////////////////////
void CHScrollListBox::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();

#ifdef _DEBUG
	// NOTE: this list box is designed to work as a single column, system-drawn 
	//		 list box. The asserts below will ensure of that.
	DWORD dwStyle = GetStyle();
	ASSERT((dwStyle & LBS_MULTICOLUMN) == 0);
	ASSERT((dwStyle & LBS_OWNERDRAWFIXED) == 0);
	ASSERT((dwStyle & LBS_OWNERDRAWVARIABLE) == 0);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CHScrollListBox message handlers
///////////////////////////////////////////////////////////////////////////////
int CHScrollListBox::GetTextLen(LPCTSTR lpszText)
{
	ASSERT(AfxIsValidString(lpszText));

	CDC *pDC = GetDC();
	ASSERT(pDC);

	CSize size;
	CFont* pOldFont = pDC->SelectObject(GetFont());
	if ((GetStyle() & LBS_USETABSTOPS) == 0)
	{
		size = pDC->GetTextExtent(lpszText, (int) _tcslen(lpszText));
		size.cx += 3;
	}
	else
	{
		// Expand tabs as well
		size = pDC->GetTabbedTextExtent(lpszText, (int) _tcslen(lpszText), 0, NULL);
		size.cx += 2;
	}
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	return size.cx;
}

///////////////////////////////////////////////////////////////////////////////
void CHScrollListBox::ResetHExtent()
{
	if (GetCount() == 0)
	{
		SetHorizontalExtent(0);
		return;
	}

	CWaitCursor cwc;
	int iMaxHExtent = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		CString csText;
		GetText(i, csText);
		int iExt = GetTextLen(csText);
		if (iExt > iMaxHExtent)
			iMaxHExtent = iExt;
	}
	SetHorizontalExtent(iMaxHExtent);
}

///////////////////////////////////////////////////////////////////////////////
void CHScrollListBox::SetNewHExtent(LPCTSTR lpszNewString)
{
	int iExt = GetTextLen(lpszNewString);
	if (iExt > GetHorizontalExtent())
		SetHorizontalExtent(iExt);
}

///////////////////////////////////////////////////////////////////////////////
// OnAddString: wParam - none, lParam - string, returns - int
///////////////////////////////////////////////////////////////////////////////
LRESULT CHScrollListBox::OnAddString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (!((lResult == LB_ERR) || (lResult == LB_ERRSPACE)))
		SetNewHExtent((LPCTSTR) lParam);
	return lResult;
}

///////////////////////////////////////////////////////////////////////////////
// OnInsertString: wParam - index, lParam - string, returns - int 
///////////////////////////////////////////////////////////////////////////////
LRESULT CHScrollListBox::OnInsertString(WPARAM wParam, LPARAM lParam)
{

	LRESULT lResult = Default();

	if (!((lResult == LB_ERR) || (lResult == LB_ERRSPACE)))
		SetNewHExtent((LPCTSTR) lParam);

//Black Add for auto scroll
	
	SetCurSel(GetCount()-1);


	return lResult;
}

///////////////////////////////////////////////////////////////////////////////
// OnDeleteString: wParam - index, lParam - none, returns - int 
///////////////////////////////////////////////////////////////////////////////
LRESULT CHScrollListBox::OnDeleteString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (!((lResult == LB_ERR) || (lResult == LB_ERRSPACE)))
		ResetHExtent();
	return lResult;
}

///////////////////////////////////////////////////////////////////////////////
// OnDir: wParam - attr, lParam - wildcard, returns - int 
///////////////////////////////////////////////////////////////////////////////
LRESULT CHScrollListBox::OnDir(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (!((lResult == LB_ERR) || (lResult == LB_ERRSPACE)))
		ResetHExtent();
	return lResult;
}

///////////////////////////////////////////////////////////////////////////////
// OnResetContent: wParam - none, lParam - none, returns - int 
///////////////////////////////////////////////////////////////////////////////
LRESULT CHScrollListBox::OnResetContent(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	SetHorizontalExtent(0);
	return lResult;
}

void CHScrollListBox::DrawItem(LPDRAWITEMSTRUCT lpdis) 
{
    if (lpdis->itemID < 0)
        return; 

    COLORREF cvText;
    COLORREF cvBack;
    CString itemString;

    if ((lpdis->itemState & ODS_SELECTED) &&    // if item has been selected
        (lpdis->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
            DrawFocusRect(lpdis->hDC, &lpdis->rcItem); 
    

    if (!(lpdis->itemState & ODS_SELECTED) &&    // if item has been deselected
        (lpdis->itemAction & ODA_SELECT))
            DrawFocusRect(lpdis->hDC, &lpdis->rcItem); 
    

    if(lpdis->itemData)        // if color information is present
            cvText = SetTextColor(lpdis->hDC, lpdis->itemData);
        else     // if no color information, use default system colors
            cvText = SetTextColor(lpdis->hDC, GetSysColor((lpdis->itemState & ODS_SELECTED)
        ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT)); 
    
    // always use system colors for background
    cvBack = SetBkColor(lpdis->hDC, GetSysColor((lpdis->itemState & ODS_SELECTED)
        ? COLOR_HIGHLIGHT : COLOR_WINDOW)); 

    // get and display item text
    GetText(lpdis->itemID, itemString );
    DrawText(lpdis->hDC, itemString, -1, &lpdis->rcItem, DT_LEFT | DT_SINGLELINE);

    // restore DC colors
    SetTextColor(lpdis->hDC, cvText); 
    SetBkColor(lpdis->hDC, cvBack); 
}

int CHScrollListBox::InsertStringColor( int nIndex, LPCTSTR lpszItem, COLORREF rgb)
{
    int item = ((CListBox*)this)->InsertString(nIndex,lpszItem);
    if(item >=0)
        SetItemData(item,rgb);
    return item;

}
