// HsvScrollBar.cpp : ��@��
//

#include "stdafx.h"
#include "demo.h"
#include "HsvScrollBar.h"
#include "afxdialogex.h"

//***�ϥΤ���ܮؤ��ܼ�***//
//�l��ܮؤ�����
extern CHsvScrollBar* pHsvdlg; 
//initial value for ball color 
extern UINT hmin, hmax, smin, smax, vmin, vmax;
UINT hmin_old, hmax_old, smin_old, smax_old, vmin_old, vmax_old; //�O���l�ȡA�p�G���������s�A�i�H�^�_���l��
//if call hsv modeless dialog
extern bool is_hsv_tunning;

// CHsvScrollBar ��ܤ��

IMPLEMENT_DYNAMIC(CHsvScrollBar, CDialogEx)

CHsvScrollBar::CHsvScrollBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHsvScrollBar::IDD, pParent)
	, m_edit_hmin(hmin)
	, m_edit_hmax(hmax)
	, m_edit_smin(smin)
	, m_edit_smax(smax)
	, m_edit_vmin(vmin)
	, m_edit_vmax(vmax)
{
	//�O���l�ȡA�p�G���������s�A�i�H�^�_���l��
	hmin_old = hmin;
	hmax_old = hmax;
	smin_old = smin;
	smax_old = smax;
	vmin_old = vmin;
	vmax_old = vmax;
}

BOOL CHsvScrollBar::OnInitDialog()   
{
	CDialogEx::OnInitDialog();

	m_slider_hmin.SetRange(0, 180);
	m_slider_hmax.SetRange(0, 210);
	m_slider_smin.SetRange(0, 256);
	m_slider_smax.SetRange(0, 256);
	m_slider_vmin.SetRange(0, 256);
	m_slider_vmax.SetRange(0, 256);

	m_slider_hmin.SetPos(hmin);
	m_slider_hmax.SetPos(hmax);
	m_slider_smin.SetPos(smin);
	m_slider_smax.SetPos(smax);
	m_slider_vmin.SetPos(vmin);
	m_slider_vmax.SetPos(vmax);



	return TRUE;
}

CHsvScrollBar::~CHsvScrollBar()
{
}

void CHsvScrollBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_Hmin, m_slider_hmin);
	DDX_Control(pDX, IDC_SLIDER_Hmax, m_slider_hmax);
	DDX_Control(pDX, IDC_SLIDER_Smin, m_slider_smin);
	DDX_Control(pDX, IDC_SLIDER_Smax, m_slider_smax);
	DDX_Control(pDX, IDC_SLIDER_Vmin, m_slider_vmin);
	DDX_Control(pDX, IDC_SLIDER_Vmax, m_slider_vmax);

	DDX_Text(pDX, IDC_EDIT_Hmin, m_edit_hmin);
	DDX_Text(pDX, IDC_EDIT_Hmax, m_edit_hmax);
	DDX_Text(pDX, IDC_EDIT_Smin, m_edit_smin);
	DDX_Text(pDX, IDC_EDIT_Smax, m_edit_smax);
	DDX_Text(pDX, IDC_EDIT_Vmin, m_edit_vmin);
	DDX_Text(pDX, IDC_EDIT_Vmax, m_edit_vmax);
	
	DDV_MinMaxUInt(pDX, m_edit_hmin, 0, 180);
	DDV_MinMaxUInt(pDX, m_edit_hmax, 0, 210);
	DDV_MinMaxUInt(pDX, m_edit_smin, 0, 256);
	DDV_MinMaxUInt(pDX, m_edit_smax, 0, 256);
	DDV_MinMaxUInt(pDX, m_edit_vmin, 0, 256);
	DDV_MinMaxUInt(pDX, m_edit_vmax, 0, 256);
}

BEGIN_MESSAGE_MAP(CHsvScrollBar, CDialogEx)
	ON_BN_CLICKED(IDOK, &CHsvScrollBar::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHsvScrollBar::OnBnClickedCancel)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_Hmin, &CHsvScrollBar::OnEnChangeEditHmin)
	ON_EN_CHANGE(IDC_EDIT_Hmax, &CHsvScrollBar::OnEnChangeEditHmax)
	ON_EN_CHANGE(IDC_EDIT_Smin, &CHsvScrollBar::OnEnChangeEditSmin)
	ON_EN_CHANGE(IDC_EDIT_Smax, &CHsvScrollBar::OnEnChangeEditSmax)
	ON_EN_CHANGE(IDC_EDIT_Vmin, &CHsvScrollBar::OnEnChangeEditVmin)
	ON_EN_CHANGE(IDC_EDIT_Vmax, &CHsvScrollBar::OnEnChangeEditVmax)
END_MESSAGE_MAP()

//// CHsvScrollBar �T���B�z�`��
//

void CHsvScrollBar::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	pHsvdlg = NULL;
	is_hsv_tunning = FALSE;
	delete this;
}

// CModelessDlg �T���B�z�`��

void CHsvScrollBar::OnBnClickedOk()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	if (UpdateData(TRUE)){ //��DDV�ˬd��ƥ��T�A���T�^��1 => ��������
		is_hsv_tunning = FALSE;
		DestroyWindow();
	}
}

void CHsvScrollBar::OnBnClickedCancel()
{
	//�ר�����N�^�_��l��
	hmin = hmin_old;
	hmax = hmax_old;
	smin = smin_old;
	smax = smax_old;
	vmin = vmin_old;
	vmax = vmax_old;

	is_hsv_tunning = FALSE;
	DestroyWindow();
}

void CHsvScrollBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID())
	{
	//�Y�O��ID_SLIDER1_PARAM1�ҵo�X�T���A�h�^���H�U�ƶ�
	case IDC_SLIDER_Hmin:
		nPos = m_slider_hmin.GetPos();//���oSlider�������m
		m_edit_hmin = nPos;//���Slider��m
		hmin = nPos;
		break;
	case IDC_SLIDER_Hmax:
		nPos = m_slider_hmax.GetPos();
		m_edit_hmax = nPos;
		hmax = nPos;
		break;
	case IDC_SLIDER_Smin:
		nPos = m_slider_smin.GetPos();
		m_edit_smin = nPos;
		smin = nPos;
		break;
	case IDC_SLIDER_Smax:
		nPos = m_slider_smax.GetPos();
		m_edit_smax = nPos;
		smax = nPos;
		break;
	case IDC_SLIDER_Vmin:
		nPos = m_slider_vmin.GetPos();
		m_edit_vmin = nPos;
		vmin = nPos;
		break;
	case IDC_SLIDER_Vmax:
		nPos = m_slider_vmax.GetPos();
		m_edit_vmax = nPos;
		vmax = nPos;
		break;
	}
	//�N���ε{�����ܼƸ�Ƨ�s�ܹ�ܲ�
	UpdateData(FALSE);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHsvScrollBar::OnEnChangeEditHmin()
{
	UpdateData(TRUE);
	m_slider_hmin.SetPos(m_edit_hmin);
	hmin = m_edit_hmin;
}

void CHsvScrollBar::OnEnChangeEditHmax()
{
	UpdateData(TRUE);
	m_slider_hmax.SetPos(m_edit_hmax);
	hmax = m_edit_hmax;
}

void CHsvScrollBar::OnEnChangeEditSmin()
{
	UpdateData(TRUE);
	m_slider_smin.SetPos(m_edit_smin);
	smin = m_edit_smin;
}

void CHsvScrollBar::OnEnChangeEditSmax()
{
	UpdateData(TRUE);
	m_slider_smax.SetPos(m_edit_smax);
	smax = m_edit_smax;
}

void CHsvScrollBar::OnEnChangeEditVmin()
{
	UpdateData(TRUE);
	m_slider_vmin.SetPos(m_edit_vmin);
	vmin = m_edit_vmin;
}

void CHsvScrollBar::OnEnChangeEditVmax()
{
	UpdateData(TRUE);
	m_slider_vmax.SetPos(m_edit_vmax);
	vmax = m_edit_vmax;
}
