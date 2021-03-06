#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

#define PI 3.141592
// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
  //From class tutorial
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd PHt = P_ * Ht;
  MatrixXd S = H_ * PHt + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */
  // 1. algo - convert stored x to polar. Hint: we need to apply jacobian as data won't be gaussian in z
  // 2. Apply kalman equations

  // Get px, py, vx, vy from x_
  float px = x_[0];
  float py = x_[1];
  float vx = x_[2];
  float vy = x_[3];

  float rho = sqrt(px*px + py*py);
  float phi = atan2(py,px);
  float rho_dot;
  if(rho < 0.0000001)
  {
  	rho = 0.0000001;
  }
  
  rho_dot = (px*vx+py*vy)/rho;
  
  VectorXd hx = VectorXd(3);
  hx << rho, phi, rho_dot;

  //Use equations from class
  VectorXd y = z - hx;
  //Normalize angle i.e. y(1)
  
  //Normalization: https://stackoverflow.com/questions/24234609/standard-way-to-normalize-an-angle-to-%CF%80-radians-in-java
  float width = 2.0 * PI; 
  float offsetValue = y(1) + PI;   // value relative to 0
  y(1) = (offsetValue - (floor(offsetValue / width) * width)) - PI;
  
  
  MatrixXd Ht = H_.transpose();
  MatrixXd PHt = P_ * Ht;
  MatrixXd S = H_ * PHt + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}
