
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
#include "Stereo.h"
//using namespace mao; 在這用不好

//Picture Control header
#include "CvvImage.h"
#include "afxwin.h"

//HSV Modeless Dialog
#include "HsvScrollBar.h"

//String Stream  
#include <Cstring>
#include <sstream>
#include <string>
#include <math.h>
using namespace std;

//Camera Thread
enum Camera_State {
	START,
	PAUSE,
	STOP
};

//Traking Method
enum TrackingMethod {//追蹤方法選擇
	None,
	FindContour,
	Hough
};

enum FindContour_State {
	FindContourStart,
	FindContourPause,
	FindContourStop
};

enum Trajectory_State {
	Trajectory_State_Off,
	Trajectory_State_Start
};


//Ball Trajectory
#include<vector>
#include "BallTrajectory.h"									//先忽略 20181218
class CBallState {
private:
	double x, y, z;  //position in world coordinate 
	double t;		//timestamp
	unsigned int i; //index number 
	double vx, vy, vz;
public:
	void SetBallState(unsigned int index, double x_value, double y_value, double z_value, double t_value);
	void SetBallState(unsigned int index, double x_value, double y_value, double z_value, double Velocity_x, double Velocity_y, double Velocity_z, double t_value);
	double GetPosition_x() { return x; }
	double GetPosition_y() { return y; }
	double GetPosition_z() { return z; }
	double GetTimestamp_t() { return t; }
	unsigned int GetIndex() { return i; }
	double GetVelocity_x() { return vx; }
	double GetVelocity_y() { return vy; }
	double GetVelocity_z() { return vz; }

};


//for float and double precision
#include <limits>
typedef std::numeric_limits< double > dbl;
//cout.precision(dbl::max_digits10);

//Write a txt file 
#include "fstream"
#include "iostream"
#include "afxcmn.h"

//format output txt format 
#include <iomanip>

//for debug
//#include <malloc.h>
//#include <crtdbg.h>
//_CrtSetBreakAlloc(number);

// CdemoDlg 對話方塊
class CdemoDlg : public CDialogEx
{
// 建構
public:
	CdemoDlg(CWnd* pParent = NULL);	// 標準建構函式		//nullptr / NULL
	
// 對話方塊資料
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	static CWinThread *pCameraThread_L;
	static Camera_State camera_state_L;
	static UINT CameraThread_L(LPVOID pParam);

	static CWinThread *pCameraThread_R;
	static Camera_State camera_state_R;
	static UINT CameraThread_R(LPVOID pParam);

public:
	CEdit m_MainEditBox;
	CListBox m_MainListBox;

private:
	static CWinThread *pImageShowThread;
	static UINT ImageShowThread(LPVOID pParam);

private:
	static CWinThread *pFindContour;
	static FindContour_State find_contour_state;
	static UINT FindContourThread(LPVOID pParam);

	static CWinThread *pFindContourR;
	static FindContour_State find_contour_state_R;
	static UINT FindContourThreadR(LPVOID pParam);

public:
	afx_msg void OnBnClickedHsvFilter();
	afx_msg void OnBnClickedStopcapture();

public:
	CComboBox m_TrackingMethod;
	afx_msg void OnCbnSelchangeTrackingmethod();

private:
	static TrackingMethod MethodChoose;

private:
	static CWinThread *pTrajectoryPredict;
	static UINT TrajectoryPredict(LPVOID pParam);
	
public:
	afx_msg void OnBnClickedListboxclear();
	afx_msg void OnBnClickedShowtext();

//private:
//	static CWinThread *pHSVThread_L;
//	static UINT HSVThread_L(LPVOID pParam);
//
//	static CWinThread *pHSVThread_R;
//	static UINT HSVThread_R(LPVOID pParam);
//public:
//	afx_msg void OnBnClickedprocessingHsv();

	afx_msg void OnBnClickedopenpicturecontroll();
	afx_msg void OnBnClickedImgsave();

	//afx_msg void printMatrix(vector<double> matrix);
	//afx_msg void printColumn(vector<double> matrix);
	
private:
	static CWinThread *pTrajectoryPredict_XYZ;	
	static UINT TrajectoryPredict_XYZ(LPVOID pParam);

private:
	static Trajectory_State TrajectoryState;
	/*static UINT Bpnn(LPVOID pParam);
	static CWinThread *pBpnn;
		
	static vector<double> readfile(string filename);
	static vector<double> separateMat(vector<double> matrix, int column);
	static double calcuMSE(vector<double> testdata, double* weight, double* weight1, double* weight2);*/
};




