#include "ulm3_pdoa_comm.h"

#include <iostream>

void ULM3PDOAComm::openDev(const char* dev_name)
{
    fd_ = open(dev_name, O_RDONLY | O_NOCTTY);
    if (-1 == fd_) {
        std::cerr << "Error opening serial port.";
        return;
    }
}

ULM3PDOAComm::ULM3PDOAComm(const char* port_name)
{
    buffer.str = new char[1024];
    buffer.length = 0;
    buffer.capacity = 1024;

    openDev(port_name);
    if (tcgetattr(fd_, &serial_port_setting_)) {
        close(fd_);
        std::cerr << "Error getting serial port attributes.";
        return;
    }

    cfsetospeed(&serial_port_setting_, B115200);
    cfsetispeed(&serial_port_setting_, B115200);

    serial_port_setting_.c_cflag |= (CLOCAL | CREAD);
    serial_port_setting_.c_cflag |= PARENB;
    serial_port_setting_.c_cflag &= ~CSTOPB;
    serial_port_setting_.c_cflag &= ~CSIZE;
    serial_port_setting_.c_cflag |= CS8;

    // serial_port_setting_.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    serial_port_setting_.c_lflag |= ICANON;
    serial_port_setting_.c_iflag &= ~(INPCK | ISTRIP | INLCR | ICRNL | IGNCR);
    serial_port_setting_.c_oflag &= ~OPOST;

    serial_port_setting_.c_cc[VMIN] = 1;
    serial_port_setting_.c_cc[VTIME] = 0;

    if (tcsetattr(fd_, TCSAFLUSH, &serial_port_setting_)) {
        close(fd_);
        std::cerr << "Error setting serial port attributes.";
        return;
    }
}

ULM3PDOAComm::~ULM3PDOAComm()
{
    stop();
    close(fd_);
    if (buffer.str != nullptr) {
        delete[] buffer.str;
    }
}

void ULM3PDOAComm::registerCallback(ULM3PDOACallback* cb)
{
    ulm3_pdoa_callback_ = cb;
}

void ULM3PDOAComm::unregisterCallback() { ulm3_pdoa_callback_ = nullptr; }

void ULM3PDOAComm::run()
{
    ssize_t bytesRead;
    running_ = 1;
    while (running_) {
        bytesRead = read(fd_, buffer.str, buffer.capacity);
        buffer.length = bytesRead;
        /*for (int i = 0; i < bytesRead; i++) {
            std::cout << buffer.str[i];
        }*/
        ulm3_pdoa_callback_->hasSample(buffer);
    }
    close(fd_);
}

void ULM3PDOAComm::start()
{
    if (running_) return;
    daq_thread_ = std::thread(&ULM3PDOAComm::run, this);
}

void ULM3PDOAComm::stop()
{
    if (!running_) return;
    running_ = 0;
    daq_thread_.join();
}
