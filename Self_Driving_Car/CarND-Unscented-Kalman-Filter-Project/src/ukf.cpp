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
  std_a_ = 1.0;

  // Process noise standard deviation yaw acceleration in rad/s^2
  std_yawdd_ = 0.5;
  
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
  Xsig_pred_ = MatrixXd(n_x_, 2*n_aug_+1);
  
  // set weights
  weights_.fill(0.0);
  double weight_0 = lambda_/(lambda_+n_aug_);
  weights_(0) = weight_0;
  for (int i=1; i<2*n_aug_+1; i++) 
  {  
    //2n+1 weights
    double weight = 0.5/(n_aug_+lambda_);
    weights_(i) = weight;
  }
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
	  x_.fill(0.0);
	  Xsig_pred_.fill(0.0);
	  P_ << 1.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 1.0;		  
      if (meas_package.sensor_type_ == MeasurementPackage::RADAR) 
      {
          double rho = meas_package.raw_measurements_[0];
          double phi = meas_package.raw_measurements_[1];
          double rhoDot = meas_package.raw_measurements_[2];
	      double v = 0.0;
          /**
           Convert radar from polar to cartesian coordinates and initialize state.
           */
          double px = rho *cos(phi);
          double py = rho *sin(phi);
		  double vx = rhoDot*cos(phi);
		  double vy = rhoDot*sin(phi);
		  v = sqrt(vx*vx+vy*vy);
	      x_<<px,py,v,0.0,0.0;
      } 
      else if (meas_package.sensor_type_ == MeasurementPackage::LASER) 
      {
          // LiDAR has no velocity component
          x_ << meas_package.raw_measurements_(0), meas_package.raw_measurements_(1), 0.0, 0.0, 0.0;
		  if (fabs(x_(0)) < 0.001 && fabs(x_(1)) < 0.001) 
		  {
              x_(0) = 0.001;
              x_(1) = 0.001;
          }
      }
	  
	  is_initialized_ = true;
	  time_us_ = meas_package.timestamp_;
	  return;
  }
  
  /* Prediction step */
  double dt = (meas_package.timestamp_ - time_us_)/ 1000000.0;;
  Prediction(dt);
  
  static int ctr =0;
  ctr++;
  if(ctr == 150) exit(0);
  /* Measurement update step */
  if(meas_package.sensor_type_ == MeasurementPackage::RADAR && use_radar_ == true )
  {
	  /* Radar update */
	  UpdateRadar(meas_package);
  }
  else if (meas_package.sensor_type_ == MeasurementPackage::LASER && use_laser_ == true)
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

   /* 1. Calculate augmentation vector and co-variance for vector 2 i.e. noise */
   MatrixXd Xsig_aug = MatrixXd(n_aug_, 2*n_aug_+1);
   Xsig_aug.fill(0.0);
   MatrixXd P_aug = MatrixXd(n_aug_, n_aug_);
   P_aug.fill(0.0);
   
   /* Similar to x_ create x_aug_ with last two values zero */
   VectorXd x_aug_ = VectorXd(n_aug_);
   x_aug_.head(5) = x_;
   x_aug_(5) = 0;
   x_aug_(6) = 0;
   
   //std::cout<<"delta_t: "<<delta_t<<endl;
   //std::cout<<"Initial x_aug_=" <<x_aug_<<endl;
   /* Fill p_aug with values */
   P_aug.topLeftCorner(5,5) = P_;
   P_aug(5,5) = std_a_*std_a_;
   P_aug(6,6) = std_yawdd_*std_yawdd_;
   //std::cout<<"Initial P_aug_=" <<P_aug<<endl;
	  
   /* Square root matrix */
   MatrixXd L = P_aug.llt().matrixL();
   
   /* Create augmented sigma points */
   Xsig_aug.col(0) = x_aug_;
   //std::cout<<"Sigma point aug: 0"<< " "<<Xsig_aug.col(0)<<endl;
   for( int i = 0; i < n_aug_; i++ )
   {
       Xsig_aug.col(i+1)       = x_aug_ + sqrt(lambda_+n_aug_) * L.col(i);
       Xsig_aug.col(i+1+n_aug_) = x_aug_ - sqrt(lambda_+n_aug_) * L.col(i);
       //std::cout<<"Sigma point aug: "<<i+1<<" "<<Xsig_aug.col(i+1)<<endl;
	   //std::cout<<"Sigma point aug: "<<i+1+n_aug_<<" "<<Xsig_aug.col(i+1+n_aug_)<<endl;
   }
     
   /* 2. Predict sigma points */
   Xsig_pred_.fill(0.0);
   for (int i = 0; i< 2*n_aug_+1; i++)
   {
       //extract values for better readability
       double p_x = Xsig_aug(0,i); //0.312243
       double p_y = Xsig_aug(1,i); //0.58034
       double v = Xsig_aug(2,i);   //1.73205
       double yaw = Xsig_aug(3,i); //0 
       double yawd = Xsig_aug(4,i); //0
       double nu_a = Xsig_aug(5,i); //0
       double nu_yawdd = Xsig_aug(6,i); //0

       //predicted state values
       double px_p=0.0, py_p=0.0;

       //avoid division by zero
       if (fabs(yawd) > 0.001) {
          px_p = p_x + (v/yawd) * ( sin (yaw + yawd*delta_t) - sin(yaw));
          py_p = p_y + (v/yawd) * ( cos(yaw) - cos(yaw+yawd*delta_t) );
       }
       else {
          px_p = p_x + v*delta_t*cos(yaw); // 0.312243 + 1.73205*delta_t*1
          py_p = p_y + v*delta_t*sin(yaw); // 0.58034 + 1.73205*delta_t*0 = 0.58034
       }

       double v_p = v; //1.73205
       double yaw_p = yaw + yawd*delta_t; // 0+0 = 0
       double yawd_p = yawd; // 0

       //add noise
       px_p = px_p + 0.5*nu_a*delta_t*delta_t * cos(yaw); // +0
       py_p = py_p + 0.5*nu_a*delta_t*delta_t * sin(yaw); // 0.58034+0 = 0.58034
       v_p = v_p + nu_a*delta_t; //1.73205

       yaw_p = yaw_p + 0.5*nu_yawdd*delta_t*delta_t; // 0+ 0 = 0
       yawd_p = yawd_p + nu_yawdd*delta_t; // 0+0 = 0

       //write predicted sigma point into right column
       Xsig_pred_(0,i) = px_p;  //
       Xsig_pred_(1,i) = py_p;  //0.58034
       Xsig_pred_(2,i) = v_p;   //1.73205
       Xsig_pred_(3,i) = yaw_p; //0
       Xsig_pred_(4,i) = yawd_p; //0
	   //std::cout<<"Sigma point prediction: "<<i<< " "<<Xsig_pred_.col(i)<<endl; 
   }
    /* 3. From sigma point predictions at time t+dt, calculate new mean and co-variance */

	
	// Calculate mean
	x_.fill(0.0);
	for (int i = 0; i < 2 * n_aug_ + 1; i++) 
	{  
        //iterate over sigma points
        x_ = x_ + weights_(i) * Xsig_pred_.col(i);
    }
	//std::cout<<"Predicted x_"<<x_<<endl;
	//Calculate co-variance
	P_.fill(0.0);
    for ( int i = 0; i < 2 * n_aug_ + 1; i++) {  //iterate over sigma points

        // state difference
        VectorXd x_diff = Xsig_pred_.col(i) - x_;
        //angle normalization
        while (x_diff(3)> M_PI) x_diff(3)-=2.*M_PI;
        while (x_diff(3)<-M_PI) x_diff(3)+=2.*M_PI;

        P_ = P_ + weights_(i) * x_diff * x_diff.transpose() ;
    }
	//std::cout<<"Predicted P_"<<P_<<endl;
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
  Zsig.fill(0.0);
  //transform sigma points into measurement space
  for (int i = 0; i < 2 * n_aug_ + 1; i++) 
  {  
      //2n+1 sigma points

      // measurement model
      Zsig(0,i) = Xsig_pred_(0,i);                        //px
      Zsig(1,i) = Xsig_pred_(1,i);                        //py
  }
  
  //mean predicted measurement
  VectorXd z_pred = VectorXd(n_z);
  z_pred.fill(0.0);
  for (int i=0; i < 2*n_aug_+1; i++) {
      z_pred = z_pred + weights_(i) * Zsig.col(i);
  }

  std::cout<<"Z_pred(LiDAR)= "<<z_pred<<endl;
  //innovation covariance matrix S
  MatrixXd S = MatrixXd(n_z,n_z);
  S.fill(0.0);


  
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 sigma points
    //residual
    VectorXd z_diff = Zsig.col(i) - z_pred;
    S = S + weights_(i) * z_diff * z_diff.transpose();
	
  }

  //add measurement noise covariance matrix
  MatrixXd R = MatrixXd(n_z,n_z);
  R.fill(0.0);
  R <<    std_laspx_*std_laspx_, 0,
          0, std_laspy_*std_laspy_;
		  
  S = S + R;

  //cross corelation matrix
  MatrixXd Tc = MatrixXd(n_x_, n_z);
  Tc.fill(0.0);  
  
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 sigma points
    //residual
    VectorXd z_diff = Zsig.col(i) - z_pred;
	
	// state difference
    VectorXd x_diff = Xsig_pred_.col(i) - x_;
	Tc = Tc + weights_(i) * x_diff * z_diff.transpose();
  }
  //We have mean and co-variance for predicted r0, phi and rho-dot. 
  //Now from actual measurement update mean and co-variance i.e x_ and P_
  //Kalman gain
  MatrixXd K = Tc * S.inverse();
  
  //LiDAR actual measurement
  VectorXd z = VectorXd(n_z);
  z.fill(0.0);
  z << meas_package.raw_measurements_[0],
       meas_package.raw_measurements_[1];
	   
  std::cout<<"Z(LiDAR)= "<<z<<endl;

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
  Zsig.fill(0.0);
  //transform sigma points into measurement space
  for (int i = 0; i < 2 * n_aug_ + 1; i++) 
  {  
      //2n+1 sigma points

      // extract values for better readability
      double p_x = Xsig_pred_(0,i);
      double p_y = Xsig_pred_(1,i);
      double v  = Xsig_pred_(2,i);
      double yaw = Xsig_pred_(3,i);

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

  std::cout<<"Z_pred(RADAR)= "<<z_pred<<endl;
  
  //innovation covariance matrix S
  MatrixXd S = MatrixXd(n_z,n_z);
  S.fill(0.0);

  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 sigma points
    //residual
    VectorXd z_diff = Zsig.col(i) - z_pred;

    //angle normalization
    while (z_diff(1)> M_PI) z_diff(1)-=2.*M_PI;
    while (z_diff(1)<-M_PI) z_diff(1)+=2.*M_PI;

    S = S + weights_(i) * z_diff * z_diff.transpose();
	
  }

  //add measurement noise covariance matrix
  MatrixXd R = MatrixXd(n_z,n_z);
  R.fill(0.0);
  R <<    std_radr_*std_radr_, 0, 0,
          0, std_radphi_*std_radphi_, 0,
          0, 0,std_radrd_*std_radrd_;
  S = S + R;
  
  //cross corelation matrix
  MatrixXd Tc = MatrixXd(n_x_, n_z);
  Tc.fill(0.0);
  for (int i = 0; i < 2 * n_aug_ + 1; i++) {  //2n+1 sigma points
    //residual
    VectorXd z_diff = Zsig.col(i) - z_pred;

    //angle normalization
    while (z_diff(1)> M_PI) z_diff(1)-=2.*M_PI;
    while (z_diff(1)<-M_PI) z_diff(1)+=2.*M_PI;
	
	// state difference
    VectorXd x_diff = Xsig_pred_.col(i) - x_;
	
    //angle normalization
    while (x_diff(3)> M_PI) x_diff(3)-=2.*M_PI;
    while (x_diff(3)<-M_PI) x_diff(3)+=2.*M_PI;
	
	Tc = Tc + weights_(i) * x_diff * z_diff.transpose();
  }
  
  //We have mean and co-variance for predicted r0, phi and rho-dot. 
  //Now from actual measurement update mean and co-variance i.e x_ and P_
  //Kalman gain
  MatrixXd K = Tc * S.inverse();
  
  //RADAR actual measurement
  VectorXd z = VectorXd(n_z);
  z.fill(0.0);
  z << meas_package.raw_measurements_[0],
       meas_package.raw_measurements_[1],
	   meas_package.raw_measurements_[2];

  std::cout<<"z(RADAR)= "<<z <<endl;
  //Difference from z_pred
  VectorXd z_diff = z - z_pred;
  
  //angle normalization
  while (z_diff(1)> M_PI) z_diff(1)-=2.*M_PI;
  while (z_diff(1)<-M_PI) z_diff(1)+=2.*M_PI;

  //update state mean and covariance matrix
  x_ = x_ + K * z_diff;
  P_ = P_ - K*S*K.transpose();
}
