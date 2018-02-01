#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
        0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
        0, 0.0009, 0,
        0, 0, 0.09;

  /**
  TODO:
    * Finish initializing the FusionEKF.
    * Set the process and measurement noises
  */

}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {


  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    /**
    TODO:
      * Initialize the state ekf_.x_ with the first measurement.
      * Create the covariance matrix.
      * Remember: you'll need to convert radar from polar to cartesian coordinates.
    */
    // first measurement
    cout << "EKF: " << endl;
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1.0, 1.0, 1.0, 1.0;

    // Set prediction uncertainty very high as compared to R initially( S=HPHT +R )
    MatrixXd P_ = MatrixXd(4, 4);
    P_ << 1.0, 0.0, 0.0, 0.0,
          0.0, 1.0, 0.0, 0.0,
          0.0, 0.0, 1000.0, 0.0,
          0.0, 0.0, 0.0, 1000.0;

    // Consider 1 as delta t initially(never used) and 1 as noise to calculate covariance for prediction(p! = FPFT +Q )
    MatrixXd Q_ = MatrixXd(4,4);
    Q_ << 1.0, 0.0, 1.0, 0.0,
          0.0, 1.0, 0.0, 1.0,
          0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 1.0;
   
    // F function (p1 = p0 + ut) or x' = Fx. Never used but still init (for code clarity)
    MatrixXd F_ = MatrixXd(4,4);
    F_ << 1.0, 0.0, 1.0, 0.0,
          0.0, 1.0, 0.0, 1.0,
          0.0, 0.0, 1.0, 0.0,
          0.0, 0.0, 0.0, 1.0;

    H_laser_ << 1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0;


    //H for Radar i.e. Hj is jacobian. So not initialized


    float px =0.0, py=0.0;
    float vx =0.0, vy=0.0;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) 
    {
      float rho = measurement_pack.raw_measurements_[0];
      float phi = measurement_pack.raw_measurements_[1];
      float rhoDot = measurement_pack.raw_measurements_[2];
      /**
      Convert radar from polar to cartesian coordinates and initialize state.
      */
      /* x is vertical axis, y is horizontal. Cos(angle) = adj/hyp. Therefore: */
      px = rho *cos(phi);
      py = rho *sin(phi);
      //TODO: vx, vy should ideally be fetched from Radar data. below is rough estimation(try 1 later). 
	  //Makes a difference in Jacobian calculation?
	  vx = rhoDot*cos(phi);
	  vy = rhoDot*sin(phi);
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
      Initialize state.
      */
      //Easier for LASER
      px = measurement_pack.raw_measurements_[0];
      py = measurement_pack.raw_measurements_[1];
    }

    ekf_.x_ << px, py,vx,vy;
	//We don't want px and py to be zero as it will cause issues with atan2() in UpdateEKF
	if( ekf_.x_[0] < 0.001 ) ekf_.x_[0] = 0.001;
	if( ekf_.x_[1] < 0.001 ) ekf_.x_[1] = 0.001;
	
    //Initialize all constants - will be re-calculated anyway 
    //ekf_.Init(ekf_.x_, P_, F_, H_laser_, R_laser_, Q_); 
    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  /**
   TODO:
     * Update the state transition matrix F according to the new elapsed time.
      - Time is measured in seconds.
     * Update the process noise covariance matrix.
     * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  //Keeping standard deviation low. Assuming sensors work well.
  float noise_ax = 9.0; 
  float noise_ay = 9.0;

  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;	//dt - expressed in seconds
  previous_timestamp_ = measurement_pack.timestamp_;
  
  /* If dt is very small, we should not predict again as values won't change but it will mess
     up calculations */
  if( dt > 0.001)
  {
	
	float dt_2 = dt * dt;
	float dt_3 = dt_2 * dt;
	float dt_4 = dt_3 * dt;
	
	ekf_.F_ = MatrixXd(4, 4);
	ekf_.F_ << 1.0, 0.0, dt,  0.0,
	           0.0, 1.0, 0.0, dt,
			   0.0, 0.0, 1.0, 0.0,
			   0.0, 0.0, 0.0, 1.0;

    ekf_.F_ = MatrixXd(4, 4);
	ekf_.Q_ <<  dt_4/4.0*noise_ax, 0.0, dt_3/2.0*noise_ax, 0.0,
			0.0, dt_4/4.0*noise_ay, 0.0, dt_3/2.0*noise_ay,
			dt_3/2.0*noise_ax, 0.0, dt_2*noise_ax, 0.0,
			0.0, dt_3/2.0*noise_ay, 0.0, dt_2*noise_ay;

	ekf_.Predict();
  }

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  /**
   TODO:
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // Radar updates
    // Cannot use linear equations here :(  
    ekf_.R_ = R_radar_;
    ekf_.H_ = tools.CalculateJacobian(ekf_.x_);
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
  } else {
    // Laser updates
    // Thankfully can use linear equations here :)
    ekf_.H_ = H_laser_;
    ekf_.R_ = R_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
