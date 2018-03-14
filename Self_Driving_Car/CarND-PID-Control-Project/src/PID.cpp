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
	this->Kd = Kd;
	
	//Init all errors to zero
	p_error = 0.0;
	i_error = 0.0;
	d_error = 0.0;
	
}

void PID::UpdateError(double cte) {
	double previous_cte = p_error; // Note: p_error keeps track of cte in previous cycle
    // The proportional part of the error is the cross track error itself.	
	p_error = cte;
	// The integral part is to cumulatively add cte, so as to bring it closer to zero.
	i_error += cte;
	// The derivative part reduces current cte by the previous cte. This ensures that car smoothly transitions.
	d_error = cte - previous_cte;
}

double PID::TotalError() {
	
	 return (-Kp*p_error - Ki*i_error - Kd*d_error);
}

