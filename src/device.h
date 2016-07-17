#pragma once

#include <stdexcept>
#include <string>

#include <Tactonic.h>

class DeviceError : public std::runtime_error {
public:
    explicit DeviceError(const std::string& what_arg) :
        std::runtime_error(what_arg) {}
    explicit DeviceError(const char* what_arg) :
        std::runtime_error(what_arg) {}
};

class Device;

typedef struct frame_s {
    const Device* device;
    double time;
    int* forces;
    int num_forces;
    int rows;
    int cols;
} frame_t;

const size_t MAX_DEVICES = 8;

class Device {
public:
    Device();
    ~Device();
    int get_rows() const;
    int get_cols() const;
    frame_t get_current_frame() const;
    bool poll();
private:
    int rows;
    int cols;

    int num_devices;
    TactonicDeviceList* device_list;
    TactonicDevice composite_device;
    TactonicDevice devices[MAX_DEVICES];

    TactonicFrame* composite_frame;
    TactonicFrame* frames[MAX_DEVICES];

    bool updated[MAX_DEVICES];
};

