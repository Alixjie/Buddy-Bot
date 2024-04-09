#ifndef __UM3_PDOACOMM_H__
#define __UM3_PDOACOMM_H__

#include <fcntl.h>
#include <termio.h>
#include <unistd.h>

#include <cstddef>
#include <thread>

typedef struct uwb_sample {
    int tag_id;
    int x_cm;
    int y_cm;
    int distance_cm;
    int range_num;
    float pdoa_deg;
    float aoa_deg;
    int distance_offset_cm;
    int pdoa_offset_deg;
} uwb_sample;

/**
 * Callback of the ULM3_PDOA
 **/
class ULM3PDOACallback {
public:
    virtual void hasSample(char *uwb_string, size_t length) = 0;
};

class ULM3PDOAComm {
public:
    /**
     * Constructor. Opens the SPI device and waits for to start
     * the acquisition.
     * \param port_name the path to device.
     **/
    ULM3PDOAComm(char *port_name);

    /**
     * Destructor which makes sure the data acquisition.
     * has stopped.
     **/
    ~ULM3PDOAComm();

    /**
     * Registers the callback which is called whenever there is a sample.
     * \param cb pointer the callback interface.
     **/
    void registerCallback(ULM3PDOACallback *cb);

    /**
     *Unregisters the callback to the callback interface
     **/
    void unregisterCallback();

    /**
     * Start the data acquisition.
     **/
    void start();

    /**
     * Stop the data acquisition.
     **/
    void stop();

private:
    int running_ = 0;
    int fd_;
    std::thread daq_thread_;
    ULM3PDOACallback *ulm3_pdoa_callback_ = nullptr;

    struct termios serial_port_setting_;
    uwb_sample pretreatment(char *sample_string);
    void openDev(char *dev_name);
    void run();
};

#endif