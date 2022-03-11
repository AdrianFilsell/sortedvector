
// sortedvectorDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "sortedvector.h"
#include "sortedvectorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CsortedvectorDlg dialog



CsortedvectorDlg::CsortedvectorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SORTEDVECTOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nAscending = BST_CHECKED;
	m_nSort = getindexfromsort(intpairvector::E_t::primary);
}

void CsortedvectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_LISTCTRL,m_ListCtrl);
	DDX_Check(pDX,IDC_ASCENDINGCHECK,m_nAscending);
	DDX_CBIndex(pDX,IDC_SORTCOMBO,m_nSort);
}

BEGIN_MESSAGE_MAP(CsortedvectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_POPULATE,CsortedvectorDlg::OnPopulate)
	ON_BN_CLICKED(IDC_ASCENDINGCHECK,CsortedvectorDlg::OnAscending)
	ON_CBN_SELCHANGE(IDC_SORTCOMBO,CsortedvectorDlg::OnSort)
	ON_BN_CLICKED(IDC_FIND,CsortedvectorDlg::OnFind)
	ON_BN_CLICKED(IDC_DIFF,CsortedvectorDlg::OnDiff)

END_MESSAGE_MAP()


// CsortedvectorDlg message handlers

BOOL CsortedvectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_ListCtrl.setsort(getsortfromindex(m_nSort));
	m_ListCtrl.setascending(m_nAscending == BST_CHECKED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CsortedvectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CsortedvectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CsortedvectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsortedvectorDlg::OnAscending(void)
{
	UpdateData();
	m_ListCtrl.setascending(m_nAscending==BST_CHECKED);
}

void CsortedvectorDlg::OnSort(void)
{
	UpdateData();
	m_ListCtrl.setsort(getsortfromindex(m_nSort));
}

void CsortedvectorDlg::OnPopulate( void )
{
	std::shared_ptr<extstl::intvector> spSeeds{ new extstl::intvector };
	const int nSeeds = 15;
	for( int n = 0 ; n < nSeeds ; ++n )
		spSeeds->push_back( rand() );

	int nSeed;
	double dSeed;
	const int nPairs = 20;
	std::shared_ptr<intpairvector> spInts{ new intpairvector };
	for( int n = 0 ; n < nPairs ; ++n )
	{
		dSeed = ( rand() / static_cast<double>(RAND_MAX) );
		dSeed = dSeed * static_cast<double>(nSeeds-1);
		nSeed = static_cast<int>( dSeed + 0.5 );
		ASSERT( nSeed > -1 && nSeed < nSeeds );

		const int nA = spSeeds->get()[nSeed];
		
		dSeed = ( rand() / static_cast<double>(RAND_MAX) );
		dSeed = dSeed * static_cast<double>(nSeeds-1);
		nSeed = static_cast<int>( dSeed + 0.5 );
		ASSERT( nSeed > -1 && nSeed < nSeeds );

		const int nB = spSeeds->get()[nSeed];
		
		spInts->push_back( intpairvector::T_t( new intpair(std::make_pair(nA,nB)) ) );
	}
	m_ListCtrl.setints(spInts);
}

void CsortedvectorDlg::OnFind( void )
{
	UpdateData();

	const enum intpairvector::E_t s = getsortfromindex(m_nSort);

	intpairdlg dlg(this,s);
	if( dlg.DoModal() != IDOK )
		return;
	
	CString csInfo;
	int nIndex = -1;
	int nInsert = -1;
	bool bFound = false;
	std::shared_ptr<intpair> spFind = std::shared_ptr<intpair>(new intpair(std::make_pair(dlg.m_nFirst,dlg.m_nSecond)));
	switch( s )
	{
		case intpairvector::E_t::primary:
		{
			bFound = intpairvector::sorted().getindex( m_ListCtrl.getints(), spFind, intpairvector::sorted::S_t(), intpairvector::sorted::U_t(), nIndex );	// dont supply a range just search whole vector
			intpairvector::sorted().getfirstnotbeforeindex( m_ListCtrl.getints(), spFind, intpairvector::sorted::S_t(), nInsert );							// dont supply a range just search whole vector

			CString cs;
			if( bFound )
				cs.Format(_T("Found [%li,%li] at [%li]\r\n"),spFind->first,spFind->second,nIndex);
			else
				cs.Format(_T("NOT found [%li,%li]\r\n"),spFind->first,spFind->second);

			csInfo += cs + _T("\r\n");

			cs.Format(_T("Insert [%li,%li] at [%li]"),spFind->first,spFind->second,nInsert);
			csInfo += cs + _T("\r\n");
		}
		break;
		case intpairvector::E_t::firstint:
		{
			bFound = intpairvector::firstintsorted().getindex( m_ListCtrl.getints(), spFind, intpairvector::firstintsorted::S_t(), intpairvector::firstintsorted::U_t(), nIndex );	// dont supply a range just search whole vector
			intpairvector::firstintsorted().getfirstnotbeforeindex( m_ListCtrl.getints(), spFind, intpairvector::firstintsorted::S_t(), nInsert );									// dont supply a range just search whole vector

			CString cs;
			if( bFound )
				cs.Format(_T("Found [%li,X] at [%li]\r\n"),spFind->first,nIndex);
			else
				cs.Format(_T("NOT found [%li,X]\r\n"),spFind->first);

			csInfo += cs + _T("\r\n");

			cs.Format(_T("\r\nInsert [%li,X] at [%li]\r\n"),spFind->first,nInsert);
			csInfo += cs + _T("\r\n");
		}
		break;
		case intpairvector::E_t::secondint:
		{
			bFound = intpairvector::secondintsorted().getindex( m_ListCtrl.getints(), spFind, intpairvector::secondintsorted::S_t(), intpairvector::secondintsorted::U_t(), nIndex );	// dont supply a range just search whole vector
			intpairvector::secondintsorted().getfirstnotbeforeindex( m_ListCtrl.getints(), spFind, intpairvector::secondintsorted::S_t(), nInsert );									// dont supply a range just search whole vector

			CString cs;
			if( bFound )
				cs.Format(_T("Found [X,%li] at [%li]\r\n"),spFind->second,nIndex);
			else
				cs.Format(_T("NOT found [X,%li]\r\n"),spFind->second);

			csInfo += cs + _T("\r\n");

			cs.Format(_T("\r\nInsert [X,%li] at [%li]\r\n"),spFind->second,nInsert);
			csInfo += cs + _T("\r\n");
		}
		break;
		default:ASSERT( false );return;
	}
	AfxMessageBox(csInfo);
}

void CsortedvectorDlg::OnDiff( void )
{
	UpdateData();

	// seperate the 'first' numbers and 'second' numbers
	const intpairvector *pInts = m_ListCtrl.getints();
	if( !pInts || pInts->isempty() )
		return;
	std::shared_ptr<extstl::intvector> spFirsts{ new extstl::intvector };
	std::shared_ptr<extstl::intvector> spSeconds{ new extstl::intvector };
	auto i_pair = pInts->get().cbegin(), end = pInts->get().cend();
	for( ; i_pair != end ; ++i_pair )
	{
		spFirsts->push_back( (*i_pair)->first );
		spSeconds->push_back( (*i_pair)->second );
	}
	extstl::intvector::sorted().sort(spFirsts.get(),extstl::intvector::sorted::S_t());
	extstl::intvector::sorted().sort(spSeconds.get(),extstl::intvector::sorted::S_t());
	{
		// erasing of the duplicates should be a member function that detects how things are sorted and restores i.e. what we do here
		for( int n = static_cast<int>(spFirsts->get().size()-1) ; n > 0 ; --n )
		{
			if( spFirsts->get()[n] == spFirsts->get()[n-1] )
				spFirsts->erase(n,n+1);
			if( spSeconds->get()[n] == spSeconds->get()[n-1] )
				spSeconds->erase(n,n+1);
		}
	}
	spFirsts->setsorted(extstl::intvector::sorted::S_t::getid());
	spSeconds->setsorted(extstl::intvector::sorted::S_t::getid());

	std::shared_ptr<extstl::intvector> spANOTB{ new extstl::intvector };
	std::shared_ptr<extstl::intvector> spBNOTA{ new extstl::intvector };
	std::shared_ptr<extstl::intvector> spAANDB{ new extstl::intvector };
	extstl::intvector::sorted().diff(spFirsts.get(),spSeconds.get(),spANOTB.get(),spBNOTA.get(),spAANDB.get(),extstl::intvector::sorted::S_t(),extstl::intvector::sorted::U_t());

	CString cs;
	{
		cs += _T("First only:");
		auto i_int = spANOTB->get().cbegin(), end = spANOTB->get().cend();
		for( ; i_int != end ; ++i_int )
		{
			CString csTmp;
			if( i_int == spANOTB->get().cbegin() )
				csTmp.Format(_T(" %li"),*i_int);
			else
				csTmp.Format(_T(", %li"),*i_int);
			cs += csTmp;
		}
	}
	cs += _T("\r\n\r\n");
	{
		cs += _T("Second only:");
		auto i_int = spBNOTA->get().cbegin(), end = spBNOTA->get().cend();
		for( ; i_int != end ; ++i_int )
		{
			CString csTmp;
			if( i_int == spBNOTA->get().cbegin() )
				csTmp.Format(_T(" %li"),*i_int);
			else
				csTmp.Format(_T(", %li"),*i_int);
			cs += csTmp;
		}
	}
	cs += _T("\r\n\r\n");
	{
		cs += _T("Both:");
		auto i_int = spAANDB->get().cbegin(), end = spAANDB->get().cend();
		for( ; i_int != end ; ++i_int )
		{
			CString csTmp;
			if( i_int == spAANDB->get().cbegin() )
				csTmp.Format(_T(" %li"),*i_int);
			else
				csTmp.Format(_T(", %li"),*i_int);
			cs += csTmp;
		}
	}

	AfxMessageBox(cs);
}

enum intpairvector::E_t CsortedvectorDlg::getsortfromindex( const int n ) const
{
	switch( n )
	{
		case 0:return intpairvector::E_t::primary;
		case 1:return intpairvector::E_t::firstint;
		case 2:return intpairvector::E_t::secondint;
		default:ASSERT( false ); return intpairvector::E_t::null;
	}
}

int CsortedvectorDlg::getindexfromsort( const enum intpairvector::E_t s ) const
{
	switch( s )
	{
		case intpairvector::E_t::primary:return 0;
		case intpairvector::E_t::firstint:return 1;
		case intpairvector::E_t::secondint:return 2;
		default:ASSERT( false ); return intpairvector::E_t::null;
	}
}
