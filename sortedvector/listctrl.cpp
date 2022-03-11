
#include "pch.h"
#include "listctrl.h"
#include "resource.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(listctrl, CListCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BOOL listctrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// mutate style, report view, owner draw, ...
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT|LVS_OWNERDRAWFIXED;
	cs.dwExStyle |= LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;

	// call base class
	return CListCtrl::PreCreateWindow(cs);
}

int listctrl::OnCreate(LPCREATESTRUCT lpCIS)
{
	// call base class
	const int n = CListCtrl::OnCreate(lpCIS);

	// setup hdr
	setuphdr();

	return n;
}

void listctrl::PreSubclassWindow()
{
	// call base class
	CListCtrl::PreSubclassWindow();

	// setup hdr
	setuphdr();
}

void listctrl::OnSetFocus(CWnd *p)
{
	InvalidateRect(nullptr);
}

void listctrl::setuphdr( void )
{
	if( GetHeaderCtrl()->GetItemCount() > 0 )
		return;

	CString csIndex;csIndex.LoadString(IDS_INDEX);
	CString csFirst;csFirst.LoadString(IDS_FIRST);
	CString csSecond;csSecond.LoadString(IDS_SECOND);
	CRect rc;
	GetWindowRect(rc);
	::MapWindowPoints(NULL,m_hWnd,(LPPOINT)&rc,2);
	rc.DeflateRect(2,2);
	InsertColumn(0,csIndex,LVCFMT_LEFT,200);
	InsertColumn(1,csFirst,LVCFMT_LEFT,200);
	InsertColumn(2,csSecond,LVCFMT_LEFT,200);
}

void listctrl::getitemcolumns( const CRect& rcItemRect, std::vector<CRect>& vColumns ) const
{
	// Determine if there is anything to do
	CHeaderCtrl *pHdrCtrl = GetHeaderCtrl();
	vColumns.resize( pHdrCtrl->GetItemCount() );
	auto i = vColumns.begin();
	auto end = vColumns.cend();
	for( ; i != end ; ++i )
	{
		const int nColumn = static_cast<int>(std::distance(vColumns.begin(),i));

		CRect rcRect;
		pHdrCtrl->GetItemRect( nColumn, rcRect );

		rcRect.top = rcItemRect.top;
		rcRect.bottom = rcItemRect.bottom;
		rcRect.OffsetRect( rcItemRect.left, 0 );

		(*i) = rcRect;
	}
}

void listctrl::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// ownerdrawfixed so will not get called, if ownerdrawvvariable will be called for every item
	ASSERT( false );

	// assume the currently selected font is the same used when rendering
	TEXTMETRIC tm; 
	HDC hDC = ::GetDC( m_hWnd );
	CFont* pFont = GetFont();
	HFONT hFontOld = static_cast<HFONT>( SelectObject( hDC, pFont->GetSafeHandle() ) );
	GetTextMetrics( hDC, &tm );
	lpMIS->itemHeight = tm.tmHeight + tm.tmExternalLeading + 1;
	SelectObject( hDC, hFontOld );
	::ReleaseDC( m_hWnd, hDC );
}

void listctrl::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	// get the columns
	std::vector<CRect> vColumns;
	const CRect rcItemRect( lpDIS->rcItem );
	getitemcolumns( rcItemRect, vColumns );
	
	// render background
	CDC *pDC = CDC::FromHandle( lpDIS->hDC );
	CWnd *pFocusWnd = GetFocus();
	const bool bCtrlFocus = ( pFocusWnd && ( pFocusWnd->GetSafeHwnd() == m_hWnd ) );
	const bool bSelected = ( lpDIS->itemState & ODS_SELECTED );
	const COLORREF crBkGnd = ::GetSysColor( COLOR_WINDOW );
	pDC->FillSolidRect( &rcItemRect, ::GetSysColor( COLOR_WINDOW ) );

	// selection
	const CRect rcText = rcItemRect;
	if( bSelected )
	{
		if( bCtrlFocus )
			pDC->FillSolidRect( &rcItemRect, ::GetSysColor( COLOR_HIGHLIGHT ) );
		else
			pDC->FillSolidRect( &rcItemRect, ::GetSysColor( COLOR_BTNFACE ) );
	}
	
	// text
	CFont *pFont = GetFont();
	CFont *pOldFont = pDC->SelectObject( pFont );
	int nOldMode = pDC->SetBkMode( TRANSPARENT );
	COLORREF crOldTextColour = pDC->SetTextColor( ::GetSysColor( ( bSelected && bCtrlFocus ) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT ) );
	auto i = vColumns.cbegin(), end = vColumns.cend();
	for( ; i != end ; ++i )
	{
		const int nColumn = static_cast<int>(std::distance(vColumns.cbegin(),i));
		
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT;
		GetColumn( nColumn, &lvc );

		if( nColumn == 0 )
		{
			const CString csText = GetItemText( lpDIS->itemID, nColumn );
			DWORD dwFlags = DT_VCENTER|DT_SINGLELINE|DT_PATH_ELLIPSIS|DT_END_ELLIPSIS;

			if(lvc.fmt & LVCFMT_CENTER)
				dwFlags |= DT_CENTER;
			else
			if(lvc.fmt & LVCFMT_RIGHT)
				dwFlags |= DT_RIGHT;
			else
				dwFlags |= DT_LEFT;

			CRect rcText = *i;
			pDC->DrawText( csText, &rcText, dwFlags );

			continue;
		}
		
		switch( gettypefromcolumn( nColumn ) )
		{
			case ct_firstname:
			{
				const CString csText = GetItemText( lpDIS->itemID, nColumn );
				DWORD dwFlags = DT_VCENTER|DT_SINGLELINE|DT_PATH_ELLIPSIS|DT_END_ELLIPSIS;

				if(lvc.fmt & LVCFMT_CENTER)
					dwFlags |= DT_CENTER;
				else
				if(lvc.fmt & LVCFMT_RIGHT)
					dwFlags |= DT_RIGHT;
				else
					dwFlags |= DT_LEFT;

				CRect rcText = *i;
				pDC->DrawText( csText, &rcText, dwFlags );
			}
			break;
			case ct_surname:
			{
				const CString csText = GetItemText( lpDIS->itemID, nColumn );
				DWORD dwFlags = DT_VCENTER|DT_SINGLELINE|DT_PATH_ELLIPSIS|DT_END_ELLIPSIS;

				if(lvc.fmt & LVCFMT_CENTER)
					dwFlags |= DT_CENTER;
				else
				if(lvc.fmt & LVCFMT_RIGHT)
					dwFlags |= DT_RIGHT;
				else
					dwFlags |= DT_LEFT;

				CRect rcText = *i;
				pDC->DrawText( csText, &rcText, dwFlags );
			}
			break;
			default:ASSERT(false);break;
		}
	}
	pDC->SetTextColor( crOldTextColour );
	pDC->SetBkMode( nOldMode );
	pDC->SelectObject( pOldFont );
	
    // focus
    if( lpDIS->itemState & ODS_FOCUS )
		pDC->DrawFocusRect( &rcItemRect );
}

void listctrl::setints( const std::shared_ptr<const intpairvector> sp )
{
	m_spInts = sp ? intpairvector::clonesort(sp,m_SortType) : nullptr;
	populateints();
}

void listctrl::setascending( const bool b )
{
	m_bAscending = b;
	populateints();
}

void listctrl::setsort( const intpairvector::E_t s)
{
	m_SortType = s;
	m_spInts = m_spInts ? intpairvector::clonesort(m_spInts,m_SortType) : nullptr;
	populateints();
}

void listctrl::populateints( void )
{
	DeleteAllItems();
	if( !m_spInts )
		return;

	const int nPairs = static_cast<int>(m_spInts->get().size());
	for( int n = 0 ; n < nPairs ; ++n )
	{
		const intpair *p = m_bAscending ? m_spInts->get()[n].get() : m_spInts->get()[nPairs-1-n].get();

		const int nVectorItem = n;
		
		LVITEM lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iItem = nVectorItem;
		lvi.iSubItem = 0;
		CStringW cs;
		cs.Format(L"[%li]",n);
		lvi.pszText = const_cast<wchar_t*>( LPCWSTR(cs) );
		const int nItem = InsertItem( &lvi );
		
		{
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = nItem;
			lvi.iSubItem = 1;
			CStringW cs;
			cs.Format(L"%li",p->first);
			lvi.pszText = const_cast<wchar_t*>( LPCWSTR(cs) );
			const int nItem = SetItem( &lvi );
		}
		{
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = nItem;
			lvi.iSubItem = 2;
			CStringW cs;
			cs.Format(L"%li",p->second);
			lvi.pszText = const_cast<wchar_t*>( LPCWSTR(cs) );
			const int nItem = SetItem( &lvi );
		}
		
		SetItemData( nItem, reinterpret_cast<DWORD_PTR>(p) );
	}
}

int listctrl::getcolumnfromtype( const columntype t ) const
{
	switch( t )
	{
		case ct_firstname:return 0;
		case ct_surname:return 1;
		default:ASSERT( false );return 0;
	}
}

intpairdlg::intpairdlg(CWnd *pParent,const enum intpairvector::E_t s) : CDialogEx(IDD_INTPAIR,pParent)
{
	m_SortType = s;
	m_nFirst = 0;
	m_nSecond = 0;
}

void intpairdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_FIRSTEDIT,m_nFirst);
	DDX_Text(pDX,IDC_SECONDEDIT,m_nSecond);
}

BEGIN_MESSAGE_MAP(intpairdlg, CDialogEx)
END_MESSAGE_MAP()

BOOL intpairdlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: Add extra initialization here
	switch( m_SortType )
	{
		case intpairvector::E_t::primary:break;
		case intpairvector::E_t::firstint:GetDlgItem(IDC_SECONDEDIT)->EnableWindow(false);break;
		case intpairvector::E_t::secondint:GetDlgItem(IDC_FIRSTEDIT)->EnableWindow(false);break;
		default:ASSERT( false );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL intpairdlg::DestroyWindow()
{
	UpdateData();
	return CDialogEx::DestroyWindow();
}
