/*
	Given data sets of (x1,y1),(x2,y2),(x3,y3)...at least three sets 
	Solving a,b and c for the quadratic polynominal y(x)=ax^2+bx+c
*/
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <Windows.h>

//*** for float and double precision ***// 
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <limits>
#include "Eigen/eigen"

using namespace boost;
using namespace boost::multiprecision;
using namespace Eigen;
typedef number<cpp_dec_float<30, int32_t> > Real;

using namespace std;

struct Ball{   
	Real x = 0;
	Real y = 0;
};

struct param{
	Real a = 0;
	Real b = 0;
	Real c = 0;
};

class LstSquQuadRegr
{
	
private:
	param poly_coefficient;

public:
	void CalculateCoefficient();

	param GetCoefficient();

	Real root1, root2;
	bool CalcQuadraticRoot();  //(-b+-sqrt(b^2-4ac))/2a   return 1 => has solution  return 0 => no solution 
		
	
public:
	vector <Ball> pointArray;
private:
	int numOfEntries;
	Real *pointpair;

	/*constructor */
public:
	LstSquQuadRegr();
	
public:
	void AddPoints(Real x, Real y);
	
public:
	void aTerm();
	void bTerm();
	void cTerm();
	
public:
	Real rSquare(); // get r-squared
	
private:
	Real getSx(); // get sum of x
	
private:
	Real getSy(); // get sum of y
	
private:
	Real getSx2(); // get sum of x^2
	
private:
	Real getSx3(); // get sum of x^3
	
private:
	Real getSx4(); // get sum of x^4
	
private:
	Real getSxy(); // get sum of x*y
	
private:
	Real getSx2y(); // get sum of x^2*y
	
private:
	Real getYMean(); // mean value of y
	
private:
	Real getSStot(); // total sum of squares
	
private:
	Real getSSerr(); // residual sum of squares
	
public:
	Real getPredictedY(Real x);

public:
	void aTerm_x();
	void bTerm_y();
	void cTerm_z();

};

//class CBallState {
//private:
//	double x, y, z;  //position in world coordinate 
//	double t;		//timestamp
//	unsigned int i; //index number 
//	double vx, vy, vz;
//public:
//	void SetBallState(unsigned int index, double x_value, double y_value, double z_value, double t_value);
//	void SetBallState(unsigned int index, double x_value, double y_value, double z_value, double Velocity_x, double Velocity_y, double Velocity_z, double t_value);
//	double GetPosition_x() { return x; }
//	double GetPosition_y() { return y; }
//	double GetPosition_z() { return z; }
//	double GetTimestamp_t() { return t; }
//	unsigned int GetIndex() { return i; }
//	double GetVelocity_x() { return vx; }
//	double GetVelocity_y() { return vy; }
//	double GetVelocity_z() { return vz; }
//
//};



