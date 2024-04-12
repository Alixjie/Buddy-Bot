#include <cmath>
#include <iostream>

#include "kalman_filter.h"

int main(int argc, char *argv[])
{
    double sigma_ax = 4;
    double sigma_ay = 4;
    double delta_t = 0.1;
    Eigen::VectorXd x(4);
    Eigen::VectorXd z(2);
    Eigen::MatrixXd A = Eigen::MatrixXd(4, 4);
    Eigen::MatrixXd P = Eigen::MatrixXd(4, 4);
    Eigen::MatrixXd H = Eigen::MatrixXd(2, 4);
    Eigen::MatrixXd R = Eigen::MatrixXd(2, 2);
    Eigen::MatrixXd Q = Eigen::MatrixXd(4, 4);
    A << 1, 0, delta_t, 0, 0, 1, 0, delta_t, 0, 0, 1, 0, 0, 0, 0, 1;
    P << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1000, 0, 0, 0, 0, 1000;
    H << 1, 0, 0, 0, 0, 1, 0, 0;
    R << 0.066, 0, 0, 0.066;
    Q << pow(delta_t, 4) * sigma_ax / 4, 0, pow(delta_t, 3) * sigma_ax / 2, 0,
        0, pow(delta_t, 4) * sigma_ay / 4, 0, pow(delta_t, 3) * sigma_ax / 2,
        pow(delta_t, 3) * sigma_ax / 2, 0, pow(delta_t, 2) * sigma_ax, 0, 0,
        pow(delta_t, 3) * sigma_ay / 2, 0, pow(delta_t, 2) * sigma_ay;

    x << 71.99, 0.10, 2, 1;

    KalmanFilter filter(x, P, A, H, R, Q);
    double distance, degree;

    for (int i = 0; i < 30; ++i) {
        std::cin >> distance >> degree;
        z << distance * std::cos(degree * M_PI / 180),
            distance * std::sin(degree * M_PI / 180);
        filter.predict();
        filter.corrective(z);
        std::cout << filter.getX() << '\n' << std::endl;
    }
}