//#include "kalman_filter.h"

//#include <eigen3/Eigen/src/Core/Matrix.h>

//KalmanFilter::KalmanFilter(Eigen::VectorXd &x, Eigen::MatrixXd &P,
//                           Eigen::MatrixXd &A, Eigen::MatrixXd &H,
//                           Eigen::MatrixXd &R, Eigen::MatrixXd &Q)
//{
//    x_ = x;
//    x_size = x_.size();
//    P_ = P;
//    A_ = A;
//    H_ = H;
//    R_ = R;
//    Q_ = Q;
//}

//void KalmanFilter::predict()
//{
//    x_ = A_ * x_;
//    P_ = A_ * P_ * A_.transpose() + Q_;
//}

//void KalmanFilter::corrective(const Eigen::VectorXd &z)
//{
//    Eigen::MatrixXd Ht = H_.transpose();
//    Eigen::MatrixXd S = H_ * P_ * Ht + R_;
//    K_ = P_ * Ht * S.inverse();

//    x_ = x_ + K_ * (z - H_ * x_);

//    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(x_size, x_size);

//    P_ = (I - K_ * H_) * P_;
//}

//Eigen::VectorXd &KalmanFilter::getX() { return x_; }

//void KalmanFilter::setX(Eigen::VectorXd &x) { x_ = x; }
