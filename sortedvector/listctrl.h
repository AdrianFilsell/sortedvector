#pragma once

#include <afxcmn.h>
#include <memory>
#include "vector.h"

class theme
{
public:
	theme(){m_hTheme=NULL;}
	~theme(){close();}
	
	bool open( const HWND h, LPCWSTR lp )
	{
		close();
		m_hTheme = ::OpenThemeData( h, lp );
		return m_hTheme != NULL;
	}
	void close( void )
	{
		if( m_hTheme )
		{
			::CloseThemeData( m_hTheme );
			m_hTheme = NULL;
		}
	}
	bool render( HWND h, HDC hDC, const int nPartID, const int nStateID, const CRect& rcPaint, const CRect& rcClip ) const
	{
		if( !m_hTheme )
			return false;
		if( IsThemeBackgroundPartiallyTransparent( m_hTheme, nPartID, nStateID ) )
			DrawThemeParentBackground( h, hDC, rcPaint );
		DrawThemeBackground( m_hTheme, hDC, nPartID, nStateID, rcPaint, rcClip );
		return true;
	}
protected:
	HTHEME m_hTheme;
};

class intpair : public std::pair<int,int>
{
public:
	intpair(){}
	intpair(const std::pair<int,int>& o):std::pair<int,int>(o){}
	~intpair(){}

	bool operator ==( const intpair& other ) const { return first == other.first && second == other.second; }
	bool operator <( const intpair& other ) const { if( first < other.first ) return true; if( first > other.first ) return false; return ( second < other.second); }
	
	intpair& operator =( const intpair& other ) { *this = other; return *this; }
};

template <typename V> struct firstintvectorequalpred
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return spLhs->first == spRhs->first; }
};

template <typename V,enum V::E_t ID> struct firstintvectorlesspred : public std::binary_function<V::template T_t, V::template T_t, bool>
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return spLhs->first < spRhs->first; }
	static V::template E_t getid( void ) {return ID;}
};

template <typename V> struct secondintvectorequalpred
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return spLhs->second == spRhs->second; }
};

template <typename V,enum V::E_t ID> struct secondintvectorlesspred : public std::binary_function<V::template T_t, V::template T_t, bool>
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return spLhs->second < spRhs->second; }
	static V::template E_t getid( void ) {return ID;}
};

struct intpairvectororder
{
	enum type { null = 0, primary, firstint, secondint };
};

class intpairvector : public extstl::vector<std::shared_ptr<const intpair>,intpairvectororder::type>
{
public:	
	intpairvector(){}
	std::shared_ptr<intpairvector> clone( void ) const { std::shared_ptr<intpairvector> sp{new intpairvector(*this)}; return sp; }
	static std::shared_ptr<const intpairvector> clonesort( const std::shared_ptr<const intpairvector> in, const E_t e )
	{
		if( !in || ( in->getsorted() == e ) )
			return in;
		std::shared_ptr<intpairvector> out = in->clone();
		switch( e )
		{
			case intpairvectororder::null:out->setsorted(e);break;
			case intpairvectororder::primary:sorted().sort( out.get(), sorted::S_t() );break;
			case intpairvectororder::firstint:firstintsorted().sort( out.get(), firstintsorted::S_t() );break;
			case intpairvectororder::secondint:secondintsorted().sort( out.get(), secondintsorted::S_t() );break;
			default:ASSERT(false);return in;
		}
		return out;
	}

	intpairvector& operator =(const intpairvector& o ) { extstl::vector<std::shared_ptr<const intpair>,intpairvectororder::type>::operator=(o); return *this; }
	
	// primary type comparison
	typedef extstl::vectorequalderefpred<intpairvector> equalpred;
	typedef extstl::vectorlessderefpred<intpairvector,intpairvectororder::primary> lesspred;
	typedef extstl::vectorsorted<intpairvector,lesspred,equalpred> sorted;

	// first int comparison
	typedef firstintvectorequalpred<intpairvector> firstintequalpred;
	typedef firstintvectorlesspred<intpairvector,intpairvectororder::firstint> firstintlesspred;
	typedef extstl::vectorsorted<intpairvector,firstintlesspred,firstintequalpred> firstintsorted;

	// second int comparison
	typedef secondintvectorequalpred<intpairvector> surnameequalpred;
	typedef secondintvectorlesspred<intpairvector,intpairvectororder::secondint> secondintlesspred;
	typedef extstl::vectorsorted<intpairvector,secondintlesspred,surnameequalpred> secondintsorted;
};

class listctrl : public CListCtrl
{
public:
	listctrl(){m_SortType = intpairvector::E_t::primary;m_bAscending=true;}
	virtual ~listctrl(){}

	const intpairvector *getints( void ) const { return m_spInts.get(); }

	virtual void PreSubclassWindow() override;

	void setascending( const bool b );
	void setsort( const intpairvector::E_t s);
	void setints( const std::shared_ptr<const intpairvector> sp );
protected:
	//{{AFX_VIRTUAL(listctrl)
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(listctrl)
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	afx_msg int OnCreate(LPCREATESTRUCT lpCIS);
	afx_msg void OnSetFocus(CWnd *p);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void setuphdr( void );
	void getitemcolumns( const CRect& rcItemRect, std::vector<CRect>& vColumns ) const;
	enum columntype { ct_firstname, ct_surname };
	columntype gettypefromcolumn( const int nColumn ) const { ASSERT(nColumn>=ct_firstname && nColumn<=ct_surname); return columntype(nColumn); }
	int getcolumnfromtype( const columntype t ) const;
	void populateints( void );

	std::shared_ptr<const intpairvector> m_spInts;
	intpairvector::E_t m_SortType;
	bool m_bAscending;
};

class intpairdlg : public CDialogEx
{
public:
	intpairdlg(CWnd *pParent,const enum intpairvector::E_t s);

	virtual BOOL DestroyWindow();

// Dialog Data
	int m_nFirst;
	int m_nSecond;
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTPAIR };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	enum intpairvector::E_t m_SortType;
};
