
// sortedvectorDlg.h : header file
//

#pragma once

#include "listctrl.h"

// CsortedvectorDlg dialog
class CsortedvectorDlg : public CDialogEx
{
// Construction
public:
	CsortedvectorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SORTEDVECTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnPopulate(void);
	afx_msg void OnAscending(void);
	afx_msg void OnSort(void);
	afx_msg void OnFind(void);
	afx_msg void OnDiff(void);
	DECLARE_MESSAGE_MAP()

	listctrl m_ListCtrl;
	int m_nAscending;
	int m_nSort;

	enum intpairvector::E_t getsortfromindex( const int n ) const;
	int getindexfromsort( const enum intpairvector::E_t s ) const;
};
