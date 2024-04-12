#include "ulm3_samples.h"

#include <cmath>
#include <thread>

#include "kalman_filter.h"
#include "sync_queue.h"
#include "ulm3_acquisition_callback.h"
#include "ulm3_pdoa_comm.h"

char[] default_name="/dev/ttyUSB1";

KalmanFilter initialize_filter()
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
}

ULM3Samples::ULM3Samples(char* pname)
    : filter_(initialize_filter()),
      sync_queue_(5),
      ulm3_pdoa_comm_(pname),
      ulm3_acquisition_callback_(&sync_queue_)
{
    isFirst_ = true;
}

ULM3Samples::ULM3Samples()
    : filter_(initialize_filter()),
      sync_queue_(5),
      ulm3_pdoa_comm_(default_name),
      ulm3_acquisition_callback_(&sync_queue_)
{
    isFirst_ = true;
}

void ULM3Samples::start()
{
    ulm3_pdoa_comm_.registerCallback(&ulm3_acquisition_callback_);
    ulm3_pdoa_comm_.start();

    if (isFirst_) {
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
    }
}

output_data ULM3Samples::getData()
{
    control_param input = sync_queue_.waitAndPop();
    output_data result;
    Eigen::VectorXd z(2);
    z << input.distance * std::cos(input.degree * M_PI / 180),
        input.distance * std::sin(input.degree * M_PI / 180);
    filter_.predict();
    filter_.corrective(z);

    Eigen::VectorXd current_x = filter_.getX();
    result.x = current_x[0];
    result.y = current_x[1];
    return result;
}

int main(int argc, char* argv[])
{
    char pname[] = "/dev/ttyUSB1";
    ULM3Samples ulm3_samples(pname);
    output_data temp;
    ulm3_samples.start();
    for (int i = 0; i < 400; ++i) {
        temp = ulm3_samples.getData();
        std::cout << temp.x << ' ' << temp.y << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
