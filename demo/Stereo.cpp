#include "stdafx.h"
#include "Stereo.h"
using namespace mao_style;

#if 1
//***intrinsic parameters***// 
//float fu_left = 1807.76922, fv_left = 1808.34961, u0_left = 1049.16958, v0_left = 823.13503;
//float fu_right = 1780.65692, fv_right = 1789.44654, u0_right = 1016.83872, v0_right = 843.98094;
float fu_left = 1774.40462, fv_left = 1774.41522, u0_left = 1044.21694, v0_left = 791.24719;
float fu_right = 1770.49819, fv_right = 1771.21403, u0_right = 1058.79225, v0_right = 783.14317 ;
  
//***iamge correction parameters***//		  %----matlab image_correction_new get "p" parameter
//float kc_left[8] = { -3.759209778413423e-05, 0.012099647989555, -0.003430944663772, 4.996802562484074e-05, 9.203012505855646e-04, -0.010204633289137, 0.002757224228166, -4.104620807973691e-05 };
//float kc_right[8] = { -0.016847931046896, 0.020687972062559, -0.010455112452432, 0.002761616897140, 0.014292540726086, -0.018127270998962, 0.007699926422002, -0.001874244202830 };
float kc_left[8] = { -0.00342513225276257, 0.00482606693792004, -0.000869624952719960, 0.00162981708278467, 0.00224064170368965, -0.00350457317543862, 0.000507473126696585, -0.00122711100596783 };
float kc_right[8] = { -0.00417866607115308, 0.00511725234683099, -0.000589842352381994, 0.000360060846119106, 0.00318746564439992, -0.00395742866486323, 0.000327094938515592, -0.000261444639773317 };

//***extrinsic parameters between left camera and right camera***//
//extrinsic parameters from matlab 
//rotation matrix : left rotate to right (R_RL), same as matlab calibration output			%----matlab calibration output
//float R_rl[3][3] = {
//			{ 0.9817, -0.0033, 0.1903},
//			{ 0.0062, 0.9999, -0.0150 },
//			{ -0.1902, 0.0159, 0.9816 }
//};
float R_rl[3][3] = {
			{ 0.9870, -0.0016, 0.1607},
			{ 0.0014, 1.0000, 0.0011 },
			{ -0.1607, -0.0008, 0.9870 }
};

float b_l[3] = { 335.3433, -2.0855, -1.9144 };
//float b_l[3] = { 333.1572, -3.0507, 12.0387 };     // calcultae by => b_l= -R_LR*b_r  , b_r is given from matlab		%--- b_r is given from matlab " T " // R_LR = inv(R_rl)
													//caclculate by b_l = (inv((R_rl')*R_rl)*(R_rl'))*b_r  ??  b_l= -R_LR*b_r    the same...  2018-10 
//***extrinsic parameter between left camera and world coordinate***//

//translation : vector from camera to world coordinate in camera coordinate
//float T_c[3] = { -653.412609, 712.541980, 1830.124562 };
//float T_c[3] = { -641.5179950898146, 313.9662562701072, 1238.0834001743515 };								//		%------Extrinsic_parameters
float T_c[3] = { -660.824314,353.179300,1249.172265 };

//rotation matrix:  world coordinate rotate to left camera coordinate
//float R_cw[3][3] = { { 0.997897, -0.053149, 0.037096 },
//					{ -0.025434, -0.847544, -0.530115 },
//					{ 0.059615, 0.528057, -0.847114 } };

//float R_cw[3][3] = { {0.999655, 0.025787, 0.005002 },								//		%------Extrinsic_parameters
//					 { 0.011729, -0.267821, -0.963397 },
//					 { -0.023503, 0.963124, -0.268031 } };
float R_cw[3][3] = { {0.998792, 0.048559, 0.007556 },								//		%------Extrinsic_parameters
					 { 0.021397, -0.291292, -0.956395 },
					 { -0.044240, 0.955401, -0.291979 } };


//rotation matrix:  left camera coordinate rotate to world coordinate
float R_wc[3][3] = { { R_cw[0][0], R_cw[1][0], R_cw[2][0] },
					 { R_cw[0][1], R_cw[1][1], R_cw[2][1] },
					 { R_cw[0][2], R_cw[1][2], R_cw[2][2] } };

////***intrinsic parameters***// 
//float fu_left = 941.4005, fv_left = 941.8335, u0_left = 620.7234, v0_left = 531.9934;
//float fu_right = 940.9074, fv_right = 941.3631, u0_right = 633.8747, v0_right = 515.102;
//
////***iamge correction parameters***//
//float kc_left[8] = { 0.21529, -0.13528, -0.001032, -0.000175, -0.20009, 0.10544, 0.00050916, 8.0302e-05 };
//float kc_right[8] = { 0.21792, -0.14307, -0.00096392, -0.00058942, -0.1961, 0.11941, 0.00046166, 0.00027434 };
//
////***extrinsic parameters between left camera and right camera***//
////extrinsic parameters from matlab 
////rotation matrix : left rotate to right (R_RL), same as matlab calibration output
//float R_rl[3][3] = {
//	{ 0.99017, -0.08038, 0.1145 },
//{ 0.079731, 0.99676, 0.01025 },
//{ -0.11496, -0.0010198, 0.99337 }
//};
//
//float b_l[3] = { 275.9431, -14.9019, 16.919 };     // calcultae by => b_l= -R_LR*b_r  , b_r is given from matlab 
//
//												   //***extrinsic parameter between left camera and world coordinate***//
//
//												   //translation : vector from camera to world coordinate in camera coordinate
//												   //float T_c[3] = { -653.412609, 712.541980, 1830.124562 };
//float T_c[3] = { -619.828647, 696.170966, 1831.233900 };
//
////rotation matrix:  world coordinate rotate to left camera coordinate
////float R_cw[3][3] = { { 0.997897, -0.053149, 0.037096 },
////					{ -0.025434, -0.847544, -0.530115 },
////					{ 0.059615, 0.528057, -0.847114 } };
//
//float R_cw[3][3] = { { 0.997768, -0.058627, 0.031967 },
//{ -0.031273, -0.833236, -0.552032 },
//{ 0.059000, 0.549800, -0.833210 } };
//
//
//
////rotation matrix:  left camera coordinate rotate to world coordinate
//float R_wc[3][3] = { { R_cw[0][0], R_cw[1][0], R_cw[2][0] },
//{ R_cw[0][1], R_cw[1][1], R_cw[2][1] },
//{ R_cw[0][2], R_cw[1][2], R_cw[2][2] } };
#else if  //test 萱仔的 webcam
//intrinsic parameters 
float fu_left = 719.51769, fv_left = 725.16828, u0_left = 349.29896, v0_left = 192.04074;
float fu_right = 702.99390, fv_right = 704.05213, u0_right = 307.39433, v0_right = 217.18589;
//iamge correction parameters

float kc_left[8] = { -0.1231, 0.2560, 0.0220, -0.0068, 0.0977, -0.2055, -0.0156, 0.0045 };
float kc_right[8] = { -0.1001, 0.0731, 0.0172, 0.0054, 0.0766, -0.0596, -0.0120, -0.0035 };

//extrinsic parameters
float R[3][3] = {
	{ 0.8619, 0.1106, 0.4948 },
	{ -0.0538, 0.9904, -0.1277 },
	{ -0.5042, 0.0835, 0.8596 }
};

float b[3] = { 173.02974, 0.92719, -31.15577 };

#endif

Camera::Camera(){ cout << "it's camera constructor" << endl; }
Camera::Camera(float camera_fu, float camera_fv, float camera_u0, float camera_v0, float camera_kc[8]){
	cout << "it's camera constructor with initial parameter" << endl;
	fu = camera_fu;
	fv = camera_fv;
	u0 = camera_u0;
	v0 = camera_v0;
	kc1 = camera_kc[0];
	kc2 = camera_kc[1];
	kc3 = camera_kc[2];
	kc4 = camera_kc[3];
	kc5 = camera_kc[4];
	kc6 = camera_kc[5];
	kc7 = camera_kc[6];
	kc8 = camera_kc[7];
}
Camera::~Camera(){ cout << "destructor has been called" << endl; }

void Camera::modify_modal(){
	cout << endl << "Idx: " << Idx << " Idy: " << Idy << endl;

	h_dxz = (Idx - u0) / fu;
	h_dyz = (Idy - v0) / fv;
	
	float rd = sqrt(pow(h_dxz, 2) + pow(h_dyz, 2));												//sqrt 根號    pow 次方
	float G = 4 * kc5*pow(rd, 2) + 6 * kc6*pow(rd, 4) + 8 * kc7*h_dyz + 8 * kc8*h_dxz + 1;

	h_xz = h_dxz + 1 / G*((kc1*pow(rd, 2) + kc2*pow(rd, 4))*h_dxz + (2 * kc3*h_dxz*h_dyz) + kc4*(pow(rd, 2) + 2 * pow(h_dxz, 2)));
	h_yz = h_dyz + 1 / G*((kc1*pow(rd, 2) + kc2*pow(rd, 4))*h_dyz + (2 * kc4*h_dxz*h_dyz) + kc3*(pow(rd, 2) + 2 * pow(h_dyz, 2)));

	Ix = h_xz*fu + u0;
	Iy = h_yz*fv + v0;

	cout << "G:" << G << endl;
	cout << "the modified Ix,Iy is:" << endl;
	cout << "Ix: " << Ix << " Iy: " << Iy << endl;
}

Stereo::Stereo(){ 
	
	cout << "it's Stereo constructor" << endl; 
	memset(d, 0, sizeof (d));//initialize the array d[3]

}

void Stereo::setCamera(LeftOrRight choose, float camera_fu, float camera_fv, float camera_u0, float camera_v0, float camera_kc[8]){

	if (choose == LeftCamera){//left Camera
		cout << "setting Left camera initial parameter" << endl;

		Camera_left.fu = camera_fu;
		Camera_left.fv = camera_fv;
		Camera_left.u0 = camera_u0;
		Camera_left.v0 = camera_v0;
		Camera_left.kc1 = camera_kc[0];
		Camera_left.kc2 = camera_kc[1];
		Camera_left.kc3 = camera_kc[2];
		Camera_left.kc4 = camera_kc[3];
		Camera_left.kc5 = camera_kc[4];
		Camera_left.kc6 = camera_kc[5];
		Camera_left.kc7 = camera_kc[6];
		Camera_left.kc8 = camera_kc[7];
	}
	if (choose == RightCamera){//Right Camera
		cout << "setting Right camera initial parameter" << endl;

		Camera_right.fu = camera_fu;
		Camera_right.fv = camera_fv;
		Camera_right.u0 = camera_u0;
		Camera_right.v0 = camera_v0;
		Camera_right.kc1 = camera_kc[0];
		Camera_right.kc2 = camera_kc[1];
		Camera_right.kc3 = camera_kc[2];
		Camera_right.kc4 = camera_kc[3];
		Camera_right.kc5 = camera_kc[4];
		Camera_right.kc6 = camera_kc[5];
		Camera_right.kc7 = camera_kc[6];
		Camera_right.kc8 = camera_kc[7];

	}

}

void Stereo::initialize(float R[3][3], float b[3]){

	for (int i = 0; i<3; i++){
		for (int j = 0; j<3; j++){
			RotationMatrix_RL[i][j] = R[i][j];
		}
		b_L[i] = b[i];
	}
	//RotationMatrix_RL[][]

	for (int i = 0; i<3; i++){
		for (int j = 0; j<3; j++){
			d[i] += RotationMatrix_RL[i][j] * b_L[j];
		}
		//cout<<d[i]<<endl;
	}

}

void Stereo::NON_SSG(){
#if 1
	k = (RotationMatrix_RL[0][0] / Camera_left.fu)*(Camera_left.Ix - Camera_left.u0) +
		(RotationMatrix_RL[0][1] / Camera_left.fv)*(Camera_left.Iy - Camera_left.v0) + RotationMatrix_RL[0][2] -
		((Camera_right.Ix - Camera_right.u0) / Camera_right.fu)*
		(RotationMatrix_RL[2][0] / Camera_left.fu*(Camera_left.Ix - Camera_left.u0) +
		RotationMatrix_RL[2][1] / Camera_left.fv*(Camera_left.Iy - Camera_left.v0) + RotationMatrix_RL[2][2]);

	Camera_left.h_z = (1 / k)*(d[0] - d[2] * (Camera_right.Ix - Camera_right.u0) / Camera_right.fu);
	Camera_left.h_x = (1 / k)*(d[0] - d[2] * (Camera_right.Ix - Camera_right.u0) / Camera_right.fu)*(Camera_left.Ix - Camera_left.u0) / Camera_left.fu;
	Camera_left.h_y = (1 / k)*(d[0] - d[2] * (Camera_right.Ix - Camera_right.u0) / Camera_right.fu)*(Camera_left.Iy - Camera_left.v0) / Camera_left.fv;
#else if  //test ,課本的另一個推導，但我似乎有推錯
	k = (RotationMatrix_RL[1][0] / Camera_left.fu)*(Camera_left.Ix - Camera_left.u0) +
		(RotationMatrix_RL[1][1] / Camera_left.fv)*(Camera_left.Iy - Camera_left.v0) + RotationMatrix_RL[1][2] -
		((Camera_right.Iy - Camera_right.v0) / Camera_right.fv)*
		(RotationMatrix_RL[2][0] / Camera_left.fu*(Camera_left.Ix - Camera_left.u0) +
		RotationMatrix_RL[2][1] / Camera_left.fv*(Camera_left.Iy - Camera_left.v0) + RotationMatrix_RL[2][2]);

	Camera_left.h_z = (1 / k)*(d[1] - d[2] * (Camera_right.Iy - Camera_right.v0) / Camera_right.fv);
	Camera_left.h_x = (1 / k)*(d[1] - d[2] * (Camera_right.Iy - Camera_right.v0) / Camera_right.fv)*(Camera_left.Ix - Camera_left.u0) / Camera_left.fu;
	Camera_left.h_y = (1 / k)*(d[1] - d[2] * (Camera_right.Iy - Camera_right.v0) / Camera_right.fv)*(Camera_left.Iy - Camera_left.v0) / Camera_left.fv;
#endif

}