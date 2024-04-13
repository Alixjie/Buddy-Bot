#include <iostream>

#include "../sync_queue.h"
#include "../ulm3_acquisition_callback.h"
#include "../ulm3_pdoa_comm.h"

int main(int argc, char* argv[])
{
    char pname[] = "/dev/ttyUSB0";

    SyncQueue<control_param> sync_queue(5);

    ULM3PDOAComm ulm3_pdoa_comm(pname);
    ULM3AcquisitionCallback ulm3_acquisition_callback(&sync_queue);
    ulm3_pdoa_comm.registerCallback(&ulm3_acquisition_callback);
    ulm3_pdoa_comm.start();

    control_param* temp = new control_param[2];
    for (int i = 0; i < 2000; i++) {
        sync_queue.waitAndPop(2, temp);
        std::cout << "Distance: " << temp[0].distance
                  << " Speed: " << temp[0].speed
                  << " Degree: " << temp[0].degree << std::endl;
        std::cout << "Distance: " << temp[1].distance
                  << " Speed: " << temp[1].speed
                  << " Degree: " << temp[1].degree << std::endl;
        /* if (i == 99) {
            sync_queue.clear();
            break;
        } */
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    delete[] temp;

    return 0;
}