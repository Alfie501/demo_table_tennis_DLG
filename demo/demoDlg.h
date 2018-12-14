
// demoDlg.h: 標頭檔
//


#pragma once
#pragma warning(disable : 4996)
//opencv header
#include "cv.h"
#include <highgui.h>
using namespace cv;

//flycapture header
#include "FlyCapture2.h"
using namespace FlyCapture2;

//Stereo header
//#include "Stereo.h"
//using namespace mao; 在這用不好

//Picture Control header
#include "CvvImage.h"
#include "afxwin.h"

//String Stream 
#include <Cstring>
#include <sstream>
using namespace std;

//Camera Thread
enum Camera_State {
	START,
	PAUSE,
	STOP
};

//for float and double precision
#include <limits>
typedef std::numeric_limits< double > dbl;
//cout.precision(dbl::max_digits10);

//Write a txt file 
#include "fstream"
#include "afxcmn.h"

//format output txt format 
#include <iomanip>

// CdemoDlg 對話方塊
class CdemoDlg : public CDialogEx
{
// 建構
public:
	CdemoDlg(CWnd* pParent = nullptr);	// 標準建構函式		//nullptr / NULL
	
// 對話方塊資料
/*
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEMO_DIALOG };
#endif
*/

	enum { IDD = IDD_DEMO_DIALOG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 支援
	
	
// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCameraL();
	afx_msg void OnBnClickedCameraR();
	afx_msg void OnBnClickedImgshow();
	
	CEdit m_MainEditBox;
	CListBox m_MainListBox;

private:
	static CWinThread *pCameraThread_L;
	static Camera_State camera_state_L;
	static UINT CameraThread_L(LPVOID pParam);

	static CWinThread *pCameraThread_R;
	static Camera_State camera_state_R;
	static UINT CameraThread_R(LPVOID pParam);

private:
	static CWinThread *pImageShowThread;
	static UINT ImageShowThread(LPVOID pParam);


};