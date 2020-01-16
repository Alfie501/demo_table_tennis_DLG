#include "stdafx.h"
#include "Stereo.h"
using namespace mao_style;

#if 1
//***intrinsic parameters***// 
//float fu_left = 1771.29450, fv_left = 1772.26299, u0_left = 1050.96239, v0_left = 794.69331;
//float fu_right = 1767.04086, fv_right = 1767.46966, u0_right = 1065.93038, v0_right = 786.28660;
float fu_left = 1780.67243, fv_left = 1779.40209, u0_left = 1044.47612, v0_left = 794.57946;
float fu_right = 1773.81215, fv_right = 1773.34476, u0_right = 1055.18051, v0_right = 787.23829;
  
//***iamge correction parameters***//		  %----matlab image_correction_new get "p" parameter
//float kc_left[8] = { -0.004907231147251, 0.002960456029719, -0.001660297660146, 2.400318452386080e-04, 0.004153775267758, -0.002827985759285, 0.001212721016294, -1.664398110085841e-04 };
//float kc_right[8] = { -0.007910210851360, 0.014615384935610, -7.196922805089618e-04, -6.799936222742662e-04, 0.006322570670544, -0.012293632699295, 4.272529887890987e-04, 4.354500380622517e-04 };
float kc_left[8] = { -0.006353046506854, 0.005594470149711, -0.001619778193004, 0.001010184309294, 0.004972125217032, -0.004470955971120, 0.001047339819927, -7.170590870094057e-04 };
float kc_right[8] = { -0.011404124580104, 0.022973922972679, -0.001068390511635, 7.001414550224595e-04, 0.008146325572365, -0.017932116182866, 4.309545280572341e-04, -5.575917189171165e-04 };

//***extrinsic parameters between left camera and right camera***//
//extrinsic parameters from matlab 
//rotation matrix : left rotate to right (R_RL), same as matlab calibration output			%----matlab calibration output

//float R_rl[3][3] = {					
//			{ 0.9873, -0.0022, 0.1590},
//			{ 0.0019, 1.0000, 0.0022 },
//			{ -0.1590, -0.0018, 0.9873 }
//};
float R_rl[3][3] = {
			{ 0.9878, -0.0022,  0.1557},
			{ 0.0019,  1.0000,  0.0019 },
			{ -0.1557, -0.0016,  0.9878 }
};


//float b_l[3] = { 335.9368, -0.7154, -0.6220 };
float b_l[3] = { 336.5182, -0.9323, -1.3177 };

// calcultae by => b_l= -R_LR*b_r  , b_r is given from matlab		%--- b_r is given from matlab " T "  //R_LR = inv(R_rl)
//caclculate by b_l = (inv((R_rl')*R_rl)*(R_rl'))*b_r  ??  b_l= -R_LR*b_r    the same...  2018-10 


//***extrinsic parameter between left camera and world coordinate***//

//translation : vector from camera to world coordinate in camera coordinate
//float T_c[3] = { -842.449940, 485.552512, 1496.50269 };
//float T_c[3] = { -843.842244, 	 492.162807, 	 1499.521777 };
float T_c[3] = { -850.386192,	 487.585502 ,	 1503.391583 };

//rotation matrix:  world coordinate rotate to left camera coordinate
//float R_cw[3][3] = { {0.999784, - 0.020291,	 0.004488 },								//		%------Extrinsic_parameters
//					 { -0.005652, -0.473340, -0.880862 },
//					 { 0.019998, 0.880646, -0.473352 } };
//float R_cw[3][3] = { {0.999626, - 0.027010, 	 0.004382 },								//		%------Extrinsic_parameters
//					 {-0.008919, - 0.473020, - 0.881007},
//					 { 0.025869, 	 0.880638, - 0.473084 } };
float R_cw[3][3] = { {0.999678, - 0.025318, 	 0.001591 },								//		%------Extrinsic_parameters
					 {-0.010625, - 0.474812, - 0.880023},
					 { 0.023036, 	 0.879723, - 0.474928  } };
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
	k = (RotationMatrix_RL[0][0] / Camera_left.fu) * (Camera_left.Ix - Camera_left.u0) +
		(RotationMatrix_RL[0][1] / Camera_left.fv) * (Camera_left.Iy - Camera_left.v0) + RotationMatrix_RL[0][2] -
		((Camera_right.Ix - Camera_right.u0) / Camera_right.fu) *
		((RotationMatrix_RL[2][0] / Camera_left.fu) * (Camera_left.Ix - Camera_left.u0) +
		(RotationMatrix_RL[2][1] / Camera_left.fv) * (Camera_left.Iy - Camera_left.v0) + RotationMatrix_RL[2][2]);

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