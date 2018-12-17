
// demoDlg.cpp: 實作檔案
//

#include "stdafx.h"
#include "demo.h"
#include "demoDlg.h"
#include "afxdialogex.h"
//#include <opencv/highgui.h>
#include <math.h>
#include "afx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




//------初始宣告與定義------
//***Image Capture***//
Mat imageL;

//Initialized static member of CdemotDlg
CWinThread* CdemoDlg::pCameraThread_L = NULL;
Camera_State CdemoDlg::camera_state_L = STOP;

Mat imageR;
//Initialized static member of CdemotDlg
CWinThread* CdemoDlg::pCameraThread_R = NULL;
Camera_State CdemoDlg::camera_state_R = STOP;

//Initialized static member of CdemotDlg
CWinThread* CdemoDlg::pImageShowThread = NULL;

//***showing frame_rate***//
unsigned int frames_L = 0;
unsigned int frames_R = 0;

//***for show image***///
bool is_ShowImg_Receiving_L = 0;
bool is_ShowImg_Receiving_R = 0;
IplImage * show_imgl = NULL;
IplImage * show_imgr = NULL;
unsigned int ImgShowRate(0);
unsigned int ImgShowRate_R(0);

//***HSV Slider Bar Modeless Dialog***//
CHsvScrollBar* pHsvdlg = NULL;

//***for traking method ***//
TrackingMethod CdemoDlg::MethodChoose = None;
bool isFindContourReceivingPic_L = 0;
bool isFindContourReceivingPic_R = 0;
CEvent Event_Contour_Pic(FALSE, TRUE); //手動設定模式，初始狀態為FALSE
CEvent Event_Contour_Pic_R(FALSE, TRUE); //手動設定模式，初始狀態為FALSE
Mat ContourImage_L, ContourImage_R;

bool is_hsv_tunning = 0;
Mat hsv_L, mask_L, mask1_L, hsv_R, mask_R, mask1_R;
//UINT hmin = 20, hmax = 30, smin = 40, smax = 256, vmin = 232, vmax = 256;  //initial value for ball color 
//UINT hmin = 11, hmax = 30, smin = 89, smax = 256, vmin = 232, vmax = 256;  //initial value for ball color 
UINT hmin = 11, hmax = 30, smin = 100, smax = 256, vmin = 232, vmax = 256;  //initial value for ball color 

//be used to findcontour
CWinThread* CdemoDlg::pFindContour = NULL;
FindContour_State CdemoDlg::find_contour_state = FindContourStop;

CWinThread* CdemoDlg::pFindContourR = NULL;
FindContour_State CdemoDlg::find_contour_state_R = FindContourStop;

unsigned int processing_rate = 0;
unsigned int processing_rate_R = 0;

bool isFindBall_L(0);
bool isFindBall_R(0);
bool is_SearchWindowGet_L(0);
bool is_SearchWindowGet_R(0);
bool is_ActiveFramesDifference(1);
//unsigned int Ix_L = 0, Iy_L = 0, Ix_R = 0, Iy_R = 0;
float Ix_L = 0, Iy_L = 0, Ix_R = 0, Iy_R = 0;
float radius_L = 0, radius_R = 0;
float hx_L = 0, hy_L = 0, hz_L = 0;
float X, Y, Z; //world coordinate


//ROI Setting							//2018-05-06
#if 1
UINT LeftCameraoffsetX = 0;
UINT LeftCameraoffsetY = 0;
UINT LeftCameraWidth = 2048;
UINT LeftCameraHeight = 1536;

UINT RightCameraoffsetX = 0;
UINT RightCameraoffsetY = 0;
UINT RightCameraWidth = 2048;
UINT RightCameraHeight = 1536;

#else if
UINT LeftCameraoffsetX = 384;
UINT LeftCameraoffsetY = 256;
UINT LeftCameraWidth = 1280;
UINT LeftCameraHeight = 1024;

UINT RightCameraoffsetX = 384;
UINT RightCameraoffsetY = 256;
UINT RightCameraWidth = 1280;
UINT RightCameraHeight = 1024;

#endif

//transform c++ string to windows CString and show.
bool is_EnableShowText = 1;

void ShowText(string str) {//ascii string
	if (is_EnableShowText) {

		CWnd* pMainWnd = AfxGetMainWnd();
		CListBox* pListBox = (CListBox*)pMainWnd->GetDlgItem(IDC_MainListBox);

		string str2;
		CString cstr;

		int start = 0; int end = 0;
		//由於ListBox一次是一整串的輸出，因此如果輸出資料是一大串，那換行得自己來
		//將一整串字串依照換行字元("\r\n")換行，再分別加入 ListBox 
		while (str[start] != '\0') {  //遇到結束字元跳出
			end = str.find("\r\n", start);//從 start 開始尋找 "\r\n" 找到後會得到 '\r'之位置
			if (end == -1) { //if str.find return -1 => can not find the "\r\n" =>  it means only one line
				str2.assign(str, start, end - start);
				cstr = str2.c_str();
				pListBox->AddString(cstr); //show the line directly
				break;
			}
			str2.assign(str, start, end - start); //將 str字串 從 start 位置開始 往後 (end-start) 個字元設定給str2
			cstr = str2.c_str();  //將c++字串轉成 MFC 字串
			pListBox->AddString(cstr);
			start = end + 2;  //將搜尋起點設為終點+2(略過'\r''n')，開始擷取下一筆字串
		}

		int nCount = pListBox->GetCount(); // Declare m_LisBox as a control in ClassView member variables.

		if (nCount >= 100) {  //超過100比就刪掉舊的，假設現在是105筆 就刪掉 0,1,2,3,4 筆
			int i = nCount - 100;
			/*stringstream ss;
			ss << i;
			cstr = ss.str().c_str();
			pListBox->AddString (cstr);*/  //這不知道打幹嘛的 忘記了 應該可以刪掉
			while (i) {
				pListBox->DeleteString(i - 1); //zero-based index, so minus 1 
				i--;
			}
		}
		nCount = pListBox->GetCount();
		pListBox->SetCurSel(nCount - 1); // SetCurrentSelect (List box is zero based. 從零開始的意思，設定選取目標)
		pListBox->SetTopIndex(nCount - 1); // moves the view. 使得選到的那個項目至頂，永遠看的到，選最底部的那個讓清單自動捲動

		//RECT r;
		//pListBox->GetItemRect(0, &r);  //獲得 編號0 的項目長方形大小
		//RECT ListBoxRect;
		//pMainWnd->GetDlgItem(IDC_MainListBox)->GetWindowRect(&ListBoxRect);  //獲得 ListBox 的窗口大小

	}

}

// CdemoDlg 對話方塊


CdemoDlg::CdemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MainListBox, m_MainListBox);
	DDX_Control(pDX, IDC_MainEditBox, m_MainEditBox);
}

BEGIN_MESSAGE_MAP(CdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CameraL, &CdemoDlg::OnBnClickedCameraL)
	ON_BN_CLICKED(IDC_CameraR, &CdemoDlg::OnBnClickedCameraR)
	ON_BN_CLICKED(IDC_Imgshow, &CdemoDlg::OnBnClickedImgshow)
		
	
	ON_BN_CLICKED(IDC_HSV_Filter, &CdemoDlg::OnBnClickedHsvFilter)
END_MESSAGE_MAP()


// CdemoDlg 訊息處理常式

BOOL CdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	/*
	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
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
	*/
	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}


// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CdemoDlg::OnBnClickedCameraL()
{	//initialize	
#if 0
	// TODO: 在此加入控制項告知處理常式程式碼
	Camera Cam_L;
	Image rawImage_L;
	BusManager busMgr;
	PGRGuid guid_L;
	Error error;
	CameraInfo camInfo;
	//busMgr.GetCameraFromSerialNumber(12292560, &guid_L);		//2018-04-24 change
	busMgr.GetCameraFromSerialNumber(17491073, &guid_L);		//2018-06-20

	Cam_L.Connect(&guid_L);

	CWnd * pMainWnd = FromHandle(m_hWnd);
	CWnd * pWnd_ImageShow_redDetect = pMainWnd->GetDlgItem(IDC_PicControl1);
	CDC* dc = pWnd_ImageShow_redDetect->GetWindowDC();

	//Cam_L.SetVideoModeAndFrameRate(VIDEOMODE_FORMAT7, FRAMERATE_30);//這行家了沒辦法改變 frame rate.

	const Mode Format7Mode = MODE_0;
	const PixelFormat Format7PixelFormat = PIXEL_FORMAT_RAW8;  //PIXEL_FORMAT_411YUV8;

	struct Format7Info Format7Info;
	bool supported;
	Format7Info.mode = Format7Mode;  //k_fmt7Mode = mode0
	error = Cam_L.GetFormat7Info(&Format7Info, &supported);

	Format7ImageSettings ImageSettings;
	ImageSettings.mode = Format7Mode;
	ImageSettings.offsetX = LeftCameraoffsetX;
	ImageSettings.offsetY = LeftCameraoffsetY;
	ImageSettings.width = LeftCameraWidth;// 也可設定成 fmt7Info.maxWidth 為最大
	ImageSettings.height = LeftCameraHeight;// 也可設定成 fmt7Info.maxHeight 為最大
	ImageSettings.pixelFormat = Format7PixelFormat;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	error = Cam_L.ValidateFormat7Settings(&ImageSettings, &valid, &fmt7PacketInfo);

	// Set the settings to the camera
	error = Cam_L.SetFormat7Configuration(&ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
	if (error != PGRERROR_OK)
	{
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Camera Setting Error", 0, 0);
	}

	Cam_L.StartCapture();
	//SetTimer(1, 1000, NULL);   //show frame_rate on main window
	//::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 1, 1000, NULL);   //show frame_rate on main window
	
	/*//get picture control dc 
	CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	CWnd * pWnd_ImageShow = pMainWnd->GetDlgItem(IDC_PicControl1);
	CDC* dc = pWnd_ImageShow->GetWindowDC();*/

	RECT rect;
	pMainWnd->GetDlgItem(IDC_PicControl1)->GetWindowRect(&rect);
	IplImage* Img_L2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
	Image BGR_format;

	camera_state_L = START;

	while (camera_state_L == START)
	{

		Cam_L.RetrieveBuffer(&rawImage_L);
		rawImage_L.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &BGR_format);
		//unsigned int rowBytes0 = (double)BGR_format.GetReceivedDataSize() / (double)BGR_format.GetRows();
		//cv::Mat imageL = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData() ,0); //rowBytes0=0
		imageL = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData(), 0); //rowBytes0=0
				

		//imshow("window", imageL);
		//cvWaitKey(1);

		IplImage* Img_L = cvCloneImage(&IplImage(imageL));

		RECT rect;//size of picture control
		GetDlgItem(IDC_PicControl1)->GetWindowRect(&rect);

		IplImage* Img_L2 = cvCreateImage( cvSize( (rect.right - rect.left), (rect.bottom - rect.top) ), 8, 3);
		cvResize(Img_L, Img_L2); //resize the image to fit into the picture control

		CvvImage show;
		show.CopyOf(Img_L2);
		show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top) );

		cvLine(show_imgl, cvPoint(LeftCameraWidth/2, 0), cvPoint(LeftCameraWidth/2, LeftCameraHeight), Scalar(0, 0, 255), 1, 8, 0);
		cvLine(show_imgl, cvPoint(0, LeftCameraHeight/2), cvPoint(LeftCameraWidth, LeftCameraHeight/2), Scalar(0, 0, 255), 1, 8, 0);

		//cvResize(show_imgl, Img_L2);

		//cvShowImage("Img_L2",Img_L2);
		cvShowImage("Img_L", show_imgl);
		cvWaitKey(1);

		cvReleaseImage(&show_imgl);
		cvReleaseImage(&Img_L);
		cvReleaseImage(&Img_L2);
		

		if (!is_ShowImg_Receiving_L) {
			show_imgl = cvCloneImage(&IplImage(imageL));
			is_ShowImg_Receiving_L = 1;
		}

		frames_L++;
	}

	//Stop Capture 
	error = Cam_L.StopCapture();

	// Disconnect the camera
	error = Cam_L.Disconnect();

	//cvReleaseImage(&Img_L2);
	//cvReleaseCapture(&capture);                  //do not use cause it's global
	//cvDestroyWindow("Camera");
#else if
	//if the camera doesn't start, then start it!
	if (camera_state_L != START) {
		pCameraThread_L = AfxBeginThread(CameraThread_L, NULL);
		m_MainEditBox.SetWindowTextW(L"Open the Camera_L");
	}
	else {
		m_MainEditBox.SetWindowTextW(L"Camera_L is  already open!");
	}
#endif
}

UINT CdemoDlg::CameraThread_L(LPVOID pParam)
{
	Camera Cam_L;
	Image rawImage_L;
	BusManager busMgr;
	PGRGuid guid_L;
	Error error;
	CameraInfo camInfo;
	//busMgr.GetCameraFromSerialNumber(17391980, &guid_L);		//2018-05-14
	busMgr.GetCameraFromSerialNumber(17491073, &guid_L);		//2018-06-20

	error = Cam_L.Connect(&guid_L);

	if (error != PGRERROR_OK)
	{
		//::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Error: Can't connect to the Camera!", 0, 0);
		camera_state_L = STOP;
		return -1;
	}

	//Camera Format7 Settings
	const Mode Format7Mode = MODE_0;
	const PixelFormat Format7PixelFormat = PIXEL_FORMAT_RAW8;    
	//const PixelFormat Format7PixelFormat = PIXEL_FORMAT_411YUV8;
	struct Format7Info format7Info;

	bool supported;
	format7Info.mode = Format7Mode;  //k_fmt7Mode = mode0
	error = Cam_L.GetFormat7Info(&format7Info, &supported);

	Format7ImageSettings ImageSettings;
	ImageSettings.mode = Format7Mode;
	ImageSettings.offsetX = LeftCameraoffsetX;
	ImageSettings.offsetY = LeftCameraoffsetY;
	ImageSettings.width = LeftCameraWidth;// 也可設定成 fmt7Info.maxWidth 為最大
	ImageSettings.height = LeftCameraHeight;// 也可設定成 fmt7Info.maxHeight 為最大
	ImageSettings.pixelFormat = Format7PixelFormat;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	error = Cam_L.ValidateFormat7Settings(&ImageSettings, &valid, &fmt7PacketInfo);
	//Cam_L.SetVideoModeAndFrameRate(VIDEOMODE_FORMAT7, FRAMERATE_30);
	// Set the settings to the camera
	//error = Cam_L.SetFormat7Configuration(&ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
	error = Cam_L.SetFormat7Configuration(&ImageSettings, (float)90);
	if (error != PGRERROR_OK)
	{
		//::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Camera Setting Error", 0, 0);
	}

	Cam_L.StartCapture();
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 1, 1000, NULL);   //show frame_rate on main window
	//get picture control dc 
	CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	CWnd * pWnd_ImageShow = pMainWnd->GetDlgItem(IDC_PicControl1);
	CDC* dc = pWnd_ImageShow->GetWindowDC();

	//get and show camera info
	/*stringstream ss;
	string str;
	error = Cam_L.GetCameraInfo(&camInfo);
	ss << " ***Left Camera Information*** " << "\r\n"
		<< " SensorInfo: " << camInfo.sensorInfo << "\r\n"
		<< " Resolution: " << camInfo.sensorResolution << "\r\n"
		<< " SerialNumber: " << camInfo.serialNumber << "\r\n";
	ShowText(ss.str());*/
	//先註解掉

	//get size of picture control,and prepare a image space for resize image to fit in picture control
	RECT rect;
	pMainWnd->GetDlgItem(IDC_PicControl1)->GetWindowRect(&rect);
	IplImage* Img_L2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
	//Cam_L.SetVideoModeAndFrameRate(VIDEOMODE_1280x960RGB, FRAMERATE_30);//this is standard mode, but what i want is custom mode.
	Image BGR_format;

	camera_state_L = START;

	//Video capture loop
	while (camera_state_L == START) {

		Cam_L.RetrieveBuffer(&rawImage_L);
		rawImage_L.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &BGR_format);
		//unsigned int rowBytes0 = (double)BGR_format.GetReceivedDataSize() / (double)BGR_format.GetRows();
		imageL = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData(), 0); //rowBytes0=0

		////show left image in picture control 
		//IplImage* Img_L = cvCloneImage(&IplImage(imageL)); //cvvimage function only take IplImage..
		//cvResize(Img_L, Img_L2); //resize the image to fit into the picture control 
		//CvvImage show;
		//show.CopyOf(Img_L2);
		//show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));
		/*
		if (is_save_picL) {  //for saving picture if i pressed save button
			save_imgl = cvCloneImage(&IplImage(imageL));
			is_save_picL = 0;
			//cvWaitKey(1);
		}*/

		if (!is_ShowImg_Receiving_L && pImageShowThread) {
			show_imgl = cvCloneImage(&IplImage(imageL));
			is_ShowImg_Receiving_L = 1;
		}
		/*
		if (is_AskingSetRoiImg_L) {
			setRoiImgL = imageL.clone();
			is_AskingSetRoiImg_L = 0;
		}*/
		/*
		//for tracking method
		if (MethodChoose != None) {
			switch (MethodChoose)
			{
			case Hough:

				break;
			case FindContour:
				if (!isFindContourReceivingPic_L) {
					ContourImage_L = imageL.clone();
					//waitKey(1);
					Event_Contour_Pic.SetEvent();
					isFindContourReceivingPic_L = 1;
				}
				break;
			default:
				break;
			}   
	
					
		}*/
				//先註解掉

		//cvReleaseImage(&Img_L); //release everytime cause the function cvcloneimage will increase the RAM
		frames_L++;
	}

	//Stop Capture 
	error = Cam_L.StopCapture();
	if (error != PGRERROR_OK)
	{
		return -1;
	}

	// Disconnect the camera
	error = Cam_L.Disconnect();
	if (error != PGRERROR_OK)
	{
		return -1;
	}

	cvReleaseImage(&Img_L2);
	return 0;
}

void CdemoDlg::OnBnClickedCameraR()
{
#if 0
	// TODO: 在此加入控制項告知處理常式程式碼
	Camera Cam_R;
	Image rawImage_R;
	BusManager busMgr;
	PGRGuid guid_R;
	Error error;
	CameraInfo camInfo;
	//busMgr.GetCameraFromSerialNumber(12292560, &guid_R);	//2018-05-14
	busMgr.GetCameraFromSerialNumber(17491067, &guid_R);	//2018-06-20
	
	Cam_R.Connect(&guid_R);

	CWnd * pMainWnd = FromHandle(m_hWnd);
	CWnd * pWnd_ImageShow_redDetect = pMainWnd->GetDlgItem(IDC_PicControl1);
	CDC* dc = pWnd_ImageShow_redDetect->GetWindowDC();

	//Cam_R.SetVideoModeAndFrameRate(VIDEOMODE_FORMAT7, FRAMERATE_30);//這行家了沒辦法改變 frame rate.
	
	//Camera Format7 Settings
	const Mode Format7Mode = MODE_0;
	const PixelFormat Format7PixelFormat = PIXEL_FORMAT_RAW8;  //PIXEL_FORMAT_411YUV8;

	struct Format7Info Format7Info;
	bool supported;
	Format7Info.mode = Format7Mode;  //k_fmt7Mode = mode0
	error = Cam_R.GetFormat7Info(&Format7Info, &supported);

	Format7ImageSettings ImageSettings;
	ImageSettings.mode = Format7Mode;
	ImageSettings.offsetX = RightCameraoffsetX;
	ImageSettings.offsetY = RightCameraoffsetY;// 
	ImageSettings.width = RightCameraWidth;// 也可設定成 fmt7Info.maxWidth 為最大
	ImageSettings.height = RightCameraHeight;// 也可設定成 fmt7Info.maxHeight 為最大
	ImageSettings.pixelFormat = Format7PixelFormat;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	error = Cam_R.ValidateFormat7Settings(&ImageSettings, &valid, &fmt7PacketInfo);

	// Set the settings to the camera
	//error = Cam_R.SetFormat7Configuration(&ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
	error = Cam_R.SetFormat7Configuration(&ImageSettings, (float)90);
	if (error != PGRERROR_OK)
	{
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Camera Setting Error", 0, 0);
	}
	
	//Start Capture
	Cam_R.StartCapture();
	//::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 2, 1000, NULL);   //show frame_rate on main window 
	/*//get picture control handle 
	CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	CWnd * pWnd_ImageShow = pMainWnd->GetDlgItem(IDC_PicControl2);
	CDC* dc = pWnd_ImageShow->GetWindowDC();*/
	
	//get the size of picture control
	RECT rect;//size of picture control
	pMainWnd->GetDlgItem(IDC_PicControl2)->GetWindowRect(&rect);
	IplImage* Img_R2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
	//Cam_R.SetVideoModeAndFrameRate(VIDEOMODE_1280x960Y8, FRAMERATE_60);
	Image BGR_format;

	camera_state_R = START;

	//Video capture loop
	while (camera_state_R == START) {
		Cam_R.RetrieveBuffer(&rawImage_R);
		rawImage_R.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &BGR_format);
		//unsigned int rowBytes0 = (double)BGR_format.GetReceivedDataSize() / (double)BGR_format.GetRows();
		imageR = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData(), 0); //rowBytes0=0

		//imshow("windowR", imageR);
		//cvWaitKey(1);
		//show image on picture control 
		//IplImage* Img_R = cvCloneImage(&IplImage(imageR));//Mat to iplimage 
		//cvResize(Img_R, Img_R2); //resize the image to fit into the picture control 
		//CvvImage show;
		//show.CopyOf(Img_R2);
		//show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));

		//for saving picture if i pressed save button
		IplImage* Img_R = cvCloneImage(&IplImage(imageR));

		RECT rect;//size of picture control
		GetDlgItem(IDC_PicControl2)->GetWindowRect(&rect);

		IplImage* Img_R2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
		cvResize(Img_R, Img_R2); //resize the image to fit into the picture control

		CvvImage show;
		show.CopyOf(Img_R2);
		show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));

		cvLine(show_imgr, cvPoint(LeftCameraWidth / 2, 0), cvPoint(LeftCameraWidth / 2, LeftCameraHeight), Scalar(0, 0, 255), 1, 8, 0);
		cvLine(show_imgr, cvPoint(0, LeftCameraHeight / 2), cvPoint(LeftCameraWidth, LeftCameraHeight / 2), Scalar(0, 0, 255), 1, 8, 0);

		//cvResize(show_imgl, Img_L2);

		cvShowImage("Img_R2", Img_R2);
		cvShowImage("Img_R", show_imgr);
		cvWaitKey(1);

		cvReleaseImage(&show_imgr);
		cvReleaseImage(&Img_R);
		cvReleaseImage(&Img_R2);


		if (!is_ShowImg_Receiving_R) {
			show_imgr = cvCloneImage(&IplImage(imageR));
			is_ShowImg_Receiving_R = 1;
		}

		//cvReleaseImage(&Img_R);
		frames_R++;

	}

	//Stop Capture 
	error = Cam_R.StopCapture();
	

	// Disconnect the camera
	error = Cam_R.Disconnect();

	//cvReleaseImage(&Img_R2);
#else if
	//if the camera doesn't start, then start it!
	if (camera_state_R != START) {
		pCameraThread_R = AfxBeginThread(CameraThread_R, NULL);
		m_MainEditBox.SetWindowTextW(L"Open the Camera_R");
	}
	else {
		m_MainEditBox.SetWindowTextW(L"Camera_R is already open!");
	}
#endif
}

UINT CdemoDlg::CameraThread_R(LPVOID pParam)
{
	Camera Cam_R;
	Image rawImage_R;
	BusManager busMgr;
	PGRGuid guid_R;
	Error error;
	CameraInfo camInfo;
	
	//busMgr.GetCameraFromSerialNumber(12292560, &guid_R);	//2018-05-14
	busMgr.GetCameraFromSerialNumber(17491067, &guid_R);	//2018-06-20
	error = Cam_R.Connect(&guid_R);
	if (error != PGRERROR_OK)
	{
		//::MessageBoxW(AfxGetApp()->m_pMainWnd->m_hWnd, L"Error: Can't connect to the Camera!", 0, 0);
		camera_state_R = STOP;
		return -1;
	}

	//Camera Format7 Settings
	const Mode Format7Mode = MODE_0;
	const PixelFormat Format7PixelFormat = PIXEL_FORMAT_RAW8;
	//const PixelFormat Format7PixelFormat = PIXEL_FORMAT_411YUV8;
	struct Format7Info format7Info;
	bool supported;
	format7Info.mode = Format7Mode;  //k_fmt7Mode = mode0
	error = Cam_R.GetFormat7Info(&format7Info, &supported);

	Format7ImageSettings ImageSettings;
	ImageSettings.mode = Format7Mode;
	ImageSettings.offsetX = RightCameraoffsetX;
	ImageSettings.offsetY = RightCameraoffsetY;// 
	ImageSettings.width = RightCameraWidth;// 也可設定成 fmt7Info.maxWidth 為最大
	ImageSettings.height = RightCameraHeight;// 也可設定成 fmt7Info.maxHeight 為最大
	ImageSettings.pixelFormat = Format7PixelFormat;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	error = Cam_R.ValidateFormat7Settings(&ImageSettings, &valid, &fmt7PacketInfo);

	// Set the settings to the camera
	//error = Cam_R.SetFormat7Configuration(&ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
	error = Cam_R.SetFormat7Configuration(&ImageSettings, (float)90);
	if (error != PGRERROR_OK)
	{
		//::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Camera Setting Error", 0, 0);
	}

	//Start Capture
	Cam_R.StartCapture();
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 2, 1000, NULL);   //show frame_rate on main window 
	//get picture control handle 
	CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	CWnd * pWnd_ImageShow = pMainWnd->GetDlgItem(IDC_PicControl2);
	CDC* dc = pWnd_ImageShow->GetWindowDC();

	//get and show camera info
	/*stringstream ss;
	string str;
	error = Cam_R.GetCameraInfo(&camInfo);
	ss << " ***Right Camera Information*** " << "\r\n"
		<< " SensorInfo: " << camInfo.sensorInfo << "\r\n"
		<< " Resolution: " << camInfo.sensorResolution << "\r\n"
		<< " SerialNumber: " << camInfo.serialNumber << "\r\n";
	ShowText(ss.str());*/
						//先註解掉

	//get the size of picture control
	RECT rect;//size of picture control
	pMainWnd->GetDlgItem(IDC_PicControl2)->GetWindowRect(&rect);
	IplImage* Img_R2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
	//Cam_R.SetVideoModeAndFrameRate(VIDEOMODE_1280x960Y8, FRAMERATE_60);
	Image BGR_format;

	camera_state_R = START;

	//Video capture loop
	while (camera_state_R == START) {
		Cam_R.RetrieveBuffer(&rawImage_R);
		rawImage_R.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &BGR_format);
		//unsigned int rowBytes0 = (double)BGR_format.GetReceivedDataSize() / (double)BGR_format.GetRows();
		imageR = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData(), 0); //rowBytes0=0

		/*imshow("windowR", imageR);
		cvWaitKey(1);*/
		//show image on picture control 
		//IplImage* Img_R = cvCloneImage(&IplImage(imageR));//Mat to iplimage 
		//cvResize(Img_R, Img_R2); //resize the image to fit into the picture control 
		//CvvImage show;
		//show.CopyOf(Img_R2);
		//show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));

		//for saving picture if i pressed save button
		/*
		if (is_save_picR) {
			save_imgr = cvCloneImage(&IplImage(imageR));
			is_save_picR = 0;
			//cvWaitKey(1);
		}*/
		if (!is_ShowImg_Receiving_R && pImageShowThread) {
			show_imgr = cvCloneImage(&IplImage(imageR));
			is_ShowImg_Receiving_R = 1;
		}
		/*
		if (is_AskingSetRoiImg_R) {
			setRoiImgL = imageR.clone();
			is_AskingSetRoiImg_R = 0;
		}*/
		/*
		//for tracking method 
		if (MethodChoose != None) {
			switch (MethodChoose)
			{
			case Hough:

				break;
			case FindContour:
				if (!isFindContourReceivingPic_R) {
					ContourImage_R = imageR.clone();
					//waitKey(1);  //don't use cvwaitkey here,cause it will handle the image processing here
					Event_Contour_Pic_R.SetEvent();
					isFindContourReceivingPic_R = 1;
				}
				break;
			default:

				break;
			}
		}
		*/
						//先註解掉

		//cvReleaseImage(&Img_R);
		frames_R++;

	}

	//Stop Capture 
	error = Cam_R.StopCapture();
	if (error != PGRERROR_OK)
	{
		return -1;
	}

	// Disconnect the camera
	error = Cam_R.Disconnect();
	if (error != PGRERROR_OK)
	{
		return -1;
	}
	cvReleaseImage(&Img_R2);
	return 0;
}

void CdemoDlg::OnBnClickedImgshow()
{
	if (camera_state_L == START || camera_state_R == START) {

		if (pImageShowThread == NULL) {

			pImageShowThread = AfxBeginThread(ImageShowThread, NULL);
			m_MainEditBox.SetWindowTextW(L"Show Image On Screen!");
			//MessageBox(_T("Show Image On Screen!"));
		}
		else {
			m_MainEditBox.SetWindowTextW(L"already On!");
			//MessageBox(_T("already On!"));
		}
	}
	else {
		MessageBox(_T("Camera not open yet!"));
	}
}

UINT CdemoDlg::ImageShowThread(LPVOID pParam)
{
	//::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 5, 1000, NULL);
	//::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 6, 1000, NULL);
	////get picture control handle 
	CWnd * pWnd_ImageShow_L = AfxGetMainWnd()->GetDlgItem(IDC_PicControl1);
	CWnd * pWnd_ImageShow_R = AfxGetMainWnd()->GetDlgItem(IDC_PicControl2);
	CDC* dc_L = pWnd_ImageShow_L->GetWindowDC();
	CDC* dc_R = pWnd_ImageShow_R->GetWindowDC();

	//get the size of left picture control
	RECT rect_l;//size of picture control
	pWnd_ImageShow_L->GetWindowRect(&rect_l);
	IplImage* resize_imgl = cvCreateImage(cvSize((rect_l.right - rect_l.left), (rect_l.bottom - rect_l.top)), 8, 3);

	//get the size of right picture control
	RECT rect_r;//size of picture control
	pWnd_ImageShow_R->GetWindowRect(&rect_r);
	IplImage* resize_imgr = cvCreateImage(cvSize((rect_r.right - rect_r.left), (rect_r.bottom - rect_r.top)), 8, 3);

	CvvImage showl, showr;

	/*Mat searchImg;
	Rect searchWindow(0, 0, 0, 0);
	Mat hsv_L, hsv_R;*/

	while (camera_state_L == START || camera_state_R == START) {

		Sleep(0);//avoid busy loop

		if (is_ShowImg_Receiving_L) {//show left 
			ImgShowRate++;
			/*
			if (isFindBall_L) {  //if find the ball,then show it! 
				//circle the ball here 
				cvCircle(show_imgl, cvPoint(Ix_L, Iy_L), radius_L, Scalar(0, 0, 255), 3, 8, 0); //ix iy radius 為共享 注意thread-safe.
			}*/

			//if (0)
			//{
			//	cvLine(show_imgl, cvPoint(LeftCameraWidth / 2, 0), cvPoint(LeftCameraWidth / 2, LeftCameraHeight), Scalar(0, 0, 255), 1, 8, 0);
			//	cvLine(show_imgl, cvPoint(0, LeftCameraHeight / 2), cvPoint(LeftCameraWidth, LeftCameraHeight / 2), Scalar(0, 0, 255), 1, 8, 0);
			//}  //右眼標示十字
			
			/*
			searchImg = ContourImage_L(searchWindow);
			cvtColor(searchImg, hsv_L, CV_BGR2HSV);	//轉換色彩空間
			*/

			//IplImage *img2 = show_imgl;
			//Mat mat2 = cvarrToMat(img2);
			////cv::Mat mat2(img2, 0);
			//cvNamedWindow("ResultL",1);
			//imshow("ResultL", mat2);
			//waitKey(0);//延遲，建議參數時間延長或者設置為0讓圖片繪製完畢。0的意思為等待使用者按下任意鍵才往下執行
			//destroyWindow("ResultL");
			//// 此段使用於上述標註影像十字時

			/*
			if (is_ShowImg_Receiving_L) {
				imshow("ResultL", hsv_L);
				waitKey(1);
			}
			else {
				destroyWindow("ResultL");
			}*/
			

			//resize the image to fit into the picture control 
			cvResize(show_imgl, resize_imgl);
			//show left image in picture control 
			showl.CopyOf(resize_imgl);
			showl.Show(*dc_L, 0, 0, (rect_l.right - rect_l.left), (rect_l.bottom - rect_l.top));
						
			//release image and ask for picture again
			cvReleaseImage(&show_imgl);
			is_ShowImg_Receiving_L = 0;
		}

		if (is_ShowImg_Receiving_R) {//show right camera
			ImgShowRate_R++;
			/*
			if (isFindBall_R) {
				//circle the ball here 
				cvCircle(show_imgr, cvPoint(Ix_R, Iy_R), radius_R, Scalar(0, 0, 255), 3, 8, 0);
			}
			*/

			//if (0)
			//{
			//	cvLine(show_imgr, cvPoint(RightCameraWidth / 2, 0), cvPoint(RightCameraWidth / 2, RightCameraHeight), Scalar(0, 0, 255), 1, 8, 0);
			//	cvLine(show_imgr, cvPoint(0, RightCameraHeight / 2), cvPoint(RightCameraWidth, RightCameraHeight / 2), Scalar(0, 0, 255), 1, 8, 0);
			//}
			
			//IplImage *img2 = show_imgr;
			//Mat mat2 = cvarrToMat(img2);
			////cv::Mat mat2(img2, 0);
			//cvNamedWindow("ResultR", 1);
			//imshow("ResultR", mat2);
			//waitKey(0);//延遲，建議參數時間延長或者設置為0讓圖片繪製完畢。0的意思為等待使用者按下任意鍵才往下執行
			//destroyWindow("ResultR");

			//resize the image to fit into the picture control 
			cvResize(show_imgr, resize_imgr);
			//show right image in picture control 
			showr.CopyOf(resize_imgr);
			showr.Show(*dc_R, 0, 0, (rect_r.right - rect_r.left), (rect_r.bottom - rect_r.top));
			//release image and ask for picture again
			cvReleaseImage(&show_imgr);


			is_ShowImg_Receiving_R = 0;
		}
		//cvWaitKey(1);  // waitkey is not necessary cause there's no opencv windows event here! 
	}
	pImageShowThread = NULL;
	return 0;
}

UINT CdemoDlg::FindContourThread(LPVOID pParam) {
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 3, 1000, NULL);   //show frame_rate on main window

	Mat searchImg;
	Rect searchWindow(0, 0, 0, 0);

	Mat lastImg;

	Point2f center_point;
	float radius;

	DWORD t1, t2;

	while (find_contour_state == FindContourStart) {

		//if (WaitForSingleObject(Event_Contour_Pic_R, 0) == WAIT_OBJECT_0){//wait until motivate the event (get the picture)
		if (isFindContourReceivingPic_L == 1) {//wait until motivate the event (get the picture)
			t1 = GetTickCount();

			if (is_SearchWindowGet_L) {
				searchImg = ContourImage_L(searchWindow);
				cvtColor(searchImg, hsv_L, CV_BGR2HSV);	//轉換色彩空間
			}
			else {
				cvtColor(ContourImage_L, hsv_L, CV_BGR2HSV);	//轉換色彩空間
			}

			if (hmax <= 180) {
				inRange(hsv_L, Scalar(MIN(hmax, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),
					Scalar(MAX(hmax, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_L);
			}
			else {//special situation hue:180~210 

				inRange(hsv_L, Scalar(MIN(180, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),      //whether if hue between : hmin ~ 180
					Scalar(MAX(180, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_L);

				// hmax - 180 means we want 180~210 stands for 0~30 in hue value 
				inRange(hsv_L, Scalar(MIN(hmax - 180, 0), MIN(smax, smin), MIN(vmax, vmin), 0),   //whether if hue between : 0 ~ (hmax-180)
					Scalar(MAX(hmax - 180, 0), MAX(smax, smin), MAX(vmax, vmin), 0), mask1_L);
				add(mask_L, mask1_L, mask_L);
			}

			
			if (is_hsv_tunning == TRUE) {
				imshow("mask_L", mask_L);
				waitKey(1);
			}
			else {
				destroyWindow("mask_L");
			}

			int largest_area_L = 0;
			int largest_contour_index_L = 0;
			Rect bounding_rect_L;
			vector<vector<Point>> contours_L; // Vector for storing contour
			vector<Vec4i> hierarchy_L;

			findContours(mask_L, contours_L, hierarchy_L, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

			if (contours_L.size() > 0) { //there is some contour
				// iterate through each contour.
				for (int i = 0; i < contours_L.size(); i++)
				{
					//  Find the area of contour
					double a = contourArea(contours_L[i], false);
					/*stringstream ss;
					ss << a;
					ShowText(ss.str());*/
					// Store the index of largest contour
					if (a >= 30 && a <= 420) {
						minEnclosingCircle(contours_L[i], center_point, radius);
						if (4 <= radius && radius <= 15) {
							if (a > largest_area_L) {
								largest_area_L = a;
								largest_contour_index_L = i;
							}
							isFindBall_L = TRUE;
						}
					}
				}
				/*if (largest_area_L == 0) {													//20181215
					is_SearchWindowGet_L = NULL;
					AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"No Ball!");
					isFindBall_L = FALSE;
				}*/

				//decide if the contour is a ball or not 
				//if (largest_area_R>300){//it's not ball 
				//	AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"Not a Ball!");
				//	isFindBall_R = FALSE;
				//}
				//else{// there is a ball! 
				// Find the bounding circle for biggest contour

				if (isFindBall_L == TRUE)
				{
					if (is_SearchWindowGet_L) {
						minEnclosingCircle(contours_L[largest_contour_index_L], center_point, radius);

						center_point.x = center_point.x + searchWindow.x;//在 MatImage 中球的位置
						center_point.y = center_point.y + searchWindow.y;

						circle(ContourImage_L, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_L = (int)center_point.x;
						Iy_L = (int)center_point.y;*/

						Ix_L = center_point.x;
						Iy_L = center_point.y;
						radius_L = radius;
						/*AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"Got it!");*/		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						/*AfxGetMainWnd()->SetDlgItemTextW(IDC_L_Radius, str);*/				//20181215
					}
					else {
						minEnclosingCircle(contours_L[largest_contour_index_L], center_point, radius);
						circle(ContourImage_L, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_L = (int)center_point.x;
						Iy_L = (int)center_point.y;*/

						Ix_L = center_point.x;
						Iy_L = center_point.y;

						radius_L = radius;
						/*AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"Got it!");*/		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						/*AfxGetMainWnd()->SetDlgItemTextW(IDC_L_Radius, str);*/				//20181215
					}

					//get the searchWindow for the ball 
					//searchWindow.width = radius * 2 * 3;//球的直徑之三倍
					//searchWindow.height = radius * 2 * 3;
					searchWindow.width = 200;//固定大小，pixels 
					searchWindow.height = 200;

					searchWindow.x = center_point.x - searchWindow.width / 2;
					searchWindow.y = center_point.y - searchWindow.height / 2;

					searchWindow = searchWindow & Rect(0, 0, ContourImage_L.cols, ContourImage_L.rows);// 取交集做越界保護
					rectangle(ContourImage_L, searchWindow, Scalar(255, 255, 255), 2, 8, 0);		//框球

					is_SearchWindowGet_L = TRUE;
				}

			}

			else {//there is no contour
				is_SearchWindowGet_L = NULL;
				//AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"No Ball!");					//20181215
				isFindBall_L = FALSE;
			}

			if (is_hsv_tunning == TRUE) {
				imshow("ResultL", ContourImage_L);
				waitKey(1);
				is_SearchWindowGet_L = FALSE;//dont use searchWindow when tunning the hsv
			}
			else {
				destroyWindow("ResultL");
			}

			//reset the flag to get next picture
			//Event_Contour_Pic_L.ResetEvent();
			isFindContourReceivingPic_L = 0;

			t2 = GetTickCount() - t1;

			processing_rate++;

			//isFindBall_R = FALSE;//給下次判斷是否有找到
		}  //end if

	}//end while 
	pFindContour = NULL;
	destroyAllWindows();
	return 0;
	
}

UINT CdemoDlg::FindContourThreadR(LPVOID pParam) {
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 4, 1000, NULL);   //show frame_rate on main window

	Mat searchImg;
	Rect searchWindow(0, 0, 0, 0);

	Point2f center_point;
	float radius;

	DWORD t1, t2;

	while (find_contour_state_R == FindContourStart) {

		//if (WaitForSingleObject(Event_Contour_Pic_R, 0) == WAIT_OBJECT_0){//wait until motivate the event (get the picture)
		if (isFindContourReceivingPic_R == 1) {//wait until motivate the event (get the picture)
			t1 = GetTickCount();

			if (is_SearchWindowGet_R) {
				searchImg = ContourImage_R(searchWindow);
				cvtColor(searchImg, hsv_R, CV_BGR2HSV);	//轉換色彩空間
			}
			else {
				cvtColor(ContourImage_R, hsv_R, CV_BGR2HSV);	//轉換色彩空間
			}



			if (hmax <= 180) {
				inRange(hsv_R, Scalar(MIN(hmax, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),
					Scalar(MAX(hmax, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_R);
			}
			else {//special situation hue:180~210 

				inRange(hsv_R, Scalar(MIN(180, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),      //whether if hue between : hmin ~ 180
					Scalar(MAX(180, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_R);

				// hmax - 180 means we want 180~210 stands for 0~30 in hue value 
				inRange(hsv_R, Scalar(MIN(hmax - 180, 0), MIN(smax, smin), MIN(vmax, vmin), 0),   //whether if hue between : 0 ~ (hmax-180)
					Scalar(MAX(hmax - 180, 0), MAX(smax, smin), MAX(vmax, vmin), 0), mask1_R);
				add(mask_R, mask1_R, mask_R);
			}


			if (is_hsv_tunning == TRUE) {
				imshow("mask_R", mask_R);
				waitKey(1);

			}
			else {
				destroyWindow("mask_R");
			}

			int largest_area_R = 0;
			int largest_contour_index_R = 0;
			Rect bounding_rect_R;
			vector<vector<Point>> contours_R; // Vector for storing contour
			vector<Vec4i> hierarchy_R;

			findContours(mask_R, contours_R, hierarchy_R, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

			if (contours_R.size() > 0) { //there is some contour
				// iterate through each contour.
				for (int i = 0; i < contours_R.size(); i++)
				{
					//  Find the area of contour
					double a = contourArea(contours_R[i], false);
					// Store the index of largest contour					

					if (a >= 30 && a <= 420) {
						minEnclosingCircle(contours_R[i], center_point, radius);
						if (4 <= radius && radius <= 15) {
							if (a > largest_area_R) {
								largest_area_R = a;
								largest_contour_index_R = i;
							}
							isFindBall_R = TRUE;
						}

					}

				}
				/*if (largest_area_R == 0) {
					is_SearchWindowGet_R = NULL;
					AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"No Ball!");				//20181215
					isFindBall_R = FALSE;
				}*/

				//decide if the contour is a ball or not 
				//if (largest_area_R>300){//it's not ball 
				//	AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"Not a Ball!");
				//	isFindBall_R = FALSE;
				//}

				//else{// there is a ball! 
					// Find the bounding circle for biggest contour
				if (isFindBall_R == TRUE)
				{

					if (is_SearchWindowGet_R) {
						minEnclosingCircle(contours_R[largest_contour_index_R], center_point, radius);

						center_point.x = center_point.x + searchWindow.x;//在 MatImage 中球的位置
						center_point.y = center_point.y + searchWindow.y;

						circle(ContourImage_R, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_R = (int)center_point.x;
						Iy_R = (int)center_point.y;*/
						Ix_R = center_point.x;
						Iy_R = center_point.y;

						radius_R = radius;
						//AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"Got it!");		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						//AfxGetMainWnd()->SetDlgItemTextW(IDC_R_Radius, str);					//20181215
					}
					else {
						minEnclosingCircle(contours_R[largest_contour_index_R], center_point, radius);
						circle(ContourImage_R, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_R = (int)center_point.x;
						Iy_R = (int)center_point.y;*/
						Ix_R = center_point.x;
						Iy_R = center_point.y;
						radius_R = radius;
						//AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"Got it!");		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						//AfxGetMainWnd()->SetDlgItemTextW(IDC_R_Radius, str);					//20181215
					}

					//get the searchWindow for the ball 
					//searchWindow.width = radius * 2 * 3;//球的直徑之三倍
					//searchWindow.height = radius * 2 * 3;
					searchWindow.width = 200;//固定大小，pixels 
					searchWindow.height = 200;

					searchWindow.x = center_point.x - searchWindow.width / 2;
					searchWindow.y = center_point.y - searchWindow.height / 2;

					searchWindow = searchWindow & Rect(0, 0, ContourImage_R.cols, ContourImage_R.rows);// 取交集做越界保護
					rectangle(ContourImage_R, searchWindow, Scalar(255, 255, 255), 2, 8, 0);

					is_SearchWindowGet_R = TRUE;
				}
			}

			else {//there is no contour
				is_SearchWindowGet_R = NULL;
				//AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"No Ball!");				//20181215
				isFindBall_R = FALSE;
			}

			if (is_hsv_tunning == TRUE) {
				imshow("ResultR", ContourImage_R);
				waitKey(1);
				is_SearchWindowGet_R = FALSE;//dont use searchWindow when tunning the hsv
			}
			else {
				destroyWindow("ResultR");
			}

			//reset the flag to get next picture
			Event_Contour_Pic_R.ResetEvent();
			isFindContourReceivingPic_R = 0;

			t2 = GetTickCount() - t1;

			processing_rate_R++;

			//isFindBall_R = FALSE;//給下次判斷是否有找到
		}  //end if

	}//end while 
	pFindContourR = NULL;
	destroyAllWindows();
	return 0;
}


void CdemoDlg::OnBnClickedHsvFilter()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	is_hsv_tunning = TRUE;
	if (pHsvdlg) {
		pHsvdlg->SetForegroundWindow();  //如果視窗已經存在，把視窗顯示到最上層
		//MessageBox(L"Already Exist!");
	}
	else {
		pHsvdlg = new CHsvScrollBar(this);
		pHsvdlg->Create(IDD_HSV_ScrollBar, GetDesktopWindow());
		pHsvdlg->ShowWindow(SW_SHOWNORMAL);
	}
}
