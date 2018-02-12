#include "ukf.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/**
 * Initializes Unscented Kalman filter
 * This is scaffolding, do not modify
 */
UKF::UKF() {
  // if this is false, laser measurements will be ignored (except during init)
  use_laser_ = true;

  // if this is false, radar measurements will be ignored (except during init)
  use_radar_ = true;

  // initial state vector
  x_ = VectorXd(5);

  // initial covariance matrix
  P_ = MatrixXd(5, 5);

  // Process noise standard deviation longitudinal acceleration in m/s^2
  std_a_ = 30;

  // Process noise standard deviation yaw acceleration in rad/s^2
  std_yawdd_ = 30;
  
  //DO NOT MODIFY measurement noise values below these are provided by the sensor manufacturer.
  // Laser measurement noise standard deviation position1 in m
  std_laspx_ = 0.15;

  // Laser measurement noise standard deviation position2 in m
  std_laspy_ = 0.15;

  // Radar measurement noise standard deviation radius in m
  std_radr_ = 0.3;

  // Radar measurement noise standard deviation angle in rad
  std_radphi_ = 0.03;

  // Radar measurement noise standard deviation radius change in m/s
  std_radrd_ = 0.3;
  //DO NOT MODIFY measurement noise values above these are provided by the sensor manufacturer.
  
  /**
  TODO:

  Complete the initialization. See ukf.h for other member properties.

  Hint: one or more values initialized above might be wildly off...
  */

  n_x_ = 5;
  n_aug_ = 7;
  lambda_ = 3 - n_aug_;
  weights_ = VectorXd( 2*n_aug_ + 1);
  is_initialized_ = false;
}

UKF::~UKF() {}

/**
 * @param {MeasurementPackage} meas_package The latest measurement data of
 * either radar or laser.
 */
void UKF::ProcessMeasurement(MeasurementPackage meas_package) {
  /**
  TODO:

  Complete this function! Make sure you switch between lidar and radar
  measurements.
  */
  if(!is_initialized_)
  {
	  P_ << 1, 0, 0, 0, 0,
            0, 1, 0, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 0, 1, 0,
            0, 0, 0, 0, 1;		  
      if (meas_package.sensor_type_ == MeasurementPackage::RADAR) 
      {
          float rho = meas_package.raw_measurements_[0];
          float phi = meas_package.raw_measurements_[1];
          float rhoDot = meas_package.raw_measurements_[2];
	      float v = 0.0;
          /**
           Convert radar from polar to cartesian coordinates and initialize state.
           */
          /* x is vertical axis, y is horizontal. Cos(angle) = adj/hyp. Therefore: */
          px = rho *cos(phi);
          py = rho *sin(phi);
	      vx = rhoDot*cos(phi);
	      vy = rhoDot*sin(phi);
	      v = sqrt(vx*vx+vy*vy);
	      x_<<px,py,v,0.0,0.0
      } 
      else if (meas_package.sensor_type_ == MeasurementPackage::LASER) 
      {
          // LiDAR has no velocity component
          x_ << meas_package.raw_measurements_(0), meas_package.raw_measurements_(1), 0.0, 0.0, 0.0;
      }
	  
	  is_initialized_ = true;
	  time_us_ = meas_package.timestamp_;
	  return;
  }
  
  /* Prediction step */
  double dt = meas_package.timestamp_ - time_us_;
  Prediction(dt);
  
  /* Measurement update step */
  if(meas_package.sensor_type_ == MeasurementPackage::RADAR)
  {
	  /* Radar update */
	  UpdateRadar(meas_package);
  }
  else
  {
	  /* LiDar update */
	  UpdateLidar(meas_package);
  }
}

/**
 * Predicts sigma points, the state, and the state covariance matrix.
 * @param {double} delta_t the change in time (in seconds) between the last
 * measurement and this one.
 */
void UKF::Prediction(double delta_t) {
  /**
  TODO:

  Complete this function! Estimate the object's location. Modify the state
  vector, x_. Predict sigma points, the state, and the state covariance matrix.
  */
  /* Calculate mean, co-variance i.e. x_ and p_ 
   * Mean depends on two vectors. first vector is state vector from sigma points( predicted )
   * second is noise vector calculated from sigma points augmented
   */
 #if 0  
   /* 1. Calculate Sigma points for vector 1 */
   MatrixXd Xsig = MatrixXd(n_x_, 2*n_x_+1);
   MatrixXd A = P.llt().matrixL();
   
   Xsig.col(0) = x_; //First sigma point is mean
   for( int i=0; i<n_x_; i++ )
   {
	   Xsig.col(i+1)     = x_ + sqrt(lambda+n_x) * A.col(i);
       Xsig.col(i+1+n_x) = x_ - sqrt(lambda+n_x) * A.col(i);
   }
 #endif
   /* 1. Calculate augmentation vector and co-variance for vector 2 i.e. noise */
   MatrixXd Xsig_aug = MatrixXd(n_aug_, 2*n_aug_+1);
   MatrixXd P_aug = MatrixXd(n_aug_, n_aug_);
   
   /* Similar to x_ create x_aug_ with last two values zero */
   VectorXd x_aug_ = VectorXd(n_aug_);
   x_aug_.head(5) = x_;
   x_aug_(5) = 0;
   x_aug_(6) = 0;
   
   /* Fill p_aug with values */
   P_aug.fill(0.0);
   P_aug.topLeftCorner(5,5) = P;
   P_aug(5,5) = std_a_*std_a_;
   P_aug(6,6) = std_yawdd_*std_yawdd_;
   
   /* Square root matrix */
   MatrixXd L = P_aug.llt().matrixL;
   
   /* Create augmented sigma points */
   Xsig_aug.col(0) = x_aug_;
   for( i = 0; i < n_aug_; i++ )
   {
       Xsig_aug.col(i+1)       = x_aug + sqrt(lambda+n_aug_) * L.col(i);
       Xsig_aug.col(i+1+n_aug_) = x_aug - sqrt(lambda+n_aug_) * L.col(i);
   }
   
   /* 2. Predict sigma points */
   for (int i = 0; i< 2*n_aug_+1; i++)
   {
       //extract values for better readability
       double p_x = Xsig_aug(0,i);
       double p_y = Xsig_aug(1,i);
       double v = Xsig_aug(2,i);
       double yaw = Xsig_aug(3,i);
       double yawd = Xsig_aug(4,i);
       double nu_a = Xsig_aug(5,i);
       double nu_yawdd = Xsig_aug(6,i);

       //predicted state values
       double px_p, py_p;

       //avoid division by zero
       if (fabs(yawd) > 0.001) {
          px_p = p_x + v/yawd * ( sin (yaw + yawd*delta_t) - sin(yaw));
          py_p = p_y + v/yawd * ( cos(yaw) - cos(yaw+yawd*delta_t) );
       }
       else {
          px_p = p_x + v*delta_t*cos(yaw);
          py_p = p_y + v*delta_t*sin(yaw);
       }

       double v_p = v;
       double yaw_p = yaw + yawd*delta_t;
       double yawd_p = yawd;

       //add noise
       px_p = px_p + 0.5*nu_a*delta_t*delta_t * cos(yaw);
       py_p = py_p + 0.5*nu_a*delta_t*delta_t * sin(yaw);
       v_p = v_p + nu_a*delta_t;

       yaw_p = yaw_p + 0.5*nu_yawdd*delta_t*delta_t;
       yawd_p = yawd_p + nu_yawdd*delta_t;

       //write predicted sigma point into right column
       Xsig_pred(0,i) = px_p;
       Xsig_pred(1,i) = py_p;
       Xsig_pred(2,i) = v_p;
       Xsig_pred(3,i) = yaw_p;
       Xsig_pred(4,i) = yawd_p;
   }
  
    /* 3. From sigma point predictions at time t+dt, calculate new mean and co-variance */
    // set weights
    weights_ = VectorXd(2*n_aug_+1);
    double weight_0 = lambda/(lambda+n_aug_);
    weights_(0) = weight_0;
    for (int i=1; i<2*n_aug_+1; i++) {  //2n+1 weights
    double weight = 0.5/(n_aug_+lambda);
       weights_(i) = weight;
    }
	// Calculate mean
	x_.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) 
	{  
        //iterate over sigma points
        x_ = x_ + weights_(i) * Xsig_pred.col(i);
    }
	
	//Calculate co-variance
	P_.fill(0.0);
    for ( i = 0; i < 2 * n_aug_ + 1; i++) {  //iterate over sigma points

        // state difference
        VectorXd x_diff = Xsig_pred.col(i) - x_;
        //angle normalization
        while (x_diff(3)> M_PI) x_diff(3)-=2.*M_PI;
        while (x_diff(3)<-M_PI) x_diff(3)+=2.*M_PI;

        P_ = P_ + weights_(i) * x_diff * x_diff.transpose() ;
    }
}

/**
 * Updates the state and the state covariance matrix using a laser measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateLidar(MeasurementPackage meas_package) {
  /**
  TODO:

  Complete this function! Use lidar data to update the belief about the object's
  position. Modify the state vector, x_, and covariance, P_.

  You'll also need to calculate the lidar NIS.
  */
    /*Measurement space depends on predicted sigma points and weights. No need to recalculate */
  int n_z = 2;
  
  MatrixXd Zsig = MatrixXd(n_z, 2 * n_aug_ + 1);
  //transform sigma points into measurement space
  for (int i = 0; i < 2 * n_aug_ + 1; i++) 
  {  
      //2n+1 sigma points

      // extract values for better readability
      double p_x = Xsig_pred(0,i);
      double p_y = Xsig_pred(1,i);

      // measurement model
      Zsig(0,i) = p_x;                        //px
      Zsig(1,i) = p_y;                        //py
  }
  
  //mean predicted measurement
  VectorXd z_pred = VectorXd(n_z);
  z_pred.fill(0.0);
  for (int i=0; i < 2*n_aug_+1; i++) {
      z_pred = z_pred + weights_(i) * Zsig.col(i);
  }

  //innovation covariance matrix S
  MatrixXd S = MatrixXd(n_z,n_z);
  S.fill(0.0);
  //cross corelation matrix
  MatrixXd Tc = MatrixXd(n_x, n_z);
  Tc.fill(0.0);
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 sigma points
    //residual
    VectorXd z_diff = Zsig.col(i) - z_pred;
    S = S + weights(i) * z_diff * z_diff.transpose();
	
	// state difference
    VectorXd x_diff = Xsig_pred.col(i) - x;
	Tc = Tc + weights_(i) * x_diff * z_diff.transpose();
  }

  //add measurement noise covariance matrix
  MatrixXd R = MatrixXd(n_z,n_z);
  R <<    std_laspx_*std_laspx_, 0,
          0, std_laspy_*std_laspy_;
		  
  S = S + R;
  
  //We have mean and co-variance for predicted r0, phi and rho-dot. 
  //Now from actual measurement update mean and co-variance i.e x_ and P_
  //Kalman gain
  MatrixXd K = Tc * S.inverse();
  
  //LiDAR actual measurement
  VectorXd z = VectorXd(n_z);
  z << meas_package.raw_measurements_(0),
       meas_package.raw_measurements_(1);
	   
  //Difference from z_pred
  VectorXd z_diff = z - z_pred;

  //update state mean and covariance matrix
  x_ = x_ + K * z_diff;
  P_ = P_ - K*S*K.transpose();
}

/**
 * Updates the state and the state covariance matrix using a radar measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateRadar(MeasurementPackage meas_package) {
  /**
  TODO:

  Complete this function! Use radar data to update the belief about the object's
  position. Modify the state vector, x_, and covariance, P_.

  You'll also need to calculate the radar NIS.
  */
  
  /*Measurement space depends on predicted sigma points and weights. No need to recalculate */
  int n_z = 3;
  
  MatrixXd Zsig = MatrixXd(n_z, 2 * n_aug_ + 1);
  //transform sigma points into measurement space
  for (int i = 0; i < 2 * n_aug_ + 1; i++) 
  {  
      //2n+1 sigma points

      // extract values for better readability
      double p_x = Xsig_pred(0,i);
      double p_y = Xsig_pred(1,i);
      double v  = Xsig_pred(2,i);
      double yaw = Xsig_pred(3,i);

      double v1 = cos(yaw)*v;
      double v2 = sin(yaw)*v;

      // measurement model
      Zsig(0,i) = sqrt(p_x*p_x + p_y*p_y);                        //r
      Zsig(1,i) = atan2(p_y,p_x);                                 //phi
      Zsig(2,i) = (p_x*v1 + p_y*v2 ) / sqrt(p_x*p_x + p_y*p_y);   //r_dot
  }
  
  //mean predicted measurement
  VectorXd z_pred = VectorXd(n_z);
  z_pred.fill(0.0);
  for (int i=0; i < 2*n_aug_+1; i++) {
      z_pred = z_pred + weights_(i) * Zsig.col(i);
  }

  //innovation covariance matrix S
  MatrixXd S = MatrixXd(n_z,n_z);
  S.fill(0.0);
  //cross corelation matrix
  MatrixXd Tc = MatrixXd(n_x, n_z);
  Tc.fill(0.0);
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 sigma points
    //residual
    VectorXd z_diff = Zsig.col(i) - z_pred;

    //angle normalization
    while (z_diff(1)> M_PI) z_diff(1)-=2.*M_PI;
    while (z_diff(1)<-M_PI) z_diff(1)+=2.*M_PI;

    S = S + weights(i) * z_diff * z_diff.transpose();
	
	// state difference
    VectorXd x_diff = Xsig_pred.col(i) - x;
    //angle normalization
    while (x_diff(3)> M_PI) x_diff(3)-=2.*M_PI;
    while (x_diff(3)<-M_PI) x_diff(3)+=2.*M_PI;
	
	Tc = Tc + weights_(i) * x_diff * z_diff.transpose();
  }

  //add measurement noise covariance matrix
  MatrixXd R = MatrixXd(n_z,n_z);
  R <<    std_radr*std_radr, 0, 0,
          0, std_radphi*std_radphi, 0,
          0, 0,std_radrd*std_radrd;
  S = S + R;
  
  //We have mean and co-variance for predicted r0, phi and rho-dot. 
  //Now from actual measurement update mean and co-variance i.e x_ and P_
  //Kalman gain
  MatrixXd K = Tc * S.inverse();
  
  //RADAR actual measurement
  VectorXd z = VectorXd(n_z);
  z << meas_package.raw_measurements_(0),
       meas_package.raw_measurements_(1),
	   meas_package.raw_measurements_(2);
	   
  //Difference from z_pred
  VectorXd z_diff = z - z_pred;
  
  //angle normalization
  while (z_diff(1)> M_PI) z_diff(1)-=2.*M_PI;
  while (z_diff(1)<-M_PI) z_diff(1)+=2.*M_PI;

  //update state mean and covariance matrix
  x_ = x_ + K * z_diff;
  P_ = P_ - K*S*K.transpose();
}
