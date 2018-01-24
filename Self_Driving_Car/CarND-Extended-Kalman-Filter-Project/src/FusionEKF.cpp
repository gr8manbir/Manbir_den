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

  //Keeping standard deviation low. Assuming sensors work well.
  noise_ax = 1; 
  noise_ay = 1;
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
    ekf_.x_ << 1, 1, 1, 1;

    // Set prediction uncertainty very high as compared to R initially( S=HPHT +R )
    MatrixXd P_ = MatrixXd(4, 4);
    P_ << 1, 0, 0, 0,
          0, 1, 0, 0,
          0, 0, 1000, 0,
          0, 0, 0, 1000;

    // Consider 1 as delta t initially(never used) and 1 as noise to calculate covariance for prediction(p! = FPFT +Q )
    MatrixXd Q_ = MatrixXd(4,4);
    Q_ << 1, 0, 1, 0,
          0, 1, 0, 1,
          0, 0, 1, 0,
          0, 0, 0, 1;
   
    // F function (p1 = p0 + ut) or x' = Fx. Never used but still init (for code clarity)
    MatrixXd F_ = MatrixXd(4,4);
    F_ << 1, 0, 1, 0,
          0, 1, 0, 1,
          0, 0, 1, 0,
          0, 0, 0, 1;

    H_laser_ << 1, 0, 0, 0,
                0, 1, 0, 0;


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
      //TODO: vx, vy should ideally be fetched from Radar data. How?
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
    //Initialize all constants - will be re-calculated anyway 
    ekf_.init(ekf_.x_, P_, F_, H_laser_, R_laser_, Q_); 
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

  ekf_.Predict();

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
  } else {
    // Laser updates
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
