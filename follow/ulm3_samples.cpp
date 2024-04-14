#include "ulm3_samples.h"

#include <cmath>
#include <thread>

// #include "kalman_filter.h"
#include "sync_queue.h"
#include "ulm3_acquisition_callback.h"
#include "ulm3_pdoa_comm.h"

/*KalmanFilter initialize_filter()
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

    x << 0, 0, 0, 0;

    KalmanFilter filter(x, P, A, H, R, Q);

    return filter;
}*/

ULM3Samples::ULM3Samples(const char* pname)
    : /*filter_(initialize_filter()),*/
      sync_queue_(5),
      ulm3_pdoa_comm_(pname),
      ulm3_acquisition_callback_(&sync_queue_)
{
    isFirst_ = true;
    following_ = 0;
}

// ULM3Samples::ULM3Samples()
//     : /*filter_(initialize_filter()),*/
//       sync_queue_(5),
//       ulm3_pdoa_comm_(default_name),
//       ulm3_acquisition_callback_(&sync_queue_)
//{
//     isFirst_ = true;
// }

void ULM3Samples::start()
{
    ulm3_pdoa_comm_.registerCallback(&ulm3_acquisition_callback_);
    ulm3_pdoa_comm_.start();

    /*if (isFirst_) {
        control_param first_data[2];
        sync_queue_.waitAndPop(2, first_data);

        double first_distance =
            (first_data[0].distance + first_data[1].distance) / 2;
        double first_degree = (first_data[0].degree + first_data[1].degree) / 2;

        double first_speed_x =
            (first_data[1].distance * cos(first_data[1].degree * M_PI / 180) -
             first_data[0].distance * cos(first_data[0].degree * M_PI / 180)) /
            0.1;
        double first_speed_y =
            (first_data[1].distance * sin(first_data[1].degree * M_PI / 180) -
             first_data[0].distance * sin(first_data[0].degree * M_PI / 180)) /
            0.1;

        Eigen::VectorXd first_x(4);
        first_x << first_distance * cos(first_degree * M_PI / 180),
            first_distance * sin(first_degree * M_PI / 180), first_speed_x,
            first_speed_y;

        filter_.setX(first_x);

        isFirst_ = false;
    }*/
}

output_data ULM3Samples::getData()
{
    control_param input = sync_queue_.waitAndPop();
    output_data result;
    /*Eigen::VectorXd z(2);
    z << input.distance * std::cos(input.degree * M_PI / 180),
        input.distance * std::sin(input.degree * M_PI / 180);
    filter_.predict();
    filter_.corrective(z);*/

    // Eigen::VectorXd current_x = filter_.getX();
    /*result.x = current_x[0];
    result.y = current_x[1];*/
    result.y = input.distance * cos(input.degree * M_PI / 180);
    result.x = input.distance * sin(input.degree * M_PI / 180);
    return result;
}

void ULM3Samples::run_follow()
{
    double weights[] = {0.05, 0.05, 0.1, 0.1, 0.3, 0.4};
    while (following_) {
        control_param temp_sample[5];
        sync_queue_.waitAndPop(5, temp_sample);
        control_param result;

        result.degree = 0;
        result.distance = 0;

        for (int i = 0; i < 5; ++i) {
            result.degree += weights[i] * temp_sample[i].degree;
            result.distance += weights[i] * temp_sample[i].distance;
        }

        degreeControl_(std::round(result.degree / 45) * 45);
        distanceControl_(std::round(result.distance / 30) * 30);
    }
}

void ULM3Samples::registerControl(void (*distanceControl)(int),
                                  void (*degreeControl)(int))
{
    distanceControl_ = distanceControl;
    degreeControl_ = degreeControl;
}

void ULM3Samples::startFollow()
{
    if (following_) return;

    following_ = 1;

    followThread = std::thread(&ULM3Samples::run_follow, this);
}

void ULM3Samples::stopFollow()
{
    if (!following_) return;

    following_ = 0;
    followThread.join();
}

ULM3Samples::~ULM3Samples()
{
    stopFollow();
    distanceControl_ = nullptr;
    degreeControl_ = nullptr;
}