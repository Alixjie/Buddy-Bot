#ifndef __KALMAN_FILTER_H__
#define __KALMAN_FILTER_H__

#include <eigen3/Eigen/Dense>

class KalmanFilter {
public:
    KalmanFilter(Eigen::VectorXd &x, Eigen::MatrixXd &P, Eigen::MatrixXd &A,
                 Eigen::MatrixXd &H, Eigen::MatrixXd &R, Eigen::MatrixXd &Q);

    void predict();
    void corrective(const Eigen::VectorXd &z);
    Eigen::VectorXd &getX();
    void setX(Eigen::VectorXd &);

private:
    // State vector.
    Eigen::VectorXd x_;
    int x_size;
    // State covariance matrix.
    Eigen::MatrixXd P_;
    // State transition matrix.
    Eigen::MatrixXd A_;
    // Process covariance matrix.
    Eigen::MatrixXd Q_;
    // Measurement matrix.
    Eigen::MatrixXd H_;
    // Measurement covariance matrix.
    Eigen::MatrixXd R_;
    // Kalman gain.
    Eigen::MatrixXd K_;
};

#endif
