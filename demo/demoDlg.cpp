
// demoDlg.cpp: 實作檔案
//

#include "stdafx.h"
#include "demo.h"
#include "demoDlg.h"
#include "afxdialogex.h"
#include <math.h>
#include "afx.h"
//#include <opencv/highgui.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


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


//***showing frame_rate***//
unsigned int frames_L = 0;
unsigned int frames_R = 0;


//***using for ball trajectory buffer***//

//Point2d* datalist_XY[] = { 0 };
//Point2d* datalist_YZ[] = { 0 };
//Point2d* datalist_XZ[] = { 0 };
unsigned int index_first(0);
unsigned int index_second(0);
CWinThread* CdemoDlg::pTrajectoryPredict_XYZ = NULL;
CEvent Event_TrajectoryPredict_XYZ(FALSE, TRUE); //手動設定模式，初始狀態為FALSE

double array_x[500] = { 0 };
double array_y[500] = { 0 };
double array_z[500] = { 0 };
//double *array_x = new double[500];
//double *array_y = new double[500];
//double *array_z = new double[500];
//*array_x = { 0 };
//*array_y = { 0 };
//*array_z = { 0 };


//***for the three terminal***//
bool is_ShowImg_Receiving_XY(0);
bool is_ShowImg_Receiving_YZ(0);
bool is_ShowImg_Receiving_ZX(0);


//***for show image***///
bool is_ShowImg_Receiving_L = 0;
bool is_ShowImg_Receiving_R = 0;
IplImage * show_imgl = NULL;
IplImage * show_imgr = NULL;
unsigned int ImgShowRate(0);
unsigned int ImgShowRate_R(0);

//***processing HSV***//
//Rect searchWindow_L(0, 0, 0, 0);
//Rect searchWindow_R(0, 0, 0, 0);
//CWinThread* CdemoDlg::pHSVThread_L = NULL;
//CWinThread* CdemoDlg::pHSVThread_R = NULL;

//***for save image***//
IplImage * save_imgl = NULL;
IplImage * save_imgr = NULL;
bool is_save_picL = 0;
bool is_save_picR = 0;
int num = 1;//save picture from pic1,pic2...
char  pic_num[5];
char  filename[20];

////***for setting proccessing roi***//
//bool is_AskingSetRoiImg_L(0);
//bool is_AskingSetRoiImg_R(0);
//Mat setRoiImgL;
//Mat setRoiImgR;
//bool is_RoiHasSet_L(0);
//bool is_RoiHasSet_R(0);
//bool isEnableRoi_L(1);
//bool isEnableRoi_R(1);

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
//UINT hmin = 0, hmax = 30, smin = 100, smax = 256, vmin = 150, vmax = 256;  //initial value for ball color		//232
UINT hmin = 10, hmax = 30, smin = 120, smax = 256, vmin = 60, vmax = 256;  //initial value for ball color

//****be used to findcontour****//
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


//*******for BPnn********//
//double array_netl[3] = { 0 };
//vector<double> coll_data;
//CWinThread* CdemoDlg::pBpnn = NULL;

////***for set roi for image***//
//Rect ROI_Rect_L(0, 0, 0, 0);
//Rect ROI_Rect_R(0, 0, 0, 0);
//Rect TargetRect(0, 0, 0, 0);
//Point firstPoint(0, 0);
//Point secondPoint(0, 0);
//Point Mouse_Point(0, 0);
//Mat ROI;

////***definition stereo param***//
//Stereo Vision definition in stereo.cpp 
mao_style::Stereo cam;

//instrinsic parameters
extern float fu_left, fv_left, u0_left, v0_left;
extern float fu_right, fv_right, u0_right, v0_right;
//iamge correction parameters
extern float kc_left[8];
extern float kc_right[8];
//extrinsic parameters
extern float R_rl[3][3];
extern float b_l[3];

//extrinsic parameters 
//rotation matrix:  world coordinate rotate to left camera coordinate
extern float R_cw[3][3];
//rotation matrix:  left camera coordinate rotate to world coordinate
extern float R_wc[3][3];
//translation vector 
extern float T_c[3];


//***for trajectory predict***//
bool isEnableTrajectory(1);
vector <CBallState> ball_state;
CBallState temp, temp_old;
unsigned int ball_index(0);

unsigned int flying_modal_index(0);
Trajectory_State CdemoDlg::TrajectoryState = Trajectory_State_Start;


double LandingTime = 0;
double LandingPos_x = 0;
double LandingPos_y = 0;
double LandingPos_z = 0;

double tx_a, tx_b, tx_c;
double ty_a, ty_b, ty_c;
double tz_a, tz_b, tz_c;
unsigned int caclulate_points = 5;  //how many points for least squared method					//20190109 改點個數(ori:10)

CWinThread* CdemoDlg::pTrajectoryPredict = NULL;												//先忽略 20181218

ofstream file_measure_points;

ofstream file_polyfit_points;
ofstream file_polyfit_landing_point;

ofstream file_polyfit_coeff_tx;
ofstream file_polyfit_coeff_ty;
ofstream file_polyfit_coeff_tz;

ofstream file_roots;
ofstream file_predict_trajectory_1st;
ofstream file_predict_landing_point;
ofstream file_rebound_modal;
ofstream file_predict_trajectory_2nd;
ofstream file_strike_point;

ofstream file_predict_moving_direction;
ofstream file_nonKalman_landing_point;
ofstream file_Angularvelocity;
ofstream file_A_training_data;
ofstream file_nonKalman_filter;
//ofstream predict_trajectory_1st;
ofstream strike_point;


bool isTimeOut(0);
bool is_getStikePoint(0);
bool is_getAngularvelocity(1);

//***Prediction with Kalman Filter***//
//double gravity = 9.81e3;// (mm/s^2)
//double Kd = 0.18e3;
CEvent Event_PredictTrajectory(FALSE, TRUE); //手動設定模式，初始狀態為FALSE

class CKalmanFilter {
public:
	Matrix<double, 6, 1>X;
	Matrix<double, 6, 6>A;
	Matrix<double, 6, 1>B;

	Matrix<double, 6, 6>P;
	Matrix<double, 6, 6>Q;

	Matrix<double, 3, 1>Z;
	Matrix<double, 6, 1>Zerr;
	Matrix<double, 6, 1>Xerr;
	Matrix<double, 3, 6>H;

	Matrix<double, 6, 3>K;
	Matrix<double, 3, 3>R;
	Matrix<double, 6, 6>I;

	Matrix<double, 3, 3>Ga;
	Matrix<double, 3, 1>Gb;
	Matrix<double, 3, 1>W_t;

	Matrix<double, 6, 1>X_old;
	Matrix<double, 6, 1>X_new;

	double Kd = 0.18e-3; // (1/mm)					//我算完是 Kd = 0.12e-3(Cd=0.4) ~ 0.18e-3(Cd=0.6)  (單位:1/mm)，英文論文的空氣密度是錯的。
	double Km = 7.46848e-3;  // no unit				//考慮球體旋轉的係數項，直接將瑪格努斯力乘進去。 Kd = lo*rb*s*CL;
	double gravity = 9.81e3;// (mm/s^2)				//截面積為0.126e-3 (m^2)、球體流體阻力細數約為0.47	
	double dt;										//->截面積pi*r^2？ = 1.26*10^-3 // ans Cd=0.4 -> km=0.1204 (1/m) = 0.1204e-3 (1/mm) 
	double Vx, Vy, Vz;								//Km = 7.46848e-3 (when 0.6) ~ 2.408e-3 (when 0.2)
	double Wx, Wy, Wz;								//轉速
	double Wx_o, Wy_o, Wz_o;
	double ax, ay, az;								//加速度
	double V, V_t;		// W

	CKalmanFilter();
	void KalmanFilter();
	void UpdateVariables();
	void UpdateVariables_predict();				//new add    20190325
	void UpdateAngularvelocity();
	void UpdateVariables_Angularvelocity();
	void flyingmodel();

};

CKalmanFilter::CKalmanFilter() {
	//initialize 

	I = Matrix<double, 6, 6>::Identity();

	A << 1, 0, 0, dt, 0, 0,
		0, 1, 0, 0, dt, 0,
		0, 0, 1, 0, 0, dt,
		0, 0, 0, (1 - Kd * V * dt), 0, 0,		//(1 - Kd * V * dt) 這樣才對，但聖懋這邊這樣寫沒差，因為下面更新的部分有寫對	//-Km * Wz * dt, Km * Wy * dt
		0, 0, 0, 0, (1 - Kd * V * dt), 0,		//(1 - Kd * V * dt + Km * V * W * dt) 好像不是每項都長這樣		//Km * Wz, -Km * Wx
		0, 0, 0, 0, 0, (1 - Kd * V * dt);		//-Km * Wy, Km * Wx
	
	//A << 1, 0, 0, dt, 0, 0,
	//	0, 1, 0, 0, dt, 0,
	//	0, 0, 1, 0, 0, dt,
	//	0, 0, 0, (1 - Kd * V * dt), -Km * Wz * dt, Km * Wy * dt,		//(1 - Kd * V * dt) 這樣才對，但聖懋這邊這樣寫沒差，因為下面更新的部分有寫對	//-Km * Wz * dt, Km * Wy
	//	0, 0, 0, Km * Wz * dt, (1 - Kd * V * dt), -Km * Wx * dt,		//(1 - Kd * V * dt + Km * V * W * dt) 好像不是每項都長這樣		//Km * Wz, -Km * Wx
	//	0, 0, 0, -Km * Wy * dt, Km * Wx * dt, (1 - Kd * V * dt);

	X << 0, 0, 0, 0, 0, 0;

	B << 0, 0, 0, 0, 0, -dt;
	

	//斜方差矩陣是否需要實驗? P 估測、Q 預測、R 量測，初始給Xo()與Po()  -> 應證誤差矩陣的影響 
	// -> Po其實給多少沒有差別，只要最後會收斂就好。實際影響較大的部分為Q、R矩陣。
	// -> 但Po還是需要給出一定量的誤差，這樣濾波器才會反應。
	// -> 濾波器 hint: lyapunov、riccati equestion。

	P << 10, 0, 0, 0, 0, 0,
		0, 10, 0, 0, 0, 0,
		0, 0, 10, 0, 0, 0,
		0, 0, 0, 10, 0, 0,
		0, 0, 0, 0, 10, 0,
		0, 0, 0, 0, 0, 10;

	Q << 20, 0, 0, 0, 0, 0,
		0, 20, 0, 0, 0, 0,
		0, 0, 20, 0, 0, 0,
		0, 0, 0, 20, 0, 0,
		0, 0, 0, 0, 20, 0,
		0, 0, 0, 0, 0, 20;

	H << 1, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0;

	R << 10, 0, 0,
		0, 10, 0,
		0, 0, 10;

	Z << 0, 0, 0;

	Zerr << 0, 0, 0, 0, 0, 0;

	Xerr << 0, 0, 0, 0, 0, 0;

	Ga << 0, 0, 0,
		0, 0, 0,
		0, 0, 0;

	Gb << 0, 0, 0;

	W_t << 0, 0, 0;
}

void CKalmanFilter::UpdateVariables() {
	//update State Matrix A  (6*6 Matrix)
	A(0, 3) = dt;
	A(1, 4) = dt;
	A(2, 5) = dt;

	A(3, 4) = -Km * Wz * dt;		//更新？感覺只要在預測前算出角速度，當作定值一直代入?
	A(3, 5) = Km * Wy * dt;
	A(4, 3) = Km * Wz * dt;
	A(4, 5) = -Km * Wx * dt;
	A(5, 3) = -Km * Wy * dt;
	A(5, 4) = Km * Wx * dt;

	//V = sqrt(pow(Vx, 2) + pow(Vy, 2) + pow(Vz, 2));
	V = sqrt(pow(X[3], 2) + pow(X[4], 2) + pow(X[5], 2));
	
	A(3, 3) = 1 - Kd * V * dt;
	A(4, 4) = 1 - Kd * V * dt;
	A(5, 5) = 1 - Kd * V * dt;

	//update Control Matrix B  (6*1 Matrix)
	B(5) = -dt;
}

void CKalmanFilter::UpdateVariables_predict() {
	//update State Matrix A  (6*6 Matrix)
	A(0, 3) = dt;
	A(1, 4) = dt;
	A(2, 5) = dt;

	A(3, 4) = -Km * Wz * dt;		
	A(3, 5) = Km * Wy * dt;
	A(4, 3) = Km * Wz * dt;
	A(4, 5) = -Km * Wx * dt;
	A(5, 3) = -Km * Wy * dt;
	A(5, 4) = Km * Wx * dt;

	//V = sqrt(pow(Vx, 2) + pow(Vy, 2) + pow(Vz, 2));
	V = sqrt(pow(X[3], 2) + pow(X[4], 2) + pow(X[5], 2));
	A(3, 3) = 1 - Kd * V * dt;
	A(4, 4) = 1 - Kd * V * dt;
	A(5, 5) = 1 - Kd * V * dt;

	//update Control Matrix B  (6*1 Matrix)
	B(5) = -dt;
}

void CKalmanFilter::KalmanFilter() {
	//update state matrix and control matrix
	UpdateVariables();
	//prediction
	//X_old = X;
	X = A * X + B * gravity;		//X的雜訊項不知道為什麼沒有->	因為是直接看誤差協方差矩陣！
	Zerr = X;
	P = A * P * A.transpose() + Q;
	//update		
	K = P * H.transpose()*(H*P*H.transpose() + R).inverse();	
	//Zerr = (Z - H * X);				//get the error of true and predict position //(Z - H * X)
	X = X + K * (Z - H * X);		//  -> (Z-H*X) 即為實際與預測的位置之誤差，如果預測與實際很準確時，此項就不會有太大的影響。
	//X_new = X;
	P = (I - K * H)*P;
	Xerr = (X - Zerr);
}

void CKalmanFilter::flyingmodel() {
	
	//dt = 0.005;
	Kd = 0.18e-3;	// (1/mm)
	Km = 7.46848e-3;  // no unit;
	gravity = 9.81e3;

	//update state matrix and control matrix
	UpdateVariables_predict();
	X_old = X;
	X = A * X + B * gravity;
	Zerr = X;
}

void CKalmanFilter::UpdateVariables_Angularvelocity() {

	Ga(0, 1) = X[5];
	Ga(0, 2) = -X[4];
	Ga(1, 0) = -X[5];
	Ga(1, 2) = X[3];
	Ga(2, 0) = X[4];
	Ga(2, 1) = -X[3];

	//V = sqrt(pow(X[3], 2) + pow(X[4], 2) + pow(X[5], 2));
	V_t = sqrt(pow(X[3], 2) + pow(X[4], 2) + pow(X[5], 2));

	//Gb(0) = (ax + Kd * V * X[0]);// *dt;
	//Gb(1) = (ay + Kd * V * X[1]);// *dt;
	//Gb(2) = (az + Kd * V * X[2] + gravity);// *dt;

	Gb(0) = (1 / (Km*dt))*(Zerr[3] - X_old[3] + Kd * V_t * X_old[3] * dt);		//預-初始 (Vt+1 - Vt)
	Gb(1) = (1 / (Km*dt))*(Zerr[4] - X_old[4] + Kd * V_t * X_old[4] * dt);
	Gb(2) = (1 / (Km*dt))*(Zerr[5] - X_old[5] + Kd * V_t * X_old[5] * dt + gravity * dt);
}

void CKalmanFilter::UpdateAngularvelocity()
{
	Kd = 0.18e-3;
	Km = 7.46848e-3;
	gravity = 9.81e3;
	UpdateVariables_Angularvelocity();
	//W_t = (1 / Km) * (Ga.transpose() * Ga).inverse() * Ga.transpose() * Gb;
	W_t << 0, 0, 0;
	W_t = (Ga.transpose() * Ga).inverse() * Ga.transpose() * Gb;
}

CKalmanFilter KF;
CKalmanFilter KF_predict; 
CKalmanFilter KF_t;


//***the time when program start***//
DWORD ProgramStartTime;  //setting at OnInitDialog()
double SystemTime;


//some function definition
void CBallState::SetBallState(unsigned int index, double x_value, double y_value, double z_value, double t_value) {

	i = index;
	x = x_value;
	y = y_value;
	z = z_value;
	t = t_value;

}

void CBallState::SetBallState(unsigned int index, double x_value, double y_value, double z_value, double Velocity_x, double Velocity_y, double Velocity_z, double t_value) {

	i = index;
	x = x_value;
	y = y_value;
	z = z_value;
	vx = Velocity_x;
	vy = Velocity_y;
	vz = Velocity_z;
	t = t_value;

}


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
				str2.assign(str, start, end - start);				//assign 分配、配置
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


CdemoDlg::CdemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MainListBox, m_MainListBox);
	DDX_Control(pDX, IDC_MainEditBox, m_MainEditBox);
	DDX_Control(pDX, IDC_TrackingMethod, m_TrackingMethod);
}

BEGIN_MESSAGE_MAP(CdemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CameraL, &CdemoDlg::OnBnClickedCameraL)
	ON_BN_CLICKED(IDC_CameraR, &CdemoDlg::OnBnClickedCameraR)
	ON_BN_CLICKED(IDC_Imgshow, &CdemoDlg::OnBnClickedImgshow)
	ON_WM_TIMER()
	
	ON_CBN_SELCHANGE(IDC_TrackingMethod, &CdemoDlg::OnCbnSelchangeTrackingmethod)
	ON_BN_CLICKED(IDC_HSV_Filter, &CdemoDlg::OnBnClickedHsvFilter)
	ON_BN_CLICKED(IDC_StopCapture, &CdemoDlg::OnBnClickedStopcapture)
	ON_BN_CLICKED(IDC_ListBoxClear, &CdemoDlg::OnBnClickedListboxclear)
	ON_BN_CLICKED(IDC_ShowText, &CdemoDlg::OnBnClickedShowtext)
	//ON_BN_CLICKED(IDC_processing_HSV, &CdemoDlg::OnBnClickedprocessingHsv)
	ON_BN_CLICKED(IDC_open_picture_controll, &CdemoDlg::OnBnClickedopenpicturecontroll)
	ON_BN_CLICKED(IDC_ImgSave, &CdemoDlg::OnBnClickedImgsave)
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
	
	//程式進來的時間
	ProgramStartTime = GetTickCount();
	SetTimer(9, 30, NULL);											//數值30是每0.03秒動一次
	
	//Tracking Method ComboBox設定  //index 即是選單中排列的順序，從0開始
	m_TrackingMethod.AddString(L"None");		//index=0
	m_TrackingMethod.AddString(L"FindContour");	//index=1
	m_TrackingMethod.AddString(L"Hough");		 //index=2
	m_TrackingMethod.SetCurSel(0);										//先忽略20181218

	//Main Edit Box 初始設定
	m_MainEditBox.SetWindowTextW(L"\r\n");
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Initializing!\r\n"));
	m_MainEditBox.ReplaceSel(_T("Finish!\r\n"));

	//m_MainEditBox.SetSel(2, 5);
	SetDlgItemText(IDC_FrameRate_L, L"0");
	SetDlgItemText(IDC_FrameRate_R, L"0");

	/*((CButton*)GetDlgItem(IDC_EnableROI_L))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_EnableROI_R))->SetCheck(1);*/							//先忽略20181218

	//AfxGetMainWnd()->ShowWindow(SW_SHOWMAXIMIZED);

	cam.initialize(R_rl, b_l);
	cam.setCamera(mao_style::LeftCamera, fu_left, fv_left, u0_left, v0_left, kc_left);
	cam.setCamera(mao_style::RightCamera, fu_right, fv_right, u0_right, v0_right, kc_right);

	//((CButton*)GetDlgItem(IDC_EnableTrajectory))->SetCheck(1);
	//((CButton*)GetDlgItem(IDC_ActiveFramesDifference))->SetCheck(1);				//先忽略20181218

	////*** For Arm Tab Control ***///												//先忽略20181218
	//m_TabControl.InsertItem(0, _T("State"));
	//m_TabControl.InsertItem(1, _T("Control"));
	//CRect TabControlRect;
	//m_TabControl.GetClientRect(&TabControlRect);
	//m_TabArmStateDlg.Create(IDD_TabArmState, &m_TabControl);  //create a specify window on m_TabControl
	//m_TabArmStateDlg.SetWindowPos(NULL, 0, 40, TabControlRect.Width(), TabControlRect.Height() - 20, SWP_SHOWWINDOW | SWP_NOZORDER);
	//pShowTabPage = &m_TabArmStateDlg;
	//m_TabArmControlDlg.Create(IDD_TabArmControl, &m_TabControl); //create a specify window on m_TabControl
	//m_TabArmControlDlg.SetWindowPos(NULL, 0, 20, TabControlRect.Width(), TabControlRect.Height() - 20, SWP_SHOWWINDOW | SWP_NOZORDER);
	//m_TabArmControlDlg.ShowWindow(SW_HIDE);  //hide it when initializing				


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

void CdemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	switch (nIDEvent)
	{
		case 1: //timer 1
			SetDlgItemInt(IDC_FrameRate_L, frames_L, false);
			frames_L = 0;	
			break;
		case 2:
			SetDlgItemInt(IDC_FrameRate_R, frames_R, false);
			frames_R = 0;
			break;
		case 3:
			SetDlgItemInt(IDC_ProcessingRate, processing_rate, false);
			processing_rate = 0;
			break;
		case 4:
			SetDlgItemInt(IDC_ProcessingRate_R, processing_rate_R, false);
			processing_rate_R = 0;
			break;
		case 5:
			SetDlgItemInt(IDC_ImgShowRate, ImgShowRate, false);
			ImgShowRate = 0;
			break;
		case 6:
			SetDlgItemInt(IDC_ImgShowRate_R, ImgShowRate_R, false);
			ImgShowRate_R = 0;
			break;
		case 7:
																					//先忽略20181218
			/*if (Maxon.m_oUpdateActive && !Maxon.UpdateStatus())
			{
				Maxon.StopTimer();
			}	*/	

			/*if (Maxon1.m_oUpdateActive && !Maxon1.UpdateStatus())
			{
				Maxon1.StopTimer();
			}*/

			break;
		case 8: 
		{																				
			isTimeOut = 1;
			ShowText("TimeOut");
			m_MainEditBox.SetWindowTextW(L"TimeOut");

		}
		case 9:
		{
			SystemTime = (double)(GetTickCount() - ProgramStartTime) / 1000;
			CString msg = L"";
			msg.Format(_T("%.3f"), SystemTime);
			SetDlgItemText(IDC_SystemTime, msg);
			break;
		}
		case 10:
		{
			string str;
			stringstream ss;
			CString cstr;

			if (isFindBall_L) {
				/*cam.Camera_left.Idx = Ix_L;
				cam.Camera_left.Idy = Iy_L;*/

				//need offset cause i've set roi in camera capture
				cam.Camera_left.Idx = Ix_L + LeftCameraoffsetX;
				cam.Camera_left.Idy = Iy_L + LeftCameraoffsetY;

				cam.Camera_left.modify_modal();
				AfxGetApp()->m_pMainWnd->SetDlgItemInt(IDC_L_Ix, cam.Camera_left.Ix - LeftCameraoffsetX);
				AfxGetApp()->m_pMainWnd->SetDlgItemInt(IDC_L_Iy, cam.Camera_left.Iy - LeftCameraoffsetY);

			}

			if (isFindBall_R) {
				/*cam.Camera_right.Idx = Ix_R;
				cam.Camera_right.Idy = Iy_R;*/

				//need offset cause i've set roi in camera capture
				cam.Camera_right.Idx = Ix_R + RightCameraoffsetX;
				cam.Camera_right.Idy = Iy_R + RightCameraoffsetY;

				cam.Camera_right.modify_modal();
				AfxGetApp()->m_pMainWnd->SetDlgItemInt(IDC_R_Ix, cam.Camera_right.Ix - RightCameraoffsetX);
				AfxGetApp()->m_pMainWnd->SetDlgItemInt(IDC_R_Iy, cam.Camera_right.Iy - RightCameraoffsetY);

			}

			if (isFindBall_R && isFindBall_L) {

				
				Event_PredictTrajectory.SetEvent();
				ball_index++;
				
				cam.NON_SSG();

				//transform from left camera to world coordinate! 
				X = R_wc[0][0] * (cam.Camera_left.h_x - T_c[0]) + R_wc[0][1] * (cam.Camera_left.h_y - T_c[1]) + R_wc[0][2] * (cam.Camera_left.h_z - T_c[2]);
				Y = R_wc[1][0] * (cam.Camera_left.h_x - T_c[0]) + R_wc[1][1] * (cam.Camera_left.h_y - T_c[1]) + R_wc[1][2] * (cam.Camera_left.h_z - T_c[2]);
				Z = R_wc[2][0] * (cam.Camera_left.h_x - T_c[0]) + R_wc[2][1] * (cam.Camera_left.h_y - T_c[1]) + R_wc[2][2] * (cam.Camera_left.h_z - T_c[2]);

				CString msg = L"";
				msg.Format(_T("%.3f"), cam.Camera_left.h_x);
				SetDlgItemText(IDC_L_hx, msg);
				msg.Format(_T("%.3f"), cam.Camera_left.h_y);
				SetDlgItemText(IDC_L_hy, msg);
				msg.Format(_T("%.3f"), cam.Camera_left.h_z);
				SetDlgItemText(IDC_L_hz, msg);

				msg.Format(_T("%.3f"), X);
				SetDlgItemText(IDC_W_x, msg);
				msg.Format(_T("%.3f"), Y);
				SetDlgItemText(IDC_W_y, msg);
				msg.Format(_T("%.3f"), Z);
				SetDlgItemText(IDC_W_z, msg);
			
				//先忽略20181218
				temp.SetBallState(ball_index, X, Y, Z, (double)(GetTickCount() - ProgramStartTime) / 1000);  //seconds 
				/*ball_state.push_back(temp);*/
				file_measure_points << ball_index << "," << X << "," << Y << "," << Z << "," << (double)(GetTickCount() - ProgramStartTime) / 1000 << endl;  //seconds

				/*ss.clear();														//此段用於輸出文字
				ss << "Index " << ball_index << " \r\n "
					<< "X " << X << " "
					<< "Y " << Y << " "
					 "Z " << Z << " \r\n ";
				ShowText(ss.str());*/

																		//先忽略20190109
				//y 在攝影機側的桌子，就不算軌跡了						//現在進1000內的那一點會有存取的問題?感覺是break後第二次來不及算，所以存取一樣的數據?
				if ( Y < 800 ) {
					break;
					//TrajectoryState = Trajectory_State_Off;	// Trajectory_State_Start
					//ball_state.clear();//clear ball state buffer									
				}

				//if ball position is same as previous, we don't take it into calculate trajectory .
				//timer is slower than processing ,so it's not the data we want 
				if (temp.GetPosition_x() == temp_old.GetPosition_x() && temp.GetPosition_y() == temp_old.GetPosition_y() && temp.GetPosition_z() == temp_old.GetPosition_z()) {
					m_MainEditBox.SetWindowTextW(L"same point!");
					break;
				}
				if (ball_state.size() != 0 && (temp.GetPosition_y()- temp_old.GetPosition_y()) > 30) {				//20190121 向後的球狀態 && Y > 800
					m_MainEditBox.SetWindowTextW(L"backward!");		//ori:	 ball_state.size() != 0 && temp.GetPosition_y() > temp_old.GetPosition_y() && Y > 800
					//(temp.GetPosition_y() - temp_old.GetPosition_y()) > 100
					ball_state.clear();
										
					if (Y > 1500) {
						TrajectoryState = Trajectory_State_Start;
						is_getStikePoint = 0;
						index_first = 0;
						index_second = 0;
						/*Point2d* datalist_XY[] = { 0 };
						Point2d* datalist_YZ[] = { 0 };
						Point2d* datalist_XZ[] = { 0 };*/
						array_x[500] = { 0 };
						array_y[500] = { 0 };
						array_z[500] = { 0 };
					}
					break;
				}				
				/*if (ball_state.size() != 0 && (temp.GetPosition_y() - temp_old.GetPosition_y() > 50) && Y >1000) {
					TrajectoryState = Trajectory_State_Start;
					is_getAngularvelocity = 1;
					is_getStikePoint = 0;
					index_first = 0;
					index_second = 0;
					Point2d* datalist_XY[200] = { 0 };
					Point2d* datalist_YZ[200] = { 0 };
					Point2d* datalist_XZ[200] = { 0 };
					break;
				}*/
				else {  //add ball_state	
					ball_state.push_back(temp);				//push_back 自動配置記憶體；好像用reserve()、resize() 或 constructor 引數比較好?
					temp_old.SetBallState(ball_index, X, Y, Z, (double)(GetTickCount() - ProgramStartTime) / 1000); //save the last value			//最後一刻的資訊?
					
					ss.clear();		//顯示球狀態
					ss.str("");
					ss << "\r\n" << "Index " << ball_index << "\r\n"
						<< "X " << X << " "
						<< "Y " << Y << " "
						<< "Z " << Z << "\r\n";
					str = ss.str();
					ShowText(ss.str());

					cstr.Format(L"ballstate size: %d", ball_state.size());
					m_MainEditBox.SetWindowTextW(cstr);
				}

				//predict trajectory									
				//unsigned int caclulate_points = 10;  //how many points for least squared method
				if (0) {	//isEnableTrajectory
					if (ball_state.size() > caclulate_points) { //erase the first element,maintain only ten sets of data in vector 
						ball_state.erase(ball_state.begin());	//or erase a range of elements ([first,last)) => ball_state.erase(ball_state.begin(),ball_state.begin()+3);
					}

					if (ball_state.size() == caclulate_points) {  //if ten sets of data, analyze the trajectory of the ball
						m_MainEditBox.SetWindowTextW(L"4 points, calculate coefficients!");

						LstSquQuadRegr Polynominal_tx;//class for calculate quadratic polynominal regression
						LstSquQuadRegr Polynominal_ty;
						LstSquQuadRegr Polynominal_tz;

						//add the data
						for (unsigned int i = 0; i < caclulate_points; i++) { //add 10 data sets 
							Polynominal_tx.AddPoints(ball_state[i].GetTimestamp_t(), ball_state[i].GetPosition_x());  //(t,x)
							Polynominal_ty.AddPoints(ball_state[i].GetTimestamp_t(), ball_state[i].GetPosition_y());  //(t,y)
							Polynominal_tz.AddPoints(ball_state[i].GetTimestamp_t(), ball_state[i].GetPosition_z());  //(t,z)
							file_polyfit_points << ball_state[i].GetIndex() << " "
								<< ball_state[i].GetPosition_x() << " "
								<< ball_state[i].GetPosition_y() << " "
								<< ball_state[i].GetPosition_z() << " "
								<< ball_state[i].GetTimestamp_t() << endl;
						}
						file_polyfit_points << endl;

						Polynominal_tx.CalculateCoefficient();  //calculate polynominal coefficient			//好像用不到了?
						Polynominal_ty.CalculateCoefficient();
						Polynominal_tz.CalculateCoefficient();

						tx_a = (double)Polynominal_tx.GetCoefficient().a;
						tx_b = (double)Polynominal_tx.GetCoefficient().b;
						tx_c = (double)Polynominal_tx.GetCoefficient().c;

						ty_a = (double)Polynominal_ty.GetCoefficient().a;
						ty_b = (double)Polynominal_ty.GetCoefficient().b;
						ty_c = (double)Polynominal_ty.GetCoefficient().c;

						tz_a = (double)Polynominal_tz.GetCoefficient().a;
						tz_b = (double)Polynominal_tz.GetCoefficient().b;
						tz_c = (double)Polynominal_tz.GetCoefficient().c;


						ss.clear();
						ss.str("");
						ss //<< "Index " << ball_index << " \r\n "  
							<< "a " << Polynominal_tz.GetCoefficient().a << " "
							<< "b " << Polynominal_tz.GetCoefficient().b << " "
							<< "c " << Polynominal_tz.GetCoefficient().c;
						str = ss.str();
						ShowText(ss.str());

						LandingTime = 0;
						LandingPos_x = 0;
						LandingPos_y = 0;
						LandingPos_z = 0;

						bool is_FindRoots = Polynominal_tz.CalcQuadraticRoot();	//Polynominal_tz //如果不用曲線擬合，那我這邊就不能用二次多項式的判斷式 b^2-4*a*c
					
						if (is_FindRoots) { //if there is roots i want. (roots is the landing time)
							
							m_MainEditBox.SetWindowTextW(L"find roots!");

							//which y is smaller means it is closer to robot arm is our landing point 
							if ((double)Polynominal_ty.getPredictedY(Polynominal_tz.root1)					//如果前面不用，後面的landingTime不知道怎麼給?
								< (double)Polynominal_ty.getPredictedY(Polynominal_tz.root2))				//也沒有判斷式就不能判斷LandingTime、root
							{
								LandingTime = (double)Polynominal_tz.root1;

							}
							else {
								LandingTime = (double)Polynominal_tz.root2;
							}

							LandingPos_x = (double)Polynominal_tx.getPredictedY(LandingTime);
							LandingPos_y = (double)Polynominal_ty.getPredictedY(LandingTime);
							LandingPos_z = (double)Polynominal_tz.getPredictedY(LandingTime);

							ss.clear();
							ss.str("");
							ss << "root1  " << Polynominal_tz.root1	<< "   root2  " << Polynominal_tz.root2 << "\r\n"
								<< "LandPos_x  " << LandingPos_x << "  "
								<< "LandPos_y  " << LandingPos_y << "  "
								<< "LandPos_z  " << LandingPos_z << "\r\n"
								<< "LandTime" << LandingTime << "  after " << (LandingTime - ball_state[9].GetTimestamp_t()) << " seconds" << "\r\n";    // ball_state[9].GetTimestamp_t()第10顆那一刻
							str = ss.str();
							ShowText(str);

							file_roots << Polynominal_tz.root1 << " " << Polynominal_tz.root2 << endl;

							file_polyfit_landing_point
								<< ball_index << " "
								<< LandingPos_x << " "
								<< LandingPos_y << " "
								<< LandingPos_z << " "
								<< LandingTime << endl;

						}

						if (is_FindRoots && LandingPos_y < 1000) { //the ball is coming  //&& LandingPos_y < 1000    // 2019/01/09 先拿掉
							if (LandingPos_y > 0 && LandingPos_x > 0 && LandingPos_x < 1525) { //the ball is in the hitting region																

								file_polyfit_coeff_tx
									<< ball_index << " "
									<< Polynominal_tx.GetCoefficient().a << " "
									<< Polynominal_tx.GetCoefficient().b << " "
									<< Polynominal_tx.GetCoefficient().c << endl;
								file_polyfit_coeff_ty
									<< ball_index << " "
									<< Polynominal_ty.GetCoefficient().a << " "
									<< Polynominal_ty.GetCoefficient().b << " "
									<< Polynominal_ty.GetCoefficient().c << endl;
								file_polyfit_coeff_tz
									<< ball_index << " "
									<< Polynominal_tz.GetCoefficient().a << " "
									<< Polynominal_tz.GetCoefficient().b << " "
									<< Polynominal_tz.GetCoefficient().c << endl;

								if (!pTrajectoryPredict) {
									pTrajectoryPredict = AfxBeginThread(TrajectoryPredict, NULL);
								}

								ss.clear();
								ss.str("");
								ss << "Ball Is Coming!!!" << "\r\n";

								ShowText(ss.str());
								GetDlgItem(IDC_MainEditBox)->SetWindowTextW(L"ball is coming!!!!!!");

							}
						}
					}
				}	  //isEnableTrajectory

				if (isEnableTrajectory && TrajectoryState == Trajectory_State_Start)	//isEnableTrajectory && TrajectoryState == Trajectory_State_Start
				{
					if (ball_state.size() > caclulate_points) { //erase the first element,maintain only ten sets of data in vector 
						ball_state.erase(ball_state.begin());	//or erase a range of elements ([first,last)) => ball_state.erase(ball_state.begin(),ball_state.begin()+3);
					}
					if (ball_state.size() == caclulate_points ) {  //if ten sets of data, analyze the trajectory of the ball
						m_MainEditBox.SetWindowTextW(L"5 points, calculate coefficients!");

						double x_1, y_1, z_1;			
						double x_0, y_0, z_0, vx_0, vy_0, vz_0;
						double time_P, time_o_P;
						

						//time_o = ball_state[caclulate_points - 3].GetTimestamp_t();						
						time_P = ball_state[caclulate_points - 1].GetTimestamp_t();
						//time_o_P = ball_state[caclulate_points - 2].GetTimestamp_t();
						time_o_P =(ball_state[caclulate_points - 5].GetTimestamp_t() + ball_state[caclulate_points - 4].GetTimestamp_t() + ball_state[caclulate_points - 3].GetTimestamp_t() + ball_state[caclulate_points - 2].GetTimestamp_t() + ball_state[caclulate_points - 1].GetTimestamp_t()) / 5;

						//the position of initinal_point
						x_0 = (ball_state[caclulate_points - 5].GetPosition_x() + ball_state[caclulate_points - 4].GetPosition_x() + ball_state[caclulate_points - 3].GetPosition_x() + ball_state[caclulate_points - 2].GetPosition_x() + ball_state[caclulate_points - 1].GetPosition_x()) / 5;
						y_0 = (ball_state[caclulate_points - 5].GetPosition_y() + ball_state[caclulate_points - 4].GetPosition_y() + ball_state[caclulate_points - 3].GetPosition_y() + ball_state[caclulate_points - 2].GetPosition_y() + ball_state[caclulate_points - 1].GetPosition_y()) / 5;
						z_0 = (ball_state[caclulate_points - 5].GetPosition_z() + ball_state[caclulate_points - 4].GetPosition_z() + ball_state[caclulate_points - 3].GetPosition_z() + ball_state[caclulate_points - 2].GetPosition_z() + ball_state[caclulate_points - 1].GetPosition_z()) / 5;
						/*x_0 = ball_state[caclulate_points - 2].GetPosition_x();
						y_0 = ball_state[caclulate_points - 2].GetPosition_y();
						z_0 = ball_state[caclulate_points - 2].GetPosition_z();*/

						//位置的算法要改，不能使用3個做為平均。
						/*x1 = (ball_state[caclulate_points - 3].GetPosition_x() + ball_state[caclulate_points - 2].GetPosition_x() + ball_state[caclulate_points - 1].GetPosition_x()) / 3;
						y1 = (ball_state[caclulate_points - 3].GetPosition_y() + ball_state[caclulate_points - 2].GetPosition_y() + ball_state[caclulate_points - 1].GetPosition_y()) / 3;
						z1 = (ball_state[caclulate_points - 3].GetPosition_z() + ball_state[caclulate_points - 2].GetPosition_z() + ball_state[caclulate_points - 1].GetPosition_z()) / 3;*/
						x_1 = ball_state[caclulate_points - 1].GetPosition_x();
						y_1 = ball_state[caclulate_points - 1].GetPosition_y();
						z_1 = ball_state[caclulate_points - 1].GetPosition_z();

						//the speed of final_point
						vx_0 = (x_1 - x_0) / (time_P - time_o_P);
						vy_0 = (y_1 - y_0) / (time_P - time_o_P);
						vz_0 = (z_1 - z_0) / (time_P - time_o_P);
												
						//ss.clear();														//此段用於輸出文字
						//ss << "Index " << ball_index << " \r\n "
						//	<< "vx0 " << vx0 << " "
						//	<< "vy0 " << vy0 << " "
						//	<< "vz0 " << vz0 << " \r\n ";
						//ShowText(ss.str());

						//Initial State						
						KF_predict.X << x_1, y_1, z_1, vx_0, vy_0, vz_0;
						double test_numble = 0;
						double test_time = 0;
						double need_time = 0;
						
						is_getAngularvelocity = 1;

						while (abs(Z - 0) > 50){
							KF_predict.dt = 0.005;
							KF_predict.flyingmodel();
							test_numble++;
							if (KF_predict.X[2] < 50 )
							{	
								//test_time = test_numble*0.005;
								if (KF_predict.X[1] < 1000) {
									TrajectoryState = Trajectory_State_Off;
								}
								break;
							}
							
							if (is_getAngularvelocity == 1) // is_getAngularvelocity == 1
							{							
								//***********這邊好像可以了*************//
								/*KF_t.X = KF_predict.X;
								KF_t.Zerr = KF_predict.Zerr;
								KF_t.X_old = KF_predict.X_old;
								KF_t.dt = KF_predict.dt;*/

								Vector3d Velocity; //= (KF_predict.Xerr) / (KF_predict.dt);		//Zerr									
								Velocity[0] = (KF_predict.Xerr[0]) / (KF_predict.dt);
								Velocity[1] = (KF_predict.Xerr[1]) / (KF_predict.dt);
								Velocity[2] = (KF_predict.Xerr[2]) / (KF_predict.dt);
								//Vector3d accelerationVelocity = Velocity / (KF.dt);			
								KF_predict.Wx = Velocity[0] / 20;
								KF_predict.Wy = Velocity[1] / 20;
								KF_predict.Wz = Velocity[2] / 20;

								//評估轉速 20190614

								/*KF_predict.UpdateAngularvelocity();
								KF_predict.Wx = KF_predict.W_t[0];
								KF_predict.Wy = KF_predict.W_t[1];
								KF_predict.Wz = KF_predict.W_t[2];*/
								file_nonKalman_filter << ball_index << " " << KF_predict.Wx << " " << KF_predict.Wy << " " << KF_predict.Wz << endl;
							}
							is_getAngularvelocity = 0;
							//ss.clear();														//this's way -> 完整的落點過程
							//ss << "Index " << ball_index << " \r\n "
							//   << "X " << KF.X[0] << " "
							//   << "Y " << KF.X[1] << " "
							//   << "Z " << KF.X[2] << " "
							//   << "Vx " << KF.X[3] << " "
							//   << "Vy " << KF.X[4] << " "
							//   << "Vz " << KF.X[5] << "\r\n ";
							//ShowText(ss.str());
							/*file_nonKalman_landing_point << ball_index++ << " "
								<< "X " << KF.X[0] << " "
								<< "Y " << KF.X[1] << " "
								<< "Z " << KF.X[2] << " "
								<< "Vx " << KF.X[3] << " "
								<< "Vy " << KF.X[4] << " "
								<< "Vz " << KF.X[5] << " "
								<< endl;*/
						}
						//need_time = test_time + time;

						//ss.clear();														//此段用於輸出文字
						//ss << "\r\n" << "Index " << ball_index << " "
						//	<< "X " << KF.X[0] << " "
						//	<< "Y " << KF.X[1] << " "
						//	<< "Z " << KF.X[2] << " "
						//	<< "Vx " << KF.X[3] << " "
						//	<< "Vy " << KF.X[4] << " "
						//	<< "Vz " << KF.X[5] << "\r\n ";
						//ShowText(ss.str());
						
						file_nonKalman_landing_point << ball_index << " "
							<< test_numble << " "
							<< KF_predict.X[0] << " "
							<< KF_predict.X[1] << " "
							<< KF_predict.X[2] << " "
							<< endl;	//need_time <<

						/*if (!pTrajectoryPredict) {
							pTrajectoryPredict = AfxBeginThread(TrajectoryPredict, NULL);
						}*/
						
						double LandingTime_P = 0;
						double LandingPos_x_P = 0;
						double LandingPos_y_P = 0;
						double LandingPos_z_P = 0;
												
						LandingPos_x_P = KF_predict.X[0];
						LandingPos_y_P = KF_predict.X[1];
						LandingPos_z_P = KF_predict.X[2];
						
						if (LandingPos_y_P < 1000) { //the ball is coming  //&& LandingPos_y < 1000    
							if (LandingPos_y_P > 0 && LandingPos_x_P > 0 && LandingPos_x_P < 1525) { //the ball is in the hitting region	
								
								if (!pTrajectoryPredict) {
									pTrajectoryPredict = AfxBeginThread(TrajectoryPredict, NULL);
								}
							}
						}
					}
				}

				//*** Origenal ***//
				//if (ball_state.size() > caclulate_points) { //erase the first element,maintain only ten sets of data in vector 
				//	ball_state.erase(ball_state.begin());	//or erase a range of elements ([first,last)) => ball_state.erase(ball_state.begin(),ball_state.begin()+3);
				//}
				//if (ball_state.size() == caclulate_points) {  //if ten sets of data, analyze the trajectory of the ball
				//	m_MainEditBox.SetWindowTextW(L"4 points, calculate coefficients!");
				//	
				//	if (!pTrajectoryPredict) {
				//		pTrajectoryPredict = AfxBeginThread(TrajectoryPredict, NULL);
				//	}
				//}	

			}//end if 
			else {//one or both of them didn't find ball
				ball_state.clear();								
				ShowText("didn't match!");
			}
		}//end case

	default:
		
		break;
	}//end switch	
	CDialogEx::OnTimer(nIDEvent);

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
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Error: Can't connect to the Camera!", 0, 0);
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
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Camera Setting Error", 0, 0);
	}

	Cam_L.StartCapture();
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 1, 1000, NULL);   //show frame_rate on main window
	//get picture control dc 
	CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	CWnd * pWnd_ImageShow = pMainWnd->GetDlgItem(IDC_PicControl1);
	CDC* dc = pWnd_ImageShow->GetWindowDC();

	//get and show camera info
	stringstream ss;
	string str;
	error = Cam_L.GetCameraInfo(&camInfo);
	ss << " ***Left Camera Information*** " << "\r\n"
		<< " SensorInfo: " << camInfo.sensorInfo << "\r\n"
		<< " Resolution: " << camInfo.sensorResolution << "\r\n"
		<< " SerialNumber: " << camInfo.serialNumber << "\r\n";
	ShowText(ss.str());
		
	//get size of picture control,and prepare a image space for resize image to fit in picture control
	RECT rect;
	pMainWnd->GetDlgItem(IDC_PicControl1)->GetWindowRect(&rect);
	IplImage* Img_L2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
	//Cam_L.SetVideoModeAndFrameRate(VIDEOMODE_1280x960RGB, FRAMERATE_30);//this is standard mode, but what i want is custom mode.
	Image BGR_format;

	camera_state_L = START;

	Mat imageL_t = cvCreateImage(cvSize(512, 384), 8, 3);

	//Video capture loop
	while (camera_state_L == START) {

		Cam_L.RetrieveBuffer(&rawImage_L);
		rawImage_L.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &BGR_format);
		//unsigned int rowBytes0 = (double)BGR_format.GetReceivedDataSize() / (double)BGR_format.GetRows();			//20181220
		imageL = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData(), 0);			//rowBytes0 = 0;	
		//Mat imageL_t = cvCreateImage(cvSize(512, 384), 8, 3);
		/*cv::imshow("image", imageL);
		waitKey(30);*/

		////show left image in picture control 
		//IplImage* Img_L = cvCloneImage(&IplImage(imageL)); //cvvimage function only take IplImage..
		//cvResize(Img_L, Img_L2); //resize the image to fit into the picture control 
		//CvvImage show;
		//show.CopyOf(Img_L2);
		//show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));
																										//先註解掉20181218
		if (is_save_picL) {  //for saving picture if i pressed save button
			save_imgl = cvCloneImage(&IplImage(imageL));
			is_save_picL = 0;
			//cvWaitKey(1);
		}

		if (!is_ShowImg_Receiving_L && pImageShowThread) {
			//cv::resize(imageL, imageL, cv::Size(512, 384));
						
			cvResize(&IplImage(imageL), &IplImage(imageL_t));
			show_imgl = cvCloneImage(&IplImage(imageL_t));

			//show_imgl = cvCloneImage(&IplImage(imageL));			
			//cvCopy(&IplImage(imageL), show_imgl, NULL);
			is_ShowImg_Receiving_L = 1;
		}
		
		/*																						//先註解掉20181218
		if (is_AskingSetRoiImg_L) {
			setRoiImgL = imageL.clone();
			is_AskingSetRoiImg_L = 0;
		}*/

		//for tracking method
		if (MethodChoose != None) {
			switch (MethodChoose)
			{
			case Hough:

				break;
			case FindContour:
				if (!isFindContourReceivingPic_L) {
					//cv::resize(imageL, imageL, cv::Size(100, 100));
					ContourImage_L = imageL.clone();
					//waitKey(1);
					Event_Contour_Pic.SetEvent();
					isFindContourReceivingPic_L = 1;
				}
				break;
			default:
				break;
			}


		}

		
		//cvReleaseImage(&Img_L); //release everytime cause the function cvcloneimage will increase the RAM
		frames_L++;
	}
	//pCameraThread_L = NULL;
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
		::MessageBoxW(AfxGetApp()->m_pMainWnd->m_hWnd, L"Error: Can't connect to the Camera!", 0, 0);
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
		::MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, L"Camera Setting Error", 0, 0);
	}

	//Start Capture
	Cam_R.StartCapture();
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 2, 1000, NULL);   //show frame_rate on main window 
	//get picture control handle 
	CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	CWnd * pWnd_ImageShow = pMainWnd->GetDlgItem(IDC_PicControl2);
	CDC* dc = pWnd_ImageShow->GetWindowDC();

	//get and show camera info
	stringstream ss;
	string str;
	error = Cam_R.GetCameraInfo(&camInfo);
	ss << " ***Right Camera Information*** " << "\r\n"
		<< " SensorInfo: " << camInfo.sensorInfo << "\r\n"
		<< " Resolution: " << camInfo.sensorResolution << "\r\n"
		<< " SerialNumber: " << camInfo.serialNumber << "\r\n";
	ShowText(ss.str());

	//get the size of picture control
	RECT rect;//size of picture control
	pMainWnd->GetDlgItem(IDC_PicControl2)->GetWindowRect(&rect);
	IplImage* Img_R2 = cvCreateImage(cvSize((rect.right - rect.left), (rect.bottom - rect.top)), 8, 3);
	//Cam_R.SetVideoModeAndFrameRate(VIDEOMODE_1280x960Y8, FRAMERATE_60);
	Image BGR_format;

	camera_state_R = START;

	Mat imageR_t = cvCreateImage(cvSize(512, 384), 8, 3);

	//Video capture loop
	while (camera_state_R == START) {
		Cam_R.RetrieveBuffer(&rawImage_R);
		rawImage_R.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &BGR_format);
		//unsigned int rowBytes0 = (double)BGR_format.GetReceivedDataSize() / (double)BGR_format.GetRows();
		imageR = cv::Mat(BGR_format.GetRows(), BGR_format.GetCols(), CV_8UC3, BGR_format.GetData(), 0); //rowBytes0=0
		//Mat imageR_t = cvCreateImage(cvSize(512, 384), 8, 3);
		/*imshow("windowR", imageR);
		cvWaitKey(1);*/
		//show image on picture control 
		//IplImage* Img_R = cvCloneImage(&IplImage(imageR));//Mat to iplimage 
		//cvResize(Img_R, Img_R2); //resize the image to fit into the picture control 
		//CvvImage show;
		//show.CopyOf(Img_R2);
		//show.Show(*dc, 0, 0, (rect.right - rect.left), (rect.bottom - rect.top));

		//for saving picture if i pressed save button							
		if (is_save_picR) {														//先註解掉20181218
			save_imgr = cvCloneImage(&IplImage(imageR));
			is_save_picR = 0;
			//cvWaitKey(1);
		}
		if (!is_ShowImg_Receiving_R && pImageShowThread) {
			//cv::resize(imageR, imageR, cv::Size(512, 384));
						
			cvResize(&IplImage(imageR), &IplImage(imageR_t));
			show_imgr = cvCloneImage(&IplImage(imageR_t));
			
			//show_imgr = cvCloneImage(&IplImage(imageR));
			//cvCopy(&IplImage(imageR), show_imgr, NULL);
			is_ShowImg_Receiving_R = 1;
		}
		/*																			//先註解掉20181218
		if (is_AskingSetRoiImg_R) {
			setRoiImgL = imageR.clone();
			is_AskingSetRoiImg_R = 0;
		}*/

		//for tracking method 
		if (MethodChoose != None) {
			switch (MethodChoose)
			{
			case Hough:

				break;
			case FindContour:
				if (!isFindContourReceivingPic_R) {
					//cv::resize(imageR, imageR, cv::Size(100, 100));
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


		//cvReleaseImage(&Img_R);
		
		frames_R++;

	}
	//pCameraThread_R = NULL;
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

UINT CdemoDlg::ImageShowThread(LPVOID pParam)
{
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 5, 1000, NULL);
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 6, 1000, NULL);
	////get picture control handle 
	CWnd * pWnd_ImageShow_L = AfxGetMainWnd()->GetDlgItem(IDC_PicControl1);
	CWnd * pWnd_ImageShow_R = AfxGetMainWnd()->GetDlgItem(IDC_PicControl2);
	CDC* dc_L = pWnd_ImageShow_L->GetWindowDC();
	CDC* dc_R = pWnd_ImageShow_R->GetWindowDC();

	//get the size of left picture control
	RECT rect_l;//size of picture control
	pWnd_ImageShow_L->GetWindowRect(&rect_l);
	//cv::Size winSize(rect_l.right, rect_l.bottom);
	IplImage* resize_imgl = cvCreateImage(cvSize((rect_l.right - rect_l.left), (rect_l.bottom - rect_l.top)), 8, 3);
	//IplImage* resize_imgl = cvCreateImage(cvSize(512, 384), 8, 3);

	//get the size of right picture control
	RECT rect_r;//size of picture control
	pWnd_ImageShow_R->GetWindowRect(&rect_r);
	//Mat resize_imgr(cv::Size(rect_r.right - rect_r.left), (rect_r.bottom - rect_r.top),8,3);
	IplImage* resize_imgr = cvCreateImage(cvSize((rect_r.right - rect_r.left), (rect_r.bottom - rect_r.top)), 8, 3);
	//resize_imgr = cvCreateImage(cvSize(512, 384), 8, 3);
	//IplImage* resize_imgr_x = cvCreateImage(cvSize(512, 384), 8, 3);
	//cvResize(resize_imgr, resize_imgr_x);

	CvvImage showl, showr;

	while (camera_state_L == START || camera_state_R == START) {

		Sleep(0);//avoid busy loop

		if (is_ShowImg_Receiving_L) {//show left 
			ImgShowRate++;
			if (isFindBall_L) {  //if find the ball,then show it! 
				//circle the ball here								
				//cvCircle(show_imgl, cvPoint(Ix_L, Iy_L), radius_L, Scalar(0, 0, 255), 3, 8, 0); //ix iy radius 為共享 注意thread-safe.
				cvCircle(show_imgl, cvPoint(Ix_L / 4, Iy_L / 4), radius_L / 4, Scalar(0, 0, 255), 1, 8, 0);
			}
			//if (0)
			//{
			//	cvLine(show_imgl, cvPoint(LeftCameraWidth / 2, 0), cvPoint(LeftCameraWidth / 2, LeftCameraHeight), Scalar(0, 0, 255), 1, 8, 0);
			//	cvLine(show_imgl, cvPoint(0, LeftCameraHeight / 2), cvPoint(LeftCameraWidth, LeftCameraHeight / 2), Scalar(0, 0, 255), 1, 8, 0);
			//}																	//右眼標示十字				
			//IplImage *img2 = show_imgl;
			//Mat mat2 = cvarrToMat(img2);
			////cv::Mat mat2(img2, 0);
			//namedWindow("ResultL", 0);
			////cvNamedWindow("ResultL",1);
			//imshow("ResultL", mat2);
			//waitKey(0);//延遲，建議參數時間延長或者設置為0讓圖片繪製完畢。0的意思為等待使用者按下任意鍵才往下執行
			//destroyWindow("ResultL");
																				// 此段使用於上述標註影像十字時

			//resize the image to fit into the picture control 
			cvResize(show_imgl, resize_imgl);
			//show left image in picture control 
			showl.CopyOf(resize_imgl);
			showl.Show(*dc_L, 0, 0, (rect_l.right - rect_l.left), (rect_l.bottom - rect_l.top));

			/*sprintf(pic_num, "%03d", num++);
			strcpy(filename, "Pic_L");
			strcat(filename, pic_num);
			strcat(filename, ".jpg");					
			cvSaveImage(filename, save_imgl);
			cvReleaseImage(&save_imgl);*/

			//release image and ask for picture again
			cvReleaseImage(&show_imgl);
			is_ShowImg_Receiving_L = 0;
		}

		if (is_ShowImg_Receiving_R) {//show right camera
			ImgShowRate_R++;
			if (isFindBall_R) {
				//circle the ball here 
				//cvCircle(show_imgr, cvPoint(Ix_R, Iy_R), radius_R, Scalar(0, 0, 255), 3, 8, 0);
				cvCircle(show_imgr, cvPoint(Ix_R / 4, Iy_R / 4), radius_R / 4, Scalar(0, 0, 255), 1, 8, 0);
			}
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
			//or
			//IplImage* img = cvLoadImage("greatwave.png", 1);
			//Mat mtx(img); // convert IplImage* -> Mat

			//resize the image to fit into the picture control 
			cvResize(show_imgr, resize_imgr);
			//show right image in picture control 
			showr.CopyOf(resize_imgr);
			showr.Show(*dc_R, 0, 0, (rect_r.right - rect_r.left), (rect_r.bottom - rect_r.top));

			/*sprintf(pic_num, "%03d", num++);
			strcpy(filename, "Pic_R");
			strcat(filename, pic_num);
			strcat(filename, ".jpg");
			cvSaveImage(filename, save_imgr);
			cvReleaseImage(&save_imgr);*/

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
	Rect searchWindow_L(0, 0, 0, 0);
	//Mat lastImg;

	Point2f center_point;
	float radius;

	//DWORD t1, t2;

	while (find_contour_state == FindContourStart) {

		//if (WaitForSingleObject(Event_Contour_Pic_R, 0) == WAIT_OBJECT_0){//wait until motivate the event (get the picture)
		if (isFindContourReceivingPic_L == 1) {//wait until motivate the event (get the picture)
			//t1 = GetTickCount();

			//if (!pHSVThread_L) {
			//	pHSVThread_L = AfxBeginThread(HSVThread_L, NULL);
			//}

			if (is_SearchWindowGet_L) {
				searchImg = ContourImage_L(searchWindow_L);
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

			//Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(5, 5));
			////erode(mask_L, mask_L, erodeStruct);		//侵蝕 、Mat()
			////dilate(mask_L, mask_L, erodeStruct);	//膨脹
			//morphologyEx(mask_L, mask_L, MORPH_OPEN, erodeStruct);
			morphologyEx(mask_L, mask_L, MORPH_OPEN, Mat());
			
			if (is_hsv_tunning == TRUE) {
				namedWindow("mask_L", 0);
				imshow("mask_L", mask_L);
				waitKey(1);
			}
			else {
				destroyWindow("mask_L");
			}

			int largest_area_L = 0;
			int largest_contour_index_L = 0;
			/*Rect bounding_rect_L;*/
			vector<vector<Point>> contours_L; // Vector for storing contour
			vector<Vec4i> hierarchy_L;
				

			findContours(mask_L, contours_L, hierarchy_L, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
			//***這一段執行需30ms?***//
			if (contours_L.size() > 0) { //there is some contour								
				// iterate through each contour.
				for (unsigned int i = 0; i < contours_L.size(); i++)
				{
					//  Find the area of contour
					double a = contourArea(contours_L[i], false);
					/*stringstream ss;
					ss << "area_L: " << a ;
					ShowText(ss.str());*/

					// Store the index of largest contour
					if (a >= 150 && a <= 1500) {								//30-420		//40-4000
						minEnclosingCircle(contours_L[i], center_point, radius);
						/*stringstream ss;
						ss << "radius_L: " << radius;
						ShowText(ss.str());*/
						if ( 9 <= radius && radius <= 31) {					//4-15	//8-40							
							if (a > largest_area_L) {
								largest_area_L = a;
								largest_contour_index_L = i;
							}
							isFindBall_L = TRUE;
						}
					}
				}
				//***//
				if (largest_area_L == 0) {													//20181215
					is_SearchWindowGet_L = NULL;
					AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"No Ball!");
					isFindBall_L = FALSE;
				}

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

						center_point.x = center_point.x + searchWindow_L.x;//在 MatImage 中球的位置
						center_point.y = center_point.y + searchWindow_L.y;

						circle(ContourImage_L, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_L = (int)center_point.x;
						Iy_L = (int)center_point.y;*/

						Ix_L = center_point.x;
						Iy_L = center_point.y;
						radius_L = radius;
						AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"Got it!");		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						AfxGetMainWnd()->SetDlgItemTextW(IDC_L_Radius, str);				//20181215
						/*str.Format(_T("%.3f"), radius_L);
						SetDlgItemText(IDC_L_Radius, str);*/
					}
					else {
						minEnclosingCircle(contours_L[largest_contour_index_L], center_point, radius);
						circle(ContourImage_L, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_L = (int)center_point.x;
						Iy_L = (int)center_point.y;*/

						Ix_L = center_point.x;
						Iy_L = center_point.y;

						radius_L = radius;
						AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"Got it!");		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						AfxGetMainWnd()->SetDlgItemTextW(IDC_L_Radius, str);				//20181215
					}

					//get the searchWindow for the ball 
					//searchWindow.width = radius * 2 * 3;//球的直徑之三倍
					//searchWindow.height = radius * 2 * 3;
					searchWindow_L.width = 200;//固定大小，pixels 
					searchWindow_L.height = 200;

					searchWindow_L.x = center_point.x - searchWindow_L.width / 2;
					searchWindow_L.y = center_point.y - searchWindow_L.height / 2;

					searchWindow_L = searchWindow_L & Rect(0, 0, ContourImage_L.cols, ContourImage_L.rows);// 取交集做越界保護
					rectangle(ContourImage_L, searchWindow_L, Scalar(255, 255, 255), 2, 8, 0);		//框球

					is_SearchWindowGet_L = TRUE;
				}

			}
			else {//there is no contour
				is_SearchWindowGet_L = NULL;
				AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_L, L"No Ball!");					//20181215
				isFindBall_L = FALSE;
			}

			if (is_hsv_tunning == TRUE) {
				namedWindow("ResultL", 0);
				imshow("ResultL", ContourImage_L);
				waitKey(1);
				is_SearchWindowGet_L = FALSE;//dont use searchWindow when tunning the hsv
			}
			else {
				destroyWindow("ResultL");
			}

			//reset the flag to get next picture
			Event_Contour_Pic.ResetEvent();
			isFindContourReceivingPic_L = 0;

			//t2 = GetTickCount() - t1;

			processing_rate++;

			//isFindBall_R = FALSE;//給下次判斷是否有找到
		}  //end if

	}//end while 
	pFindContour = NULL;
	cv::destroyAllWindows();
	return 0;
}

UINT CdemoDlg::FindContourThreadR(LPVOID pParam) {
	::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 4, 1000, NULL);   //show frame_rate on main window

	Mat searchImg;
	Rect searchWindow_R(0, 0, 0, 0);

	Point2f center_point;
	float radius;

	//DWORD t1, t2;

	while (find_contour_state_R == FindContourStart) {

		//if (WaitForSingleObject(Event_Contour_Pic_R, 0) == WAIT_OBJECT_0){//wait until motivate the event (get the picture)
		if (isFindContourReceivingPic_R == 1) {//wait until motivate the event (get the picture)
			//t1 = GetTickCount();

			if (is_SearchWindowGet_R) {
				searchImg = ContourImage_R(searchWindow_R);
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

			//Mat erodeStruct = getStructuringElement(MORPH_RECT, Size(5, 5));
			////erode(mask_R, mask_R, erodeStruct);		//Mat()
			////dilate(mask_R, mask_R, erodeStruct);	//膨脹
			//morphologyEx(mask_R, mask_R, MORPH_OPEN, erodeStruct);
			morphologyEx(mask_R, mask_R, MORPH_OPEN, Mat());

			if (is_hsv_tunning == TRUE) {
				namedWindow("mask_R", 0);
				imshow("mask_R", mask_R);
				waitKey(1);
			}
			else {
				destroyWindow("mask_R");
			}

			//if (!pHSVThread_R) {
			//	pHSVThread_R = AfxBeginThread(HSVThread_R, NULL);
			//}

			int largest_area_R = 0;
			int largest_contour_index_R = 0;
			/*Rect bounding_rect_R;*/
			vector<vector<Point>> contours_R; // Vector for storing contour
			vector<Vec4i> hierarchy_R;

			findContours(mask_R, contours_R, hierarchy_R, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

			if (contours_R.size() > 0) { //there is some contour
				// iterate through each contour.
				for (unsigned int i = 0; i < contours_R.size(); i++)
				{
					//  Find the area of contour
					double a = contourArea(contours_R[i], false);
					/*stringstream ss;
					ss << "area_R: " << a ;
					ShowText(ss.str());*/

					// Store the index of largest contour
					if (a >= 150 && a <= 1500) {					//30-420		//最遠圓面積約900，但輪廓不完全時最小約100左右
						minEnclosingCircle(contours_R[i], center_point, radius);
						/*stringstream ss;
						ss << "radius_R: " << radius;
						ShowText(ss.str());*/
						if (9 <= radius && radius <= 31) {	//8-30		//4-15	//8-40		//最遠圓半徑約10，但輪廓不完全時最小約4-5左右							
							if (a > largest_area_R) {
								largest_area_R = a;
								largest_contour_index_R = i;
							}
							isFindBall_R = TRUE;
						}

					}

				}
				if (largest_area_R == 0) {
					is_SearchWindowGet_R = NULL;
					AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"No Ball!");				//20181215
					isFindBall_R = FALSE;
				}

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

						center_point.x = center_point.x + searchWindow_R.x;//在 MatImage 中球的位置
						center_point.y = center_point.y + searchWindow_R.y;

						circle(ContourImage_R, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_R = (int)center_point.x;
						Iy_R = (int)center_point.y;*/
						Ix_R = center_point.x;
						Iy_R = center_point.y;

						radius_R = radius;
						AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"Got it!");		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						AfxGetMainWnd()->SetDlgItemTextW(IDC_R_Radius, str);					//20181215
					}
					else {
						minEnclosingCircle(contours_R[largest_contour_index_R], center_point, radius);
						circle(ContourImage_R, center_point, radius, Scalar(255, 255, 0), 2, 8, 0);

						/*Ix_R = (int)center_point.x;
						Iy_R = (int)center_point.y;*/
						Ix_R = center_point.x;
						Iy_R = center_point.y;
						radius_R = radius;
						AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"Got it!");		//20181215

						CString str;
						str.Format(L"%.2f", radius);
						AfxGetMainWnd()->SetDlgItemTextW(IDC_R_Radius, str);					//20181215
					}

					//get the searchWindow for the ball 
					//searchWindow.width = radius * 2 * 3;//球的直徑之三倍
					//searchWindow.height = radius * 2 * 3;
					searchWindow_R.width = 200;//固定大小，pixels 
					searchWindow_R.height = 200;

					searchWindow_R.x = center_point.x - searchWindow_R.width / 2;
					searchWindow_R.y = center_point.y - searchWindow_R.height / 2;

					searchWindow_R = searchWindow_R & Rect(0, 0, ContourImage_R.cols, ContourImage_R.rows);// 取交集做越界保護
					rectangle(ContourImage_R, searchWindow_R, Scalar(255, 255, 255), 2, 8, 0);

					is_SearchWindowGet_R = TRUE;
				}
			}

			else {//there is no contour
				is_SearchWindowGet_R = NULL;
				AfxGetMainWnd()->SetDlgItemTextW(IDC_BallState_R, L"No Ball!");				//20181215
				isFindBall_R = FALSE;
			}

			if (is_hsv_tunning == TRUE) {
				namedWindow("ResultR", 0);
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

			//t2 = GetTickCount() - t1;

			processing_rate_R++;

			//isFindBall_R = FALSE;//給下次判斷是否有找到
		}  //end if

	}//end while 
	pFindContourR = NULL;
	destroyAllWindows();
	return 0;
}

//UINT CdemoDlg::HSVThread_L(LPVOID pParam) {
//	Mat searchImg;
//	Rect searchWindow_L(0, 0, 0, 0);
//	
//	while (camera_state_L == START) {
//		
//		if (is_SearchWindowGet_L) {
//			searchImg = ContourImage_L(searchWindow_L);
//			cvtColor(searchImg, hsv_L, CV_BGR2HSV);	//轉換色彩空間
//		}
//		else {
//			cvtColor(ContourImage_L, hsv_L, CV_BGR2HSV);	//轉換色彩空間
//		}
//
//		if (hmax <= 180) {
//			inRange(hsv_L, Scalar(MIN(hmax, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),
//				Scalar(MAX(hmax, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_L);
//		}
//		else {//special situation hue:180~210 
//
//			inRange(hsv_L, Scalar(MIN(180, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),      //whether if hue between : hmin ~ 180
//				Scalar(MAX(180, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_L);
//
//			// hmax - 180 means we want 180~210 stands for 0~30 in hue value 
//			inRange(hsv_L, Scalar(MIN(hmax - 180, 0), MIN(smax, smin), MIN(vmax, vmin), 0),   //whether if hue between : 0 ~ (hmax-180)
//				Scalar(MAX(hmax - 180, 0), MAX(smax, smin), MAX(vmax, vmin), 0), mask1_L);
//			add(mask_L, mask1_L, mask_L);
//		}
//
//
//		if (is_hsv_tunning == TRUE) {
//			namedWindow("mask_L", 0);
//			imshow("mask_L", mask_L);
//			waitKey(1);
//		}
//		else {
//			destroyWindow("mask_L");
//		}
//
//		if (is_hsv_tunning == TRUE) {
//			namedWindow("ResultR", 0);
//			imshow("ResultR", ContourImage_R);
//			waitKey(1);
//			is_SearchWindowGet_R = FALSE;//dont use searchWindow when tunning the hsv
//		}
//		else {
//			destroyWindow("ResultR");
//		}
//		
//	}
//	pHSVThread_L = NULL;
//	destroyAllWindows();
//	return 0;
//};
//
//UINT CdemoDlg::HSVThread_R(LPVOID pParam) {
//	Mat searchImg;
//	Rect searchWindow_R(0, 0, 0, 0);
//
//	while (camera_state_R == START) {
//
//		if (is_SearchWindowGet_R) {
//			searchImg = ContourImage_R(searchWindow_R);
//			cvtColor(searchImg, hsv_R, CV_BGR2HSV);	//轉換色彩空間
//		}
//		else {
//			cvtColor(ContourImage_R, hsv_R, CV_BGR2HSV);	//轉換色彩空間
//		}
//			   
//		if (hmax <= 180) {
//			inRange(hsv_R, Scalar(MIN(hmax, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),
//				Scalar(MAX(hmax, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_R);
//		}
//		else {//special situation hue:180~210 
//
//			inRange(hsv_R, Scalar(MIN(180, hmin), MIN(smax, smin), MIN(vmax, vmin), 0),      //whether if hue between : hmin ~ 180
//				Scalar(MAX(180, hmin), MAX(smax, smin), MAX(vmax, vmin), 0), mask_R);
//
//			// hmax - 180 means we want 180~210 stands for 0~30 in hue value 
//			inRange(hsv_R, Scalar(MIN(hmax - 180, 0), MIN(smax, smin), MIN(vmax, vmin), 0),   //whether if hue between : 0 ~ (hmax-180)
//				Scalar(MAX(hmax - 180, 0), MAX(smax, smin), MAX(vmax, vmin), 0), mask1_R);
//			add(mask_R, mask1_R, mask_R);
//		}
//		
//		if (is_hsv_tunning == TRUE) {
//			namedWindow("mask_R", 0);
//			imshow("mask_R", mask_R);
//			waitKey(1);
//		}
//		else {
//			destroyWindow("mask_R");
//		}
//
//		if (is_hsv_tunning == TRUE) {
//			namedWindow("ResultR", 0);
//			imshow("ResultR", ContourImage_R);
//			waitKey(1);
//			is_SearchWindowGet_R = FALSE;//dont use searchWindow when tunning the hsv
//		}
//		else {
//			destroyWindow("ResultR");
//		}
//
//	}
//	pHSVThread_R = NULL;
//	destroyAllWindows();
//	return 0;
//
//};
 
void CdemoDlg::OnBnClickedCameraL()
{	//initialize	
	//if the camera doesn't start, then start it!
	if (camera_state_L != START) {
		pCameraThread_L = AfxBeginThread(CameraThread_L, pCameraThread_L, THREAD_PRIORITY_HIGHEST, 0, 0, NULL);			//pCameraThread_L, THREAD_PRIORITY_HIGHEST, 0, 0
		m_MainEditBox.SetWindowTextW(L"Open the Camera_L");
	}
	else {
		m_MainEditBox.SetWindowTextW(L"Camera_L is  already open!");
	}
}

void CdemoDlg::OnBnClickedCameraR()
{
	//if the camera doesn't start, then start it!
	if (camera_state_R != START) {											//提高優先權後，frame_rate從34-40 提升至36-42。	//THREAD_PRIORITY_NORMAL
		pCameraThread_R = AfxBeginThread(CameraThread_R, pCameraThread_R, THREAD_PRIORITY_HIGHEST, 0, 0, NULL);				//pCameraThread_R, THREAD_PRIORITY_HIGHEST, 0, 0
		m_MainEditBox.SetWindowTextW(L"Open the Camera_R");
	}
	else {
		m_MainEditBox.SetWindowTextW(L"Camera_R is already open!");
	}
}

void CdemoDlg::OnBnClickedImgshow()
{
	if (camera_state_L == START || camera_state_R == START) {

		if (pImageShowThread == NULL) {

			pImageShowThread = AfxBeginThread(ImageShowThread, NULL);				//pImageShowThread, THREAD_PRIORITY_NORMAL, 0, 0,
			m_MainEditBox.SetWindowTextW(L"Show Image On Screen!");
			//MessageBox(_T("Show Image On Screen!"));
			
		}
		else {
			m_MainEditBox.SetWindowTextW(L"already On!");
			//MessageBox(_T("already On!"));  //這不用了
		}
	}
	else {
		MessageBox(_T("Camera not open yet!"));
	}
}

//void CdemoDlg::OnBnClickedprocessingHsv()
//{
//	// TODO: 在此加入控制項告知處理常式程式碼
//	if (camera_state_L == START) {
//
//		if (pHSVThread_L = NULL) {
//
//			pHSVThread_L = AfxBeginThread(HSVThread_L, NULL);				//pImageShowThread, THREAD_PRIORITY_NORMAL, 0, 0,
//			m_MainEditBox.SetWindowTextW(L"Processing HSV!");
//			//MessageBox(_T("Show Image On Screen!"));
//
//		}
//		else {
//			m_MainEditBox.SetWindowTextW(L"already On!");
//			//MessageBox(_T("already On!"));  //這不用了
//		}
//	}
//	else {
//		MessageBox(_T("L: HSV not open yet!"));
//	}
//
//	if (camera_state_R == START) {
//
//		if (pHSVThread_R = NULL) {
//
//			pHSVThread_R = AfxBeginThread(HSVThread_R, NULL);				//pImageShowThread, THREAD_PRIORITY_NORMAL, 0, 0,
//			m_MainEditBox.SetWindowTextW(L"Processing HSV!");
//			//MessageBox(_T("Show Image On Screen!"));
//
//		}
//		else {
//			m_MainEditBox.SetWindowTextW(L"already On!");
//			//MessageBox(_T("already On!"));  //這不用了
//		}
//	}
//	else {
//		MessageBox(_T("R: HSV not open yet!"));
//	}
//}

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

void CdemoDlg::OnCbnSelchangeTrackingmethod()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	
	remove("DataCollect_c\\measure_points.csv");
	remove("DataCollect_c\\polyfit_points.txt");
	remove("DataCollect_c\\polyfit_landing_point.txt");

	remove("DataCollect_c\\polyfit_coeff_tx.txt");
	remove("DataCollect_c\\polyfit_coeff_ty.txt");
	remove("DataCollect_c\\polyfit_coeff_tz.txt");

	remove("DataCollect_c\\roots.txt");
	remove("DataCollect_c\\predict_trajectory_1st.csv");
	remove("DataCollect_c\\predict_landing_point.txt");
	remove("DataCollect_c\\rebound_modal.txt");
	remove("DataCollect_c\\predict_trajectory_2nd.csv");
	remove("DataCollect_c\\strike_point.txt");
	remove("DataCollect_c\\strike_point.csv");			//test

	remove("DataCollect_c\\predict_moving_direction.txt");
	remove("DataCollect_c\\nonKalman_landing_point.txt");
	remove("DataCollect_c\\Angularvelocity.txt");
	

	file_measure_points.open("DataCollect_c\\measure_points.csv", ios::app);
	file_polyfit_points.open("DataCollect_c\\polyfit_points.txt", ios::app);
	file_polyfit_landing_point.open("DataCollect_c\\polyfit_landing_point.txt", ios::app);

	file_polyfit_coeff_tx.open("DataCollect_c\\polyfit_coeff_tx.txt", ios::app);
	file_polyfit_coeff_ty.open("DataCollect_c\\polyfit_coeff_ty.txt", ios::app);
	file_polyfit_coeff_tz.open("DataCollect_c\\polyfit_coeff_tz.txt", ios::app);

	file_roots.open("DataCollect_c\\roots.txt", ios::app);
	file_predict_trajectory_1st.open("DataCollect_c\\predict_trajectory_1st.csv", ios::app);
	file_predict_landing_point.open("DataCollect_c\\predict_landing_point.txt", ios::app);
	file_rebound_modal.open("DataCollect_c\\rebound_modal.txt", ios::app);
	file_predict_trajectory_2nd.open("DataCollect_c\\predict_trajectory_2nd.csv", ios::app);
	file_strike_point.open("DataCollect_c\\strike_point.txt", ios::app);
	strike_point.open("DataCollect_c\\strike_point.csv", ios::app);

	file_predict_moving_direction.open("DataCollect_c\\predict_moving_direction.txt", ios::app);
	file_nonKalman_landing_point.open("DataCollect_c\\nonKalman_landing_point.txt", ios::app);
	file_Angularvelocity.open("DataCollect_c\\Angularvelocity.txt", ios::app);
	file_nonKalman_filter.open("DataCollect_c\\nonKalman_filter.txt", ios::app);
	file_A_training_data.open("DataCollect_c\\A_training_data.csv", ios::app);
	//file_A_training_data.open("DataCollect_c\\A_training_data.txt", ios::app);


	/*file_measure_points.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\measure_points.txt", ios::app);
	file_polyfit_points.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\polyfit_points.txt", ios::app);
	file_polyfit_landing_point.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\polyfit_landing_point.txt", ios::app);

	file_polyfit_coeff_tx.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\polyfit_coeff_tx.txt", ios::app);
	file_polyfit_coeff_ty.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\polyfit_coeff_ty.txt", ios::app);
	file_polyfit_coeff_tz.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\polyfit_coeff_tz.txt", ios::app);

	file_roots.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\roots.txt", ios::app);
	file_predict_trajectory_1st.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\predict_trajectory_1st.txt", ios::app);
	file_predict_landing_point.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\predict_landing_point.txt", ios::app);
	file_rebound_modal.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\rebound_modal.txt", ios::app);
	file_predict_trajectory_2nd.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\predict_trajectory_2nd.txt", ios::app);
	file_strike_point.open("D:\\Mega\\TableTennisRobot\\TableTennisRobot\\DataCollect\\strike_point.txt", ios::app);*/
	
	file_polyfit_coeff_tx.precision(dbl::max_digits10);
	file_polyfit_coeff_ty.precision(dbl::max_digits10);
	file_polyfit_coeff_tz.precision(dbl::max_digits10);


	if (camera_state_L == START || camera_state_R == START) {//if both camera open
		int index = m_TrackingMethod.GetCurSel();
		switch (index) {
		case FindContour:
			MethodChoose = FindContour;
			if (pFindContour) {
				break;
			}
			else {
				pFindContour = AfxBeginThread(FindContourThread, NULL);				//, pFindContour, THREAD_PRIORITY_NORMAL, 0, 0
				find_contour_state = FindContourStart;
			}

			if (pFindContourR) {
				break;
			}
			else {
				pFindContourR = AfxBeginThread(FindContourThreadR, NULL);				//, pFindContourR, THREAD_PRIORITY_NORMAL, 0, 0
				find_contour_state_R = FindContourStart;
			}

			ShowText("FindContour Start!");
			SetTimer(10, 18, NULL); //calculate hx,hy,hz in every 18 ms like 55hz
			break;
		case Hough:
			MethodChoose = Hough;
			break;
		default:
			MethodChoose = None;
			find_contour_state = FindContourStop;
			break;
		}
		UpdateData(FALSE);//更新資料至介面
	}
	else {
		MessageBox(TEXT(" error: camera should open! "));
		m_TrackingMethod.SetCurSel(None);
	}

}

void CdemoDlg::OnBnClickedStopcapture()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	camera_state_L = STOP;
	camera_state_R = STOP;
	ShowText("CameraStop");

	find_contour_state = FindContourStop;
	find_contour_state_R = FindContourStop;
	delete CHsvScrollBar();
	ball_state.clear();

	MethodChoose = None;
	m_TrackingMethod.SetCurSel(None);

	
	KillTimer(10);
}

UINT CdemoDlg::TrajectoryPredict(LPVOID pParam) {
	stringstream ss;
	string str;
	CString cstr;
	CWnd *pWnd = AfxGetApp()->m_pMainWnd;
		
	//KF.X << 0, 0, 0, 0, 0, 0;
	//double x1, y1, z1;// , vx1, vy1, vz1;				//strike_point.SetBallState(0, x1, y1, z1, vx1, vy1, vz1, time); 2041行，擊球點的球狀態不知道為什麼忽略了？->應該只是新功能
	double x0, y0, z0, vx0, vy0, vz0;
	double x1_c, y1_c, z1_c;
	double time, time_o;// , time_n;
	double time_new, time_old;
	//double Angularvelocity = 0;
	
	//for the nerual training
	/*double x_train = 0, y_train = 0, z_train = 0;
	double Vx_train = 0, Vy_train = 0, Vz_train = 0;
	double ax_train = 0, ay_train = 0, az_train = 0;
	double px_train = 0, py_train = 0, pz_train = 0;*/
	double x_train, y_train, z_train;
	double Vx_train, Vy_train, Vz_train;
	double Wx_train, Wy_train, Wz_train;
	double px_train, py_train, pz_train;

	//the time of final_point
	//time = ball_state[caclulate_points - 1].GetTimestamp_t();
	//time_o = ball_state[0].GetTimestamp_t();	
	time_o = (ball_state[caclulate_points - 5].GetTimestamp_t() + ball_state[caclulate_points - 4].GetTimestamp_t() + ball_state[caclulate_points - 3].GetTimestamp_t() + ball_state[caclulate_points - 2].GetTimestamp_t() + ball_state[caclulate_points - 1].GetTimestamp_t()) / 5;				//這邊是倒數一刻的時間
	//time_o = ball_state[caclulate_points - 2].GetTimestamp_t();
	time = ball_state[caclulate_points - 1].GetTimestamp_t();

	//the position of initinal_point
	/*x1 = ball_state[caclulate_points - 2].GetPosition_x();
	y1 = ball_state[caclulate_points - 2].GetPosition_y();
	z1 = ball_state[caclulate_points - 2].GetPosition_z();*/
	x1_c = (ball_state[caclulate_points - 5].GetPosition_x() + ball_state[caclulate_points - 4].GetPosition_x() + ball_state[caclulate_points - 3].GetPosition_x() + ball_state[caclulate_points - 2].GetPosition_x() + ball_state[caclulate_points - 1].GetPosition_x()) / 5;
	y1_c = (ball_state[caclulate_points - 5].GetPosition_y() + ball_state[caclulate_points - 4].GetPosition_y() + ball_state[caclulate_points - 3].GetPosition_y() + ball_state[caclulate_points - 2].GetPosition_y() + ball_state[caclulate_points - 1].GetPosition_y()) / 5;
	z1_c = (ball_state[caclulate_points - 5].GetPosition_z() + ball_state[caclulate_points - 4].GetPosition_z() + ball_state[caclulate_points - 3].GetPosition_z() + ball_state[caclulate_points - 2].GetPosition_z() + ball_state[caclulate_points - 1].GetPosition_z()) / 5;

	//final point 位置的算法絕對要改，不能使用3個做為平均。因為反彈的時候就錯了，Z的位置平均會很奇怪。 -> 這樣說不合理，因為只有一開始會使用這個數據
	/*x0 = (ball_state[caclulate_points - 5].GetPosition_x() + ball_state[caclulate_points - 4].GetPosition_x() + ball_state[caclulate_points - 3].GetPosition_x() + ball_state[caclulate_points - 2].GetPosition_x() + ball_state[caclulate_points - 1].GetPosition_x()) / 5;
	y0 = (ball_state[caclulate_points - 5].GetPosition_y() + ball_state[caclulate_points - 4].GetPosition_y() + ball_state[caclulate_points - 3].GetPosition_y() + ball_state[caclulate_points - 2].GetPosition_y() + ball_state[caclulate_points - 1].GetPosition_y()) / 5;
	z0 = (ball_state[caclulate_points - 5].GetPosition_z() + ball_state[caclulate_points - 4].GetPosition_z() + ball_state[caclulate_points - 3].GetPosition_z() + ball_state[caclulate_points - 2].GetPosition_z() + ball_state[caclulate_points - 1].GetPosition_z()) / 5;*/
	x0 = ball_state[caclulate_points - 1].GetPosition_x();
	y0 = ball_state[caclulate_points - 1].GetPosition_y();
	z0 = ball_state[caclulate_points - 1].GetPosition_z();

	//x0 = tx_a * pow(time, 2) + tx_b * time + tx_c;							//所以這邊是倒數一刻的位置與速度
	//y0 = ty_a * pow(time, 2) + ty_b * time + ty_c;
	//z0 = tz_a * pow(time, 2) + tz_b * time + tz_c;

	//derivative of second polynominal to calculate the speed
	/*vx0 = 2 * tx_a*time + tx_b;
	vy0 = 2 * ty_a*time + ty_b;
	vz0 = 2 * tz_a*time + tz_b;*/

	//the speed of final_point
	vx0 = (x0 - x1_c) / (time - time_o);
	vy0 = (y0 - y1_c) / (time - time_o);
	vz0 = (z0 - z1_c) / (time - time_o);

	ss.clear();														//此段用於輸出文字
	ss << "Index " << ball_index << " \r\n "
		<< "vx0 " << vx0 << " "
		<< "vy0 " << vy0 << " "
		<< "vz0 " << vz0 << " \r\n ";
	ShowText(ss.str());		
	
	//predict moving x-direction , the slider is 1200 milimeter ,and the table is 1500 wide 
	double predict_moving_y = 100;
	double predict_moving_x = x0 + vx0 * (100 - y0) / vy0;
	double predict_moving_z = 250;

	unsigned int slider_pos;
	file_predict_moving_direction << setw(10) << predict_moving_x << " " << setw(10) << predict_moving_y << " " << setw(10) << predict_moving_z << endl;

	//20181225 motor_control先忽略
	/*motor_control(predict_moving_x);
	Event_Control.SetEvent();*/

	if (predict_moving_x < 150) {
		slider_pos = 0;
	}
	else if (predict_moving_x > 1350) {
		slider_pos = 1200;
	}
	else {
		slider_pos = predict_moving_x - 150;
	}
	
	//*** 1st trajectory prediction by flying model with kalman filter ***//	
	file_predict_trajectory_1st << ball_index;		
	file_predict_trajectory_1st << "," << x0 << "," << y0 << "," << z0 << "," << vx0 << "," << vy0 << "," << vz0 << endl;
	/*file_predict_trajectory_1st << setw(15) << x0 << " " << setw(15) << y0 << " " << setw(15) << z0 << " "
								<< setw(15) << vx0 << " " << setw(15) << vy0 << " " << setw(15) << vz0 << endl;*/

	//::SetTimer(AfxGetApp()->m_pMainWnd->m_hWnd, 8, 500, NULL);  //find strike point in specific time		//開啟確認預測是否能再0.5s內完成
	//isTimeOut = false;

	//Initial State
	KF.X << x0, y0, z0, vx0, vy0, vz0;
	KF.Wx = 0; KF.Wy = 0; KF.Wz = 0;		

	x_train = x0; y_train = y0; z_train = z0;			//new add- original position & velocity
	Vx_train = vx0; Vy_train = vy0; Vz_train = vz0;
	
	//time_old = ball_state[caclulate_points - 1].GetTimestamp_t();	//Expression: sometime vector subscript out of range 檢測到球跳動的位置後，刪除了原球狀態列 line:866
	time_old = time;
	is_getAngularvelocity = 1;
	/*KF_t.X << x0, y0, z0, vx0, vy0, vz0;
	while (abs(Z - 0) > 50) {
		KF_t.dt = 0.031;
		KF_t.flyingmodel();

		file_nonKalman_filter << ball_index << " " << KF.Xerr[0] << " " << KF.Xerr[1] << " " << KF.Xerr[2] << endl;

		if (KF_t.X[2] < 50) break;
		
	}*/
	
	//WaitForSingleObject(Event_PredictTrajectory, INFINITE) == WAIT_OBJECT_0	這段有點不了解?好像是沒有再等待才會啟動?  Ans. 等待發出的訊號->觸發
	while (KF.X[2] > 0 && WaitForSingleObject(Event_PredictTrajectory, INFINITE) == WAIT_OBJECT_0) {		//INFINITE，僅在發出對象信號時才返回該函數
		time_new = (double)(GetTickCount() - ProgramStartTime) / 1000;  //seconds							//WAIT_OBJECT_0，發信號通知指定對象的狀態
		KF.dt = time_new - time_old;		
		//measurement
		KF.Z << X, Y, Z;
		//recursive predict
		KF.KalmanFilter();
		time_old = time_new;
		file_predict_trajectory_1st << ball_index << ",";		//file_predict_trajectory_1st		
		file_predict_trajectory_1st << KF.X[0] << "," << KF.X[1] << "," << KF.X[2] << ","
									<< KF.X[3] << "," << KF.X[4] << "," << KF.X[5] << endl;
		/*predict_trajectory_1st << setw(15) << KF.X[0] << " " << setw(15) << KF.X[1] << " " << setw(15) << KF.X[2] << " "
								<< setw(15) << KF.X[3] << " " << setw(15) << KF.X[4] << " " << setw(15) << KF.X[5] << endl;*/
		//file_Angularvelocity << ball_index << " " << KF.Xerr[0] << " " << KF.Xerr[1] << " " << KF.Xerr[2] << endl;	//估減預(因為檢測有時候球體可能會跳動)
		
		if (is_getAngularvelocity == 1) // is_getAngularvelocity == 1
		{
			Vector3d Velocity;// = (KF.Xerr) / (KF.dt);			//Zerr
			//Vector3d accelerationVelocity = Velocity / (KF.dt);
			Velocity[0] = ((KF.Xerr[0]) + 0.00001) / (KF.dt);
			Velocity[1] = ((KF.Xerr[1]) + 0.00001) / (KF.dt);
			Velocity[2] = ((KF.Xerr[2]) + 0.00001) / (KF.dt);
			KF.Wx = Velocity[0] / 20;
			KF.Wy = Velocity[1] / 20;
			KF.Wz = Velocity[2] / 20;			
			/*KF.ax = accelerationVelocity[0];
			KF.ay = accelerationVelocity[1];
			KF.az = accelerationVelocity[2];*/	 			
			//***********這邊好像可以了*************//
			
			//評估轉速 20190614
			/*KF_t.X = KF.X;
			KF_t.Zerr = KF.Zerr;
			KF_t.X_old = KF.X_old;
			KF_t.dt = KF.dt;

			KF_t.UpdateAngularvelocity();
			KF.Wx = KF_t.W_t[0];
			KF.Wy = KF_t.W_t[1];
			KF.Wz = KF_t.W_t[2];*/

			file_Angularvelocity << ball_index << " " << KF.Wx << " " << KF.Wy << " " << KF.Wz << endl;
			
			////collectdata;			
			//coll_data = {x_train, y_train, z_train,Vx_train, Vy_train, Vz_train, KF.Wx, KF.Wy, KF.Wz };
			////coll_data = { 305.126, 925.006, 161.064, -133.943, -3389.63, -1416.84, -0.0203016, 0.0234573, 0};
			//if (!pBpnn) {
			//	pBpnn = AfxBeginThread(Bpnn, NULL);
			//}			
		}
	
		//	//Vector3d Angularvelocity = Velocity / (KF.dt);
		//	//ss.clear();														//此段用於輸出文字
		//	//ss << "Index " << ball_index << " \r\n "
		//	//	<< "Wx " << KF.Wx << " "
		//	//	<< "Wy " << KF.Wy << " "
		//	//	<< "Wz " << KF.Wz << " \r\n ";
		//	//ShowText(ss.str());
		//	file_Angularvelocity << ball_index << " "
		//		<< KF.Zerr[0] << " "
		//		<< KF.Zerr[1] << " "
		//		<< KF.Zerr[2] << " "
		//		<< Velocity[0] << " "
		//		<< Velocity[1] << " "
		//		<< Velocity[2] << " "
		//		<< KF.ax << " "
		//		<< KF.ay << " "
		//		<< KF.az << endl;
		//		/*<< KF.Wx_o << " "
		//		<< KF.Wy_o << " "
		//		<< KF.Wz_o << " "
		//		<< KF.Wx << " "
		//		<< KF.Wy << " "
		//		<< KF.Wz << endl;*/
		//	//double Vtotal = sqrt(pow(Velocity[0], 2) + pow(Velocity[1], 2) + pow(Velocity[2], 2));			
		//	//Angularvelocity = Vtotal / 20;		// rb = 20 mm
		//}
		is_getAngularvelocity = 0;
		//ax_train = KF.ax; ay_train = KF.ay; az_train = KF.az;	//new add
		Wx_train = KF.Wx; Wy_train = KF.Wy; Wz_train = KF.Wz;
				
		array_x[index_first] = KF.X[0];
		array_y[index_first] = KF.X[1];
		array_z[index_first] = KF.X[2];
		/*datalist_XY[index_first] = &Point2d(KF.X[0], KF.X[1]);
		datalist_YZ[index_first] = &Point2d(KF.X[1], KF.X[2]);
		datalist_XZ[index_first] = &Point2d(KF.X[0], KF.X[2]);*/
		/*ss.clear();
		ss.str("");
		ss << "point_XY " << point_XY;
		str = ss.str();
		ShowText(ss.str());*/
		
		if (abs(Z - 0) < 100) {  //if the ball(radius is 20mm) is 30mm closer to table,then we find landing point.		//original 110
			if (KF.X[0] > 0 && KF.X[0] < 1500) { //if x is in the region of table

				ShowText("Got Landing Point!!!!!!");
				pWnd->GetDlgItem(IDC_MainEditBox)->SetWindowTextW(L"Got Landing Point!!!!!!");

				ss.clear();
				ss.str("");
				ss << "land_time " << time_new << " \r\n "
					<< "land_x " << KF.X[0] << " "
					<< "land_y " << KF.X[1] << " "
					<< "land_z " << KF.X[2] << " \r\n "
					<< "land_vx" << KF.X[3] << " "
					<< "land_vy" << KF.X[4] << " "
					<< "land_vz" << KF.X[5];

				str = ss.str();
				ShowText(ss.str());

				LandingTime = time_new;
				LandingPos_x = KF.X[0];
				LandingPos_y = KF.X[1];
				LandingPos_z = KF.X[2];

				file_predict_landing_point << KF.X[0] << " " << KF.X[1] << " " << KF.X[2] << " " << time_new << endl;				
				break;
			}
		}
		Event_PredictTrajectory.ResetEvent();

		
		//::KillTimer(pWnd->m_hWnd, 8);
		index_first++;
	}	
	file_predict_trajectory_1st << endl;

	
	//ss.clear();
	//ss.str("");
	//ss << "Ball Is Coming!!!" << "\r\n";
	//ShowText(ss.str());
	//pWnd->GetDlgItem(IDC_MainEditBox)->SetWindowTextW(L"ball is coming!!!!!!");	
	//*** rebound model ***//
	//calculate the Vin of rebound model 
	//30 ms before landing  = landing time - 0.002 (seconds)

	//landing velocity in
	double LandingVin_x = KF.X[3];
	double LandingVin_y = KF.X[4];
	double LandingVin_z = KF.X[5];

	//Rebound Model Coefficients									//這部分需要做實驗，驗證彈跳模型的參數。			
	//double Krx = 0.75204, bx = -0.011442e3;  //mm/s
	//double Kry = 0.50259, by = -0.50652e3;
	//double Krz = -0.87613, bz = 0.32194e3;

	double Krx = 4.2622E-01, bx = 1.522E+01;  //mm/s		//20190326	actually, this's not precise, because this's estimate number.
	double Kry = 3.6379E-02, by = -8.368E+02;
	double Krz = 2.9795E-01, bz = 4.400E+03;
	 

	//landing velocity out				//this isn't consider the wv
	double LandingVout_x = Krx * LandingVin_x + bx;
	double LandingVout_y = Kry * LandingVin_y + by;
	double LandingVout_z = Krz * LandingVin_z + bz;

	//write to file 
	file_rebound_modal << LandingVin_x << " " << LandingVin_y << " " << LandingVin_z << " "
					<< LandingVout_x << " " << LandingVout_y << " " << LandingVout_z << " " << endl << endl;
	pWnd->GetDlgItem(IDC_MainEditBox)->SetWindowTextW(L"Rebound!!!");


	//*** 2nd trajectory prediction by flying model with kalman filter ***//
	file_predict_trajectory_2nd << ball_index << ",";
	file_predict_trajectory_2nd << LandingPos_x << "," << LandingPos_y << "," << LandingPos_z << ","
								<< LandingVout_x << "," << LandingVout_y << "," << LandingVout_z << endl;
	/*file_predict_trajectory_2nd << setw(15) << LandingPos_x << " " << setw(15) << LandingPos_y << " " << setw(15) << LandingPos_z << " " 
								<< setw(15) << LandingVout_x << " " << setw(15) << LandingVout_y << " " << setw(15) << LandingVout_z << endl;*/

	double strike_plane = 100; //original:10cm 

	x0 = LandingPos_x;
	y0 = LandingPos_y;
	z0 = LandingPos_z;
	vx0 = LandingVout_x;
	vy0 = LandingVout_y;
	vz0 = LandingVout_z;

	//Initial State
	KF.X << x0, y0, z0, vx0, vy0, vz0;
	
	KF.P << 10, 0, 0, 0, 0, 0,
		0, 10, 0, 0, 0, 0,
		0, 0, 10, 0, 0, 0,
		0, 0, 0, 10, 0, 0,
		0, 0, 0, 0, 10, 0,
		0, 0, 0, 0, 0, 10;

	//KF_t.W_t << 0, 0, 0;
	KF.Wx = 0; KF.Wy = 0; KF.Wz = 0;

	is_getAngularvelocity = 1;

	while (KF.X[2] > 0 && WaitForSingleObject(Event_PredictTrajectory, INFINITE) == WAIT_OBJECT_0) {

		time_new = (double)(GetTickCount() - ProgramStartTime) / 1000;  //seconds
		KF.dt = time_new - time_old;
		//measurement
		KF.Z << X, Y, Z;
		//recursive predict
		KF.KalmanFilter();
		time_old = time_new;

		file_predict_trajectory_2nd << ball_index << ",";		
		file_predict_trajectory_2nd << KF.X[0] << "," << KF.X[1] << "," << KF.X[2] << ","
									<< KF.X[3] << "," << KF.X[4] << "," << KF.X[5] << endl;
		/*file_predict_trajectory_2nd << setw(15) << KF.X[0] << " " << setw(15) << KF.X[1] << " " << setw(15) << KF.X[2] << " "
									<< setw(15) << KF.X[3] << " " << setw(15) << KF.X[4] << " " << setw(15) << KF.X[5] << endl;*/
		//file_Angularvelocity << ball_index << " " << KF.Xerr[0] << " " << KF.Xerr[1] << " " << KF.Xerr[2] << endl;

		if (is_getAngularvelocity == 1) // is_getAngularvelocity == 1
		{			
			Vector3d Velocity;	// = (KF.Xerr) / (KF.dt);				//Zerr 20190614			
			Velocity[0] = ((KF.Xerr[0]) + 0.00001) / (KF.dt);
			Velocity[1] = ((KF.Xerr[1]) + 0.00001) / (KF.dt);
			Velocity[2] = ((KF.Xerr[2]) + 0.00001) / (KF.dt);
			//Vector3d accelerationVelocity = Velocity / (KF.dt);			
			KF.Wx = Velocity[0] / 20;
			KF.Wy = Velocity[1] / 20;
			KF.Wz = Velocity[2] / 20;

			/*KF.ax = accelerationVelocity[0];
			KF.ay = accelerationVelocity[1];
			KF.az = accelerationVelocity[2];						
			Vector3d Angularvelocity = Velocity / (KF.dt);*/			
			//ss.clear();														//此段用於輸出文字
			//ss << "Index " << ball_index << " \r\n "
			//	<< "Wx " << KF.Wx << " "
			//	<< "Wy " << KF.Wy << " "
			//	<< "Wz " << KF.Wz << " \r\n ";
			//ShowText(ss.str());			

			//評估轉速 20190614

			/*KF_t.X = KF.X;
			KF_t.Zerr = KF.Zerr;
			KF_t.X_old = KF.X_old;
			KF_t.dt = KF.dt;

			KF_t.UpdateAngularvelocity();
			KF.Wx = KF_t.W_t[0];
			KF.Wy = KF_t.W_t[1];
			KF.Wz = KF_t.W_t[2];*/
			file_Angularvelocity << ball_index << " " << KF.Wx << " " << KF.Wy << " " << KF.Wz << endl;
			
			//KF.UpdateAngularvelocity();
			//KF.Wx = KF.W_t[0];
			//KF.Wy = KF.W_t[1];
			//KF.Wz = KF.W_t[2];
			//file_Angularvelocity << ball_index << " "
			//	/*<< KF.Zerr[0] << " "
			//	<< KF.Zerr[1] << " "
			//	<< KF.Zerr[2] << " "
			//	<< Angularvelocity[0] << " "
			//	<< Angularvelocity[1] << " "
			//	<< Angularvelocity[2] << endl;*/
			//	<< KF.ax << " "
			//	<< KF.ay << " "
			//	<< KF.az << " "
			//	<< KF.Wx << " "
			//	<< KF.Wy << " "
			//	<< KF.Wz << endl;
				//double Vtotal = sqrt(pow(Velocity[0], 2) + pow(Velocity[1], 2) + pow(Velocity[2], 2));			
				//Angularvelocity = Vtotal / 20;		// rb = 20 mm
		}			
		is_getAngularvelocity = 0;
		
		array_x[index_first] = KF.X[0];
		array_y[index_first] = KF.X[1];
		array_z[index_first] = KF.X[2];
		/*datalist_XY[index_first] = &Point2d(KF.X[0], KF.X[1]);
		datalist_YZ[index_first] = &Point2d(KF.X[1], KF.X[2]);
		datalist_XZ[index_first] = &Point2d(KF.X[0], KF.X[2]);*/

		if (abs(KF.X[1] - strike_plane) < 50 || abs(KF.X[1]) < 50) {  //if the strike point in 5cm then
			if (KF.X[0] > 0 && KF.X[0] < 1500 ) { //if it's in the region of table		// && KF.X[2] < 50 測試二階落點
				
				//strike_point.SetBallState(0, x1, y1, z1, vx1, vy1, vz1, time);
				pWnd->GetDlgItem(IDC_MainEditBox)->SetWindowTextW(L"Got Strike Point !!!!!!");
				is_getStikePoint = TRUE;
				Event_TrajectoryPredict_XYZ.SetEvent();		//手動設定模式，初始狀態為FALSE
				//write to file strike_position.txt
				file_strike_point << KF.X[0] << " " << KF.X[1] << " " << KF.X[2] << " " << KF.X[3] << " " << KF.X[4] << " " << KF.X[5] << " " << time_new << endl;
				
				if (!is_ShowImg_Receiving_XY) is_ShowImg_Receiving_XY = 1;
				if (!is_ShowImg_Receiving_YZ) is_ShowImg_Receiving_YZ = 1;
				if (!is_ShowImg_Receiving_ZX) is_ShowImg_Receiving_ZX = 1;

				//****for training, if this didn't to used, it's need turn off***///
				px_train = KF.X[0]; py_train = KF.X[1]; pz_train = KF.X[2];		//new add(strike_point)
				/*file_A_training_data << x_train << " " << y_train << " " << z_train << " " << Vx_train << " " << Vy_train << " " << Vz_train << " "
									<< ax_train << " " << ay_train << " " << az_train << " " << px_train << " " << py_train << " " << pz_train << endl;*/
				file_A_training_data << x_train << "," << y_train << "," << z_train << "," << Vx_train << "," << Vy_train << "," << Vz_train << ","
									<< Wx_train << "," << Wy_train << "," << Wz_train << "," << px_train << "," << py_train << "," << pz_train << endl;
				break;
			}
		}		

		Event_PredictTrajectory.ResetEvent();
		index_first++;
	}//end while 2nd trajectory 
	index_second = index_first;
	file_predict_trajectory_2nd << endl;	
	pTrajectoryPredict = NULL;
	return 0;
}

void CdemoDlg::OnBnClickedListboxclear()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	//m_MainListBox.ResetContent();		//clean listbox

	LstSquQuadRegr QuadPoly;
	//typedef std::numeric_limits< double > dbl;
	cout.precision(dbl::max_digits10);					//precision 輸出的數據之小數位精度

	stringstream ss;
	ss.precision(dbl::max_digits10);

	QuadPoly.AddPoints(710.016, 945.191);   //(t,x)
	QuadPoly.AddPoints(710.062, 930.57);  //2
	QuadPoly.AddPoints(710.078, 920.931);  //3
	QuadPoly.AddPoints(710.125, 905.197);   //4
	QuadPoly.AddPoints(710.156, 898.161);	//5
	//QuadPoly.AddPoints(710.172, 885.711);  //6
	//QuadPoly.AddPoints(710.203, 870.323);  //7
	//QuadPoly.AddPoints(710.234, 861.002);  //8
	//QuadPoly.AddPoints(710.266, 839.907);  //9
	//QuadPoly.AddPoints(710.375, 837.616);  //10

	QuadPoly.CalculateCoefficient();

	double a = (double)QuadPoly.GetCoefficient().a;
	double b = (double)QuadPoly.GetCoefficient().b;
	double c = (double)QuadPoly.GetCoefficient().c;

	/*fstream test;
	test << ios::fixed << setprecision(15) << endl;
	test.open("testfile.txt",ios::app);
	test << a << " " << b << " " << c << endl;*/

	string str;
	ss << "a " << QuadPoly.GetCoefficient().a << "\r\n"
		<< "b " << QuadPoly.GetCoefficient().b << "\r\n"
		<< "c " << QuadPoly.GetCoefficient().c << "\r\n";
	ShowText(ss.str());
}

void CdemoDlg::OnBnClickedShowtext()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	is_EnableShowText = !is_EnableShowText;
	CWnd* pWnd = GetDlgItem(IDC_ShowText);
	pWnd->SetWindowText(is_EnableShowText ? _T("Pause") : _T("Continue"));

}

void CdemoDlg::OnBnClickedopenpicturecontroll()
{
#if 0
	// TODO: 在此加入控制項告知處理常式程式碼
	
	//get picture control dc	
	CWnd * pWnd_PicControl3 = AfxGetMainWnd()->GetDlgItem(IDC_PicControl3);
	CDC* dc_pc3 = pWnd_PicControl3->GetWindowDC();

	////get picture control dc 
	//CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);
	//CWnd * pWnd_PicControl3 = pMainWnd->GetDlgItem(IDC_PicControl3);
	//CDC* dc_pc3 = pWnd_PicControl3->GetWindowDC();

	//get size of picture control,and prepare a image space for resize image to fit in picture control
	RECT rect_pc;
	pWnd_PicControl3->GetWindowRect(&rect_pc);
	//IplImage* Img_Pic1 = cvCreateImage(cvSize((rect_pc.right - rect_pc.left), (rect_pc.bottom - rect_pc.top)), 8, 3);
	IplImage* Img_Pic1 = cvCreateImage(cvSize(1540, 2740), 8, 3);
	IplImage* resize_img = cvCreateImage(cvSize((rect_pc.right - rect_pc.left), (rect_pc.bottom - rect_pc.top)), 8, 3);
	
	////get size of picture control,and prepare a image space for resize image to fit in picture control
	//RECT rect_pc;
	//pMainWnd->GetDlgItem(IDC_PicControl3)->GetWindowRect(&rect_pc);
	//IplImage* Img_Pic1 = cvCreateImage(cvSize((rect_pc.right - rect_pc.left), (rect_pc.bottom - rect_pc.top)), IPL_DEPTH_8U, 3);
	
	CvvImage show_p3;
	
	if (find_contour_state == FindContourStart)
	{
		if (ball_state.size() != 0 && temp.GetPosition_y() > temp_old.GetPosition_y()) {				//20190121 向後的球狀態
			m_MainEditBox.SetWindowTextW(L"backward_no_tralectory!");
			cvReleaseImage(&Img_Pic1);
			Point2d datalist[200] = { 0 };
			index_first = 0;			
		}
			/*file_measure_points.read("DataCollect\\measure_points.txt", ios::in);
			file_predict_trajectory_1st.read("DataCollect\\predict_trajectory_1st.txt", ios::in);
			file_predict_landing_point.read("DataCollect\\predict_landing_point.txt", ios::in);
			file_rebound_modal.read("DataCollect\\rebound_modal.txt", ios::in);
			file_predict_trajectory_2nd.read("DataCollect\\predict_trajectory_2nd.txt", ios::in);
			file_strike_point.read("DataCollect\\strike_point.txt", ios::in);
			file_predict_moving_direction.read("DataCollect\\predict_moving_direction.txt", ios::in);*/			
		else {
			/*cvLine(Img_Pic1, cvPoint(743.433, 1465.6), cvPoint(757.654, 1409.35), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(757.654, 1409.35), cvPoint(786.398, 1244.36), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(786.398, 1244.36), cvPoint(800.683, 1164.57), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(800.683, 1164.57), cvPoint(812.926, 1072.09), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(812.926, 1072.09), cvPoint(827.188, 980.031), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(827.188, 980.031), cvPoint(838.476, 953.287), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(838.476, 953.287), cvPoint(852.526, 871.612), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(852.526, 871.612), cvPoint(864.34, 808.307), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(864.34, 808.307), cvPoint(874.505, 746.706), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(874.505, 746.706), cvPoint(883.764, 714.089), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(883.764, 714.089), cvPoint(895.678, 678.316), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(895.678, 678.316), cvPoint(912.067, 600.878), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(912.067, 600.878), cvPoint(924.768, 505.123), Scalar(0, 0, 255), 3, 8, 0);
				cvLine(Img_Pic1, cvPoint(924.768, 505.123), cvPoint(935.72, 443.804), Scalar(0, 0, 255), 3, 8, 0);*/

			for (int index_num = 1; index_num < index_first; index_num++) {
			cvLine(Img_Pic1, datalist[index_num-1], datalist[index_num], Scalar(0, 0, 255), 3, 8, 0);
			}
			//cvLine(Img_Pic1, cvPoint((rect_pc.right - rect_pc.left)/2 , 0), cvPoint((rect_pc.right - rect_pc.left)/2 , (rect_pc.bottom - rect_pc.top)), Scalar(0, 0, 255), 3, 8, 0); // CV_RGB(255,0,0)
			//resize the image to fit into the picture control 
			cvResize(Img_Pic1, resize_img);
			cvFlip(resize_img, resize_img);
			//show left image in picture control 
			show_p3.CopyOf(resize_img);
			show_p3.Show(*dc_pc3, 0, 0, (rect_pc.right - rect_pc.left), (rect_pc.bottom - rect_pc.top));
			
			//release image and ask for picture again
			cvReleaseImage(&Img_Pic1);
		}
	}
	

	//cvReleaseImage(&Img_Pic1);
	//destroyAllWindows();

#else if
	if (camera_state_L == START || camera_state_R == START) {

		if (pTrajectoryPredict_XYZ == NULL) {	//find_contour_state == FindContourStart
			pTrajectoryPredict_XYZ = AfxBeginThread(TrajectoryPredict_XYZ, NULL);			//pCameraThread_L, THREAD_PRIORITY_HIGHEST, 0, 0
			m_MainEditBox.SetWindowTextW(L"Open the TrajectoryPredict_XY");
		}
		else {
			m_MainEditBox.SetWindowTextW(L"TrajectoryPredict_XY is  already open!");

		}
	}
#endif
}

UINT CdemoDlg::TrajectoryPredict_XYZ(LPVOID pParam)
{
	////get picture control dc 
	//CWnd * pMainWnd = FromHandle(AfxGetApp()->m_pMainWnd->m_hWnd);

	//get picture control dc
	//CWnd* pWnd_PicControl3 = pMainWnd->GetDlgItem(IDC_PicControl3);
	CWnd* pWnd_PicControl3 = AfxGetMainWnd()->GetDlgItem(IDC_PicControl3);
	CDC* dc_pc3 = pWnd_PicControl3->GetWindowDC();
	
	//CWnd* pWnd_PicControl4 = pMainWnd->GetDlgItem(IDC_PicControl4);
	CWnd* pWnd_PicControl4 = AfxGetMainWnd()->GetDlgItem(IDC_PicControl4);	
	CDC* dc_pc4 = pWnd_PicControl4->GetWindowDC();
	
	//CWnd* pWnd_PicControl5 = pMainWnd->GetDlgItem(IDC_PicControl5);
	CWnd* pWnd_PicControl5 = AfxGetMainWnd()->GetDlgItem(IDC_PicControl5);	
	CDC* dc_pc5 = pWnd_PicControl5->GetWindowDC();
	

	


	//get size of picture control,and prepare a image space for resize image to fit in picture control
	RECT rect_pc3;
	pWnd_PicControl3->GetWindowRect(&rect_pc3);
	//pMainWnd->GetDlgItem(IDC_PicControl3)->GetWindowRect(&rect_pc3);
	RECT rect_pc4;
	pWnd_PicControl4->GetWindowRect(&rect_pc4);
	//pMainWnd->GetDlgItem(IDC_PicControl4)->GetWindowRect(&rect_pc4);
	RECT rect_pc5;
	pWnd_PicControl5->GetWindowRect(&rect_pc5);
	//pMainWnd->GetDlgItem(IDC_PicControl5)->GetWindowRect(&rect_pc5);

	//IplImage* Img_Pic1 = cvCreateImage(cvSize((rect_pc.right - rect_pc.left), (rect_pc.bottom - rect_pc.top)), 8, 3);
	IplImage* Img_Pic1 = cvCreateImage(cvSize(1540, 2740), 8, 3);
	IplImage* resize_img1 = cvCreateImage(cvSize((rect_pc3.right - rect_pc3.left), (rect_pc3.bottom - rect_pc3.top)), 8, 3);

	IplImage* Img_Pic2 = cvCreateImage(cvSize(2740, 1500), 8, 3);
	IplImage* resize_img2 = cvCreateImage(cvSize((rect_pc4.right - rect_pc4.left), (rect_pc4.bottom - rect_pc4.top)), 8, 3);

	IplImage* Img_Pic3 = cvCreateImage(cvSize(1500, 1540), 8, 3);
	IplImage* resize_img3 = cvCreateImage(cvSize((rect_pc5.right - rect_pc5.left), (rect_pc5.bottom - rect_pc5.top)), 8, 3);	
	
	CvvImage show_p3;
	CvvImage show_p4;
	CvvImage show_p5;
	
	IplImage* Img_Pic1_XY = cvCloneImage(Img_Pic1);
	IplImage* Img_Pic1_YZ = cvCloneImage(Img_Pic2);
	IplImage* Img_Pic1_ZX = cvCloneImage(Img_Pic3);

	while (camera_state_L == START && camera_state_R == START )	//find_contour_state == FindContourStart
	{		
		Sleep(0);

		/*if(!is_ShowImg_Receiving_XY) Img_Pic1_XY = cvCloneImage(Img_Pic1);
		if(!is_ShowImg_Receiving_YZ) Img_Pic1_YZ = cvCloneImage(Img_Pic2);
		if(!is_ShowImg_Receiving_ZX) Img_Pic1_ZX = cvCloneImage(Img_Pic3);*/
		
		if (WaitForSingleObject(Event_TrajectoryPredict_XYZ, INFINITE) == WAIT_OBJECT_0) {
			
			if (is_ShowImg_Receiving_XY)
			{
				Img_Pic1_XY = cvCloneImage(Img_Pic1);
				for (unsigned int index_num = 1; index_num < index_second; index_num++) {
					//cvLine(Img_Pic1_XY, *datalist_XY[index_num - 1], *datalist_XY[index_num], Scalar(0, 0, 255), 5, 8, 0);
					cvLine(Img_Pic1_XY, cvPoint(array_x[index_num - 1], array_y[index_num - 1]), cvPoint(array_x[index_num], array_y[index_num]), Scalar(0, 0, 255), 5, 8, 0);
				}

				cvResize(Img_Pic1_XY, resize_img1);
				cvFlip(resize_img1, resize_img1);
				show_p3.CopyOf(resize_img1);
				show_p3.Show(*dc_pc3, 0, 0, (rect_pc3.right - rect_pc3.left), (rect_pc3.bottom - rect_pc3.top));

				cvReleaseImage(&Img_Pic1_XY);
				is_ShowImg_Receiving_XY = 0;
			}

			if (is_ShowImg_Receiving_YZ)
			{
				Img_Pic1_YZ = cvCloneImage(Img_Pic2);
				for (unsigned int index_num = 1; index_num < index_second; index_num++) {
					//cvLine(Img_Pic1_YZ, *datalist_YZ[index_num - 1], *datalist_YZ[index_num], Scalar(0, 0, 255), 5, 8, 0);
					cvLine(Img_Pic1_YZ, cvPoint(array_y[index_num - 1], array_z[index_num - 1]), cvPoint(array_y[index_num], array_z[index_num]), Scalar(0, 0, 255), 5, 8, 0);
				}

				cvResize(Img_Pic1_YZ, resize_img2);
				cvFlip(resize_img2, resize_img2);
				show_p4.CopyOf(resize_img2);
				show_p4.Show(*dc_pc4, 0, 0, (rect_pc4.right - rect_pc4.left), (rect_pc4.bottom - rect_pc4.top));

				cvReleaseImage(&Img_Pic1_YZ);
				is_ShowImg_Receiving_YZ = 0;
			}

			if (is_ShowImg_Receiving_ZX)
			{
				Img_Pic1_ZX = cvCloneImage(Img_Pic3);
				for (unsigned int index_num = 1; index_num < index_second; index_num++) {
					//cvLine(Img_Pic1_ZX, *datalist_XZ[index_num - 1], *datalist_XZ[index_num], Scalar(0, 0, 255), 5, 8, 0);
					cvLine(Img_Pic1_ZX, cvPoint(array_x[index_num - 1], array_z[index_num - 1]), cvPoint(array_x[index_num], array_z[index_num]), Scalar(0, 0, 255), 5, 8, 0);
				}

				cvResize(Img_Pic1_ZX, resize_img3);
				cvFlip(resize_img3, resize_img3);
				show_p5.CopyOf(resize_img3);
				show_p5.Show(*dc_pc5, 0, 0, (rect_pc5.right - rect_pc5.left), (rect_pc5.bottom - rect_pc5.top));

				cvReleaseImage(&Img_Pic1_ZX);
				is_ShowImg_Receiving_ZX = 0;
			}

			Event_TrajectoryPredict_XYZ.ResetEvent();
		}
		cvReleaseImage(&Img_Pic1_XY);
		cvReleaseImage(&Img_Pic1_YZ);
		cvReleaseImage(&Img_Pic1_ZX);

		//if(is_getStikePoint == 1 && WaitForSingleObject(Event_TrajectoryPredict_XYZ, INFINITE) == WAIT_OBJECT_0){
		////if (temp.GetPosition_y() < 100 && temp.GetPosition_y() < temp_old.GetPosition_y()) {
		//	/*cvLine(Img_Pic1, cvPoint(743.433, 1465.6), cvPoint(757.654, 1409.35), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(757.654, 1409.35), cvPoint(786.398, 1244.36), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(786.398, 1244.36), cvPoint(800.683, 1164.57), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(800.683, 1164.57), cvPoint(812.926, 1072.09), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(812.926, 1072.09), cvPoint(827.188, 980.031), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(827.188, 980.031), cvPoint(838.476, 953.287), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(838.476, 953.287), cvPoint(852.526, 871.612), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(852.526, 871.612), cvPoint(864.34, 808.307), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(864.34, 808.307), cvPoint(874.505, 746.706), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(874.505, 746.706), cvPoint(883.764, 714.089), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(883.764, 714.089), cvPoint(895.678, 678.316), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(895.678, 678.316), cvPoint(912.067, 600.878), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(912.067, 600.878), cvPoint(924.768, 505.123), Scalar(0, 0, 255), 3, 8, 0);
		//		cvLine(Img_Pic1, cvPoint(924.768, 505.123), cvPoint(935.72, 443.804), Scalar(0, 0, 255), 3, 8, 0);*/
		//
		//	IplImage* Img_Pic1_XY = cvCloneImage(Img_Pic1);
		//	IplImage* Img_Pic1_YZ = cvCloneImage(Img_Pic2);
		//	IplImage* Img_Pic1_XZ = cvCloneImage(Img_Pic3);
		//
		//	for (unsigned int index_num = 1; index_num < index_second; index_num++) {
		//		/*if (datalist[index_num - 1] = 0 || datalist[index_num] = 0)
		//			continue;*/
		//		//cvLine(Img_Pic1, *datalist_XY[index_num - 1], *datalist_XY[index_num], Scalar(0, 0, 255), 5, 8, 0);		//Img_Pic1_XY
		//		//cvLine(Img_Pic2, *datalist_YZ[index_num - 1], *datalist_YZ[index_num], Scalar(0, 0, 255), 5, 8, 0);		//Img_Pic1_YZ
		//		//cvLine(Img_Pic3, *datalist_XZ[index_num - 1], *datalist_XZ[index_num], Scalar(0, 0, 255), 5, 8, 0);		//Img_Pic1_XZ
		//		cvLine(Img_Pic1_XY, *datalist_XY[index_num - 1], *datalist_XY[index_num], Scalar(0, 0, 255), 5, 8, 0);		
		//		cvLine(Img_Pic1_YZ, *datalist_YZ[index_num - 1], *datalist_YZ[index_num], Scalar(0, 0, 255), 5, 8, 0);		
		//		cvLine(Img_Pic1_XZ, *datalist_XZ[index_num - 1], *datalist_XZ[index_num], Scalar(0, 0, 255), 5, 8, 0);		
		//	}
		//	//cvLine(Img_Pic1, cvPoint((rect_pc.right - rect_pc.left)/2 , 0), cvPoint((rect_pc.right - rect_pc.left)/2 , (rect_pc.bottom - rect_pc.top)), Scalar(0, 0, 255), 3, 8, 0); // CV_RGB(255,0,0)
		//	//resize the image to fit into the picture control 
		//	//cvResize(Img_Pic1, resize_img1);		//Img_Pic1_XY
		//	//cvResize(Img_Pic2, resize_img2);		//Img_Pic1_YZ
		//	//cvResize(Img_Pic3, resize_img3);		//Img_Pic1_XZ
		//	cvResize(Img_Pic1_XY, resize_img1);		
		//	cvResize(Img_Pic1_YZ, resize_img2);		
		//	cvResize(Img_Pic1_XZ, resize_img3);		
		//	cvFlip(resize_img1, resize_img1);
		//	cvFlip(resize_img2, resize_img2);
		//	cvFlip(resize_img3, resize_img3);
		//	//show left image in picture control 
		//	show_p3.CopyOf(resize_img1);
		//	show_p4.CopyOf(resize_img2);
		//	show_p5.CopyOf(resize_img3);
		//	show_p3.Show(*dc_pc3, 0, 0, (rect_pc3.right - rect_pc3.left), (rect_pc3.bottom - rect_pc3.top));
		//	show_p4.Show(*dc_pc4, 0, 0, (rect_pc4.right - rect_pc4.left), (rect_pc4.bottom - rect_pc4.top));
		//	show_p5.Show(*dc_pc5, 0, 0, (rect_pc5.right - rect_pc5.left), (rect_pc5.bottom - rect_pc5.top));
		//	//release image and ask for picture again
		//	//IplImage* Img_Pic1_XY = cvCloneImage(Img_Pic1);
		//	//Img_Pic1_XY = 0;
		//	/*cvReleaseImage(&Img_Pic1);
		//	cvReleaseImage(&Img_Pic2);
		//	cvReleaseImage(&Img_Pic3); */
		//	cvReleaseImage(&Img_Pic1_XY);
		//	cvReleaseImage(&Img_Pic1_YZ);
		//	cvReleaseImage(&Img_Pic1_XZ);
		//	
		//}
		//
		//else if(is_getStikePoint == 0){	//new add
		//	Event_TrajectoryPredict_XYZ.ResetEvent();
		//}
		
		//else if (ball_state.size() != 0 && temp.GetPosition_y() > temp_old.GetPosition_y()) {				//20190121 向後的球狀態
		//	//m_MainEditBox.SetWindowTextW(L"backward_no_tralectory!");
		//	ShowText("backward_no_trajectory!");
		//	cvReleaseImage(&Img_Pic1);
		//	cvReleaseImage(&Img_Pic2);
		//	cvReleaseImage(&Img_Pic3);
		//	Point2d* datalist_XY[200] = { 0 };
		//	Point2d* datalist_YZ[200] = { 0 };
		//	Point2d* datalist_XZ[200] = { 0 };
		//	index_first = 0;
		//	index_second = 0;
		//	/*Img_Pic1_XY = 0;
		//	Img_Pic1_YZ = 0;
		//	Img_Pic1_XZ = 0;*/
		//}	
	}	
	
	pTrajectoryPredict_XYZ = NULL;
	return 0;
}

void CdemoDlg::OnBnClickedImgsave()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	if (pCameraThread_R == NULL || pCameraThread_L == NULL) {	//nullptr
		MessageBox(L"Camera not open yet!");
	}
	else {
		//flags to take a picture 
		is_save_picR = 1;
		is_save_picL = 1;

		cvDestroyAllWindows();
		sprintf(pic_num, "%03d", num++);  //number to string 

		// or operation : if i got picture then 0 => so 0 | 0 equals 0 means i got both picture then jump out of while ! 
		while (is_save_picR | is_save_picL) {
			//just wait for the picture giving from thread!
			//cvWaitKey(1);
		}

		strcpy(filename, "Pic_L");
		strcat(filename, pic_num);
		strcat(filename, ".jpg");
		
		cvNamedWindow(filename, 0);	
		cvShowImage(filename, save_imgl);			
		cvSaveImage(filename, save_imgl);
		cvReleaseImage(&save_imgl);
		filename[0] = '\0';

		strcpy(filename, "Pic_R");
		strcat(filename, pic_num);
		strcat(filename, ".jpg");

		cvNamedWindow(filename, 0);
		cvShowImage(filename, save_imgr);
		cvSaveImage(filename, save_imgr);
		cvReleaseImage(&save_imgr);

		cvWaitKey(1);
	}

}

//void CdemoDlg::printMatrix(vector<double> matrix) {
//	for (int i = 0; i < matrix.size(); i++) {
//		cout << matrix[i];
//		if (i % 3 == 2) {	//i % 4 == 3
//			cout << endl;
//		}
//		else {
//			cout << "\t";
//		}
//	}
//}
//
//void CdemoDlg::printColumn(vector<double> matrix) {
//	for (int i = 0; i < matrix.size(); i++) {
//		cout << matrix[i];
//		cout << endl;
//	}
//}
//
//double CdemoDlg::calcuMSE(vector<double> testdata, double* weight, double* weight1, double* weight2) {
//
//	//double calcuMSE_x;
//	//double calcuMSE_y;
//	//double calcuMSE_z;
//	double eta = 0.01;
//
//	double netj;
//	double netk;
//	double netl;
//
//	double* netj_r;
//	double* netk_r;
//	double* netl_r;
//	double M[16] = { 0 };
//	double N[16] = { 0 };
//	double O[3] = { 0 };
//	netj_r = M;
//	netk_r = N;
//	netl_r = O;
//
//	
//
//	for (int n = 0; n < testdata.size(); n += 12) {
//
//		//cout << part1[n] << "\t" << part1[n + 1] << "\t" << part1[n + 2] << endl;
//		for (int j = 0; j < 16; j++)
//		{
//			netj = 0;
//			for (int k = 0; k < 9; k++)
//			{
//				netj += testdata[n + k] * weight[j * 9 + k];
//			}
//			M[j] = 1 / (1 + exp(-netj));
//		}
//
//		for (int j = 0; j < 16; j++)	//hidden_to_hidden
//		{
//			netk = 0;
//			for (int k = 0; k < 16; k++)
//			{
//				netk += netj_r[k] * weight1[j * 16 + k];
//			}
//			N[j] = 1 / (1 + exp(-netk));
//		}
//
//		for (int j = 0; j < 3; j++)
//		{
//			netl = 0;
//			for (int k = 0; k < 16; k++)
//			{
//				netl += netk_r[k] * weight2[j * 16 + k];
//			}
//			O[j] = 1 / (1 + exp(-netl));
//		}
//	}
//
//	//double outputdata;
//	//outputdata = { O[0] , O[1], O[2] };
//
//	for (int i = 0; i < 3; i++)
//	{
//		printf("strike_point: ");
//		//printf("W[%d,%d]=%f\n", j, i, matrix_weightij[j + i * 9]);
//		printf("P[%d]=%f\n", i, netl_r[i]);
//		array_netl[i] = netl_r[i];
//	}
//	return array_netl[2];
//}
//
//vector<double> CdemoDlg::readfile(string filename)
//{
//	cout << "readfile is open" << endl;
//	vector<double> reult;
//	ifstream file(filename);
//	cout << "loadingfile" << endl;
//	string line;
//	if (!file.is_open()) { cout << "loading file error" << endl; }
//
//	while (getline(file, line, '\n'))
//	{
//		cout << "data:" << line.c_str() << endl;
//		istringstream templine(line);
//		string data;
//		while (getline(templine, data, ','))
//		{
//			cout << data.c_str() << endl;
//			reult.push_back(atof(data.c_str()));
//		}
//	}
//	file.close();
//	return reult;
//}
//
//vector<double> CdemoDlg::separateMat(vector<double> matrix, int column) {
//	vector<double> reult;
//	for (int i = 0; i < matrix.size(); i++) {
//		if (i % 3 == (column - 1)) {
//			reult.push_back(matrix[i]);
//		}
//	}
//	return reult;
//}
//
//UINT CdemoDlg::Bpnn(LPVOID pParam)
//{
//	//input_data	
//	double* p_strike_point;
//	double p_strike[3] = { 0 };
//	p_strike_point = p_strike;
//
//	double p_strike_point_x;
//	double p_strike_point_y;
//	double p_strike_point_z;
//	//vector<double>* input_data;
//	//Read input data
//	vector<double> matrix;
//	vector<double> matrix1;
//	vector<double> matrix2;
//	matrix = readfile("weightij.csv");	//DATA2.csv//try_and_error.txt
//	matrix1 = readfile("weightjk.csv");
//	matrix2 = readfile("weightkl.csv");
//
//	//Initial
//	//int a = (rand() % 10) + 1;
//	double weight[144] = { 0 };
//	double weight1[256] = { 0 };
//	double weight2[48] = { 0 };
//
//	vector<double> weightij = separateMat(matrix, 3);
//	vector<double> weightjk = separateMat(matrix1, 3);
//	vector<double> weightkl = separateMat(matrix2, 3);
//
//	for (int i = 0; i < 144; i++)
//	{
//		weight[i] = weightij[i];
//	}
//
//	for (int i = 0; i < 256; i++)
//	{
//		weight1[i] = weightjk[i];
//	}
//
//	for (int i = 0; i < 48; i++)
//	{
//		weight2[i] = weightkl[i];
//	}
//
//	//show_weight
//	//double *p_strike_point = { 0 };
//	calcuMSE(coll_data, weight, weight1, weight2);
//	//return outputdata;
//	for (int i = 0; i < 3; i++)
//	{
//		p_strike[i] = array_netl[i];
//	}
//	/*p_strike_point_x = p_strike[0] * (1067.04 - 133.765) + 133.765;
//	p_strike_point_y = p_strike[1] * (149.964 - 31.959) + 31.959;
//	p_strike_point_z = p_strike[2] * (459.717 - 24.9306) + 24.9306;*/
//	p_strike_point_x = (p_strike[0]-0.1) * (1243.52 - 54.6821) * (10 / 8) + 54.6821;
//	p_strike_point_y = (p_strike[1]-0.1) * (149.964 - 31.959)  * (10 / 8) + 31.959;
//	p_strike_point_z = (p_strike[2]-0.1) * (590.791 - 24.9306)  * (10 / 8) + 24.9306;
//
//	strike_point << p_strike_point_x << "," << p_strike_point_y << "," << p_strike_point_z << endl;
//	pBpnn = NULL;
//	return 0;
//}