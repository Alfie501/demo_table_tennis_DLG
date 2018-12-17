#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CHsvScrollBar 對話方塊

class CHsvScrollBar : public CDialogEx
{
	DECLARE_DYNAMIC(CHsvScrollBar)

public:
	CHsvScrollBar(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CHsvScrollBar();

// 對話方塊資料
	enum { IDD = IDD_HSV_ScrollBar };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

//覆寫關閉程序
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CSliderCtrl m_slider_hmin;
	CSliderCtrl m_slider_hmax;
	CSliderCtrl m_slider_smin;
	CSliderCtrl m_slider_smax;
	CSliderCtrl m_slider_vmin;
	CSliderCtrl m_slider_vmax;
	UINT m_edit_hmin;
	UINT m_edit_hmax;
	UINT m_edit_smin;
	UINT m_edit_smax;
	UINT m_edit_vmin;
	UINT m_edit_vmax;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditHmin();
	afx_msg void OnEnChangeEditHmax();
	afx_msg void OnEnChangeEditSmin();
	afx_msg void OnEnChangeEditSmax();
	afx_msg void OnEnChangeEditVmin();
	afx_msg void OnEnChangeEditVmax();
};
