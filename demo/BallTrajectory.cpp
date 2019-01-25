#include "stdafx.h"
#include"BallTrajectory.h"

//vector <CBallState> ball_state;

LstSquQuadRegr::LstSquQuadRegr()
{
	numOfEntries = 0;
	root1 = 0;
	root2 = 0;
}

void LstSquQuadRegr::CalculateCoefficient(){
	aTerm();
	bTerm();
	cTerm();
}

param LstSquQuadRegr::GetCoefficient(){
	return poly_coefficient;
}

bool LstSquQuadRegr::CalcQuadraticRoot(){  //(-b+-sqrt(b^2-4ac))/2a
	
	Real discriminant = pow(poly_coefficient.b, 2) - 4 * poly_coefficient.a*poly_coefficient.c; //b^2-4ac	//使用2階多項式的 b^2-4ac > 0 判斷是否有跟x交點兩次

	if (discriminant > 0 && poly_coefficient.a < 0){ //two roots and parabola opens downwards	//a需要為負號，軌跡才會是正拋物線；正號為反項
		root1 = (-poly_coefficient.b + sqrt(discriminant)) / (2 * poly_coefficient.a);
		root2 = (-poly_coefficient.b - sqrt(discriminant)) / (2 * poly_coefficient.a);
		return true;
	}
	else{  //discriminant =0 or <0  it's not the solution in ping pong ball ! 
		return false; //no solution 
	}

}

void LstSquQuadRegr::AddPoints(Real x, Real y)
{
	numOfEntries += 1;
	struct Ball temp;
	temp.x = x;
	temp.y = y;
	pointArray.push_back(temp);
}

void LstSquQuadRegr::aTerm()
{
	if (numOfEntries < 3)
	{
		/*throw new InvalidOperationException(
		"Insufficient pairs of co-ordinates");*/
	}
	//notation sjk to mean the sum of x_i^j*y_i^k. 
	Real s40 = getSx4(); //sum of x^4
	Real s30 = getSx3(); //sum of x^3
	Real s20 = getSx2(); //sum of x^2
	Real s10 = getSx();  //sum of x
	Real s00 = numOfEntries;
	//sum of x^0 * y^0  ie 1 * number of entries

	Real s21 = getSx2y(); //sum of x^2*y
	Real s11 = getSxy();  //sum of x*y
	Real s01 = getSy();   //sum of y

	//a = Da/D
	Real temp = (s21*(s20 * s00 - s10 * s10) -
		s11*(s30 * s00 - s10 * s20) +
		s01*(s30 * s10 - s20 * s20))
		/
		(s40*(s20 * s00 - s10 * s10) -
		s30*(s30 * s00 - s10 * s20) +
		s20*(s30 * s10 - s20 * s20));
	poly_coefficient.a = temp;

}

void LstSquQuadRegr::bTerm()
{
	if (numOfEntries < 3)
	{
		/*throw new InvalidOperationException(
		"Insufficient pairs of co-ordinates");*/
	}
	//notation sjk to mean the sum of x_i^j*y_i^k.
	Real s40 = getSx4(); //sum of x^4
	Real s30 = getSx3(); //sum of x^3
	Real s20 = getSx2(); //sum of x^2
	Real s10 = getSx();  //sum of x
	Real s00 = numOfEntries;
	//sum of x^0 * y^0  ie 1 * number of entries

	Real s21 = getSx2y(); //sum of x^2*y
	Real s11 = getSxy();  //sum of x*y
	Real s01 = getSy();   //sum of y

	//b = Db/D
	Real temp = (s40*(s11 * s00 - s01 * s10) -
		s30*(s21 * s00 - s01 * s20) +
		s20*(s21 * s10 - s11 * s20))
		/
		(s40 * (s20 * s00 - s10 * s10) -
		s30 * (s30 * s00 - s10 * s20) +
		s20 * (s30 * s10 - s20 * s20));
	poly_coefficient.b = temp;
	
}

void LstSquQuadRegr::cTerm()
{
	if (numOfEntries < 3)
	{
		/*throw new InvalidOperationException(
		"Insufficient pairs of co-ordinates");*/
	}
	//notation sjk to mean the sum of x_i^j*y_i^k.
	Real s40 = getSx4(); //sum of x^4
	Real s30 = getSx3(); //sum of x^3
	Real s20 = getSx2(); //sum of x^2
	Real s10 = getSx();  //sum of x
	Real s00 = numOfEntries;
	//sum of x^0 * y^0  ie 1 * number of entries

	Real s21 = getSx2y(); //sum of x^2*y
	Real s11 = getSxy();  //sum of x*y
	Real s01 = getSy();   //sum of y
	
	//c = Dc/D
	Real temp = (s40*(s20 * s01 - s10 * s11) -
		s30*(s30 * s01 - s10 * s21) +
		s20*(s30 * s11 - s20 * s21))
		/
		(s40 * (s20 * s00 - s10 * s10) -
		s30 * (s30 * s00 - s10 * s20) +
		s20 * (s30 * s10 - s20 * s20));
	poly_coefficient.c = temp;
	
}

Real LstSquQuadRegr::getSx() // get sum of x
{
	Real Sx = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sx += pointArray[i].x;
	}
	return Sx;
}

Real LstSquQuadRegr::getSy() // get sum of y
{
	Real Sy = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sy += pointArray[i].y;
	}
	return Sy;
}

Real LstSquQuadRegr::getSx2() // get sum of x^2
{
	Real Sx2 = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sx2 += pow(pointArray[i].x, 2);
	}
	return Sx2;
}

Real LstSquQuadRegr::getSx3() // get sum of x^3
{
	Real Sx3 = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sx3 += pow(pointArray[i].x, 3);
	}
	return Sx3;
}

Real LstSquQuadRegr::getSx4() // get sum of x^4
{
	Real Sx4 = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sx4 += pow(pointArray[i].x, 4);
	}
	return Sx4;
}

Real LstSquQuadRegr::getSxy() // get sum of x*y
{
	Real Sxy = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sxy += pointArray[i].x * pointArray[i].y;
	}
	return Sxy;
}

Real LstSquQuadRegr::getSx2y() // get sum of x^2*y
{
	Real Sx2y = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		Sx2y += pow(pointArray[i].x, 2) * pointArray[i].y;
	}
	return Sx2y;
}

Real LstSquQuadRegr::getYMean() // mean value of y
{
	Real y_tot = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		y_tot += pointArray[i].y;
	}
	return y_tot / numOfEntries;
}

Real LstSquQuadRegr::rSquare() // get r-squared
{
	if (numOfEntries < 3)
	{
		/*throw new InvalidOperationException(
		"Insufficient pairs of co-ordinates");*/
	}
	// 1 - (residual sum of squares / total sum of squares)
	return 1 - getSSerr() / getSStot();
}

Real LstSquQuadRegr::getSStot() // total sum of squares
{
	//the sum of the squares of the differences between 
	//the measured y values and the mean y value
	Real ss_tot = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		ss_tot += pow(pointArray[i].x - getYMean(), 2);
	}

	return ss_tot;
}

Real LstSquQuadRegr::getSSerr() // residual sum of squares
{
	//the sum of the squares of te difference between 
	//the measured y values and the values of y predicted by the equation
	Real ss_err = 0;

	for (unsigned int i = 0; i < pointArray.size(); i++){
		ss_err += pow(pointArray[i].y - getPredictedY(pointArray[i].x), 2);
	}

	return ss_err;
}

Real LstSquQuadRegr::getPredictedY(Real x)
{
	//returns value of y predicted by the equation for a given value of x

	return poly_coefficient.a * pow(x, 2) + poly_coefficient.b * x + poly_coefficient.c;
}

////新撰寫
//void LstSquQuadRegr::aTerm_x()
//{
//	if (numOfEntries < 3)
//	{
//		/*throw new InvalidOperationException(
//		"Insufficient pairs of co-ordinates");*/
//	}
//	
//	Real temp = (pointArray[numOfEntries].x - pointArray[0].x)/(ball_state[numOfEntries].GetTimestamp_t() - ball_state[0].GetTimestamp_t());
//	poly_coefficient.a = temp;
//}
//
//void LstSquQuadRegr::bTerm_y()
//{
//	if (numOfEntries < 3)
//	{
//		/*throw new InvalidOperationException(
//		"Insufficient pairs of co-ordinates");*/
//	}
//	Real temp = (pointArray[0].y) - (poly_coefficient.a)*(ball_state[0].GetTimestamp_t());
//	poly_coefficient.b = temp;
//}

