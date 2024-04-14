#include <chrono>
#include <thread>

#include "ulm3_samples.h"

void echoDistance(int distance)
{
    std::cout << "Distance: " << distance << std::endl;
}

void echoDegree(int degree) { std::cout << "Degree: " << degree << std::endl; }

int main(int argc, char* argv[])
{
    char pname[] = "/dev/ttyUSB0";
    ULM3Samples ulm3_samples(pname);
    ulm3_samples.registerControl(echoDistance, echoDegree);
    ulm3_samples.start();
    ulm3_samples.startFollow();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    ulm3_samples.startFollow();
}