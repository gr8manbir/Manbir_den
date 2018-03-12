#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
	this->Kp = Kp;
	this->Ki = Ki;
	this_>Kd = Kd;
	
	p_error = 0.0;
	i_error = 0.0;
	d_error = 0.0;
	
}

void PID::UpdateError(double cte) {
	double previous_cte = p_error; // Note: p_error keeps track of cte in previous cycle 
	p_error = cte;
	i_error += cte;
	d_error = cte - previous_cte;
}

double PID::TotalError() {
	
	 return (-kp*p_error - ki*i_error - kd*d_error);
}

