#include <iostream>
#include <math.h>
using namespace std;

namespace mao_style{

	typedef enum{
		LeftCamera,
		RightCamera
	}LeftOrRight;

	class Camera{
	public:
		Camera();
		Camera(float camera_fu, float camera_fv, float camera_u0, float camera_v0, float camera_kc[8]);
		float Ix, Iy;
		float Idx, Idy;
		float fu, fv;//need initialize 
		float u0, v0;//need initialize
		float h_x, h_y, h_z;
		float h_dx, h_dy, h_dz;
		float h_xz, h_yz;    // h_xz=h_x/h_z;
		float h_dxz, h_dyz;
		float kc1, kc2, kc3, kc4, kc5, kc6, kc7, kc8;   //need initialize
		void modify_modal();
		~Camera();
	};

	class Stereo{
	public:
		Stereo();
		Camera Camera_left, Camera_right;
		void setCamera(LeftOrRight choose, float camera_fu, float camera_fv, float camera_u0, float camera_v0, float camera_kc[8]);
		void initialize(float RotationMatrix[3][3], float translation[3]);
		void NON_SSG();
		~Stereo(){}
	private:
		float RotationMatrix_RL[3][3];//left camera with respect to right camera
		float b_L[3];//translation vector from left camera to the right camera
		float d[3];
		float k=0;
		
	};


}