#include <sstream>

#include <Tactonic.h>
#include <TactonicTouch.h>

#include "device.h"

Device::Device() {
    this->device_list = Tactonic_GetDeviceList();
    if (this->device_list->numDevices < 1) {
        throw DeviceError("No devices found.");
    } else if (this->device_list->numDevices != 2 &&
            this->device_list->numDevices != 7) {
        std::ostringstream s;
        s << this->device_list->numDevices << " devices connected; not a "
            "square";
        throw DeviceError(s.str());
    }

    if (this->device_list->numDevices == 2) {
        this->num_devices = 2;
        for (int i = 0; i < this->num_devices; i++) {
            this->devices[i] = this->device_list->devices[i];
            this->frames[i] = Tactonic_CreateFrame(this->devices[i]);
        }
    } else {
        this->num_devices = 8;
        for (int i = 0; i < this->device_list->numDevices; i++) {
            switch (this->device_list->devices[i].serialNumber) {
            case 18285664:
                this->devices[0] = this->device_list->devices[i];
                break;
            case 18284896:
                this->devices[1] = this->device_list->devices[i];
                break;
            case 18285408:
                this->devices[2] = this->device_list->devices[i];
                break;
            // Well, I broke this...
//            case 18285152:
//                this->devices[3] = this->device_list->devices[i];
//                break;
            case 18284640:
                this->devices[4] = this->device_list->devices[i];
                break;
            case 18286432:
                this->devices[5] = this->device_list->devices[i];
                break;
            case 18286176:
                this->devices[6] = this->device_list->devices[i];
                break;
            case 18285920:
                this->devices[7] = this->device_list->devices[i];
                break;
            }
        }
        this->devices[3] = this->devices[0];
        this->devices[3].serialNumber = 0;
        for (int i = 0; i < this->num_devices; i++) {
            if (this->devices[i].serialNumber != 0) {
                this->frames[i] = Tactonic_CreateFrame(this->devices[i]);
            } else {
                this->frames[i] = new TactonicFrame;
                *(this->frames[i]) = *(this->frames[0]);
                this->frames[i]->forces =
                    new int[this->frames[i]->numForces];
            }
        }
    }

    this->composite_device = this->devices[0];
    if (this->num_devices == 2) {
        // Layout looks like:
        //   ___
        //  |_1_|
        //  |_2_|
        //
        this->composite_device.rows *= 2;
    } else {
        // Layout looks like:
        //   ___ ___
        //  |_1_|_5_|
        //  |_2_|_6_|
        //  |_3_|_7_|
        //  |_4_|_8_|
        //
        this->composite_device.rows *= 4;
        this->composite_device.cols *= 2;
    }
    /*
    for (int i = 1; i < this->num_devices; i++) {
        if (this->devices[i] != NULL) {
            this->composite_device.serialNumber +=
                this->devices[i].serialNumber;
        }
    }
    */
    this->rows = this->composite_device.rows;
    this->cols = this->composite_device.cols;

    this->composite_frame = Tactonic_CreateFrame(this->composite_device);

    for (int i = 0; i < this->num_devices; i++) {
        if (this->devices[i].serialNumber != 0) {
            Tactonic_StartDevice(this->devices[i]);
            TactonicTouch_StartDetector(this->devices[i]);
        }
    }
}

Device::~Device() {
    Tactonic_DestroyFrame(this->composite_frame);
    for (int i = 0; i < this->num_devices; i++) {
        if (this->devices[i].serialNumber != 0) {
            TactonicTouch_StopDetector(this->devices[i]);
            Tactonic_StopDevice(this->devices[i]);
            Tactonic_DestroyFrame(this->frames[i]);
        } else {
            delete[] this->frames[i]->forces;
            delete this->frames[i];
        }
    }
}

int Device::get_rows() const {
    return this->rows;
}

int Device::get_cols() const {
    return this->cols;
}

frame_t Device::get_current_frame() const {
    frame_t frame;
    frame.device = this; 
    frame.time = this->composite_frame->time;
    frame.forces = this->composite_frame->forces;
    frame.num_forces = this->composite_frame->numForces;
    frame.rows = this->composite_frame->rows;
    frame.cols = this->composite_frame->cols;
    return frame;
}

bool Device::poll() {
    bool all_updated = true;
    for (int i = 0; i < this->num_devices; i++) {
        if (this->devices[i].serialNumber != 0) {
            this->updated[i] = true;
        } else {
            double old_time = this->frames[i]->time;
            Tactonic_PollFrame(this->devices[i], this->frames[i]);
            if (this->frames[i]->time > old_time) {
                this->updated[i] = true;
            }
        }
        all_updated = all_updated && this->updated[i];
    }

    if (!all_updated) {
        return false;
    }

    for (int i = 0; i < this->num_devices; i++) {
        int num_forces = this->frames[i]->numForces;
        if (this->num_devices == 2) {
            int offset = num_forces * (this->num_devices - 1 - i);
            memcpy(&this->composite_frame->forces[offset],
                   this->frames[i]->forces,
                   sizeof (int) * num_forces);
        } else {
            // To make sense of the offsets, study the diagrams above.
            // It's important to know that forces are stored row-major,
            // and the rows are wider than the columns.
            int j;
            int rows = this->frames[i]->rows;
            int cols = this->frames[i]->cols;
            if (i < MAX_DEVICES) {
                // For the left side, we flip the rows horizontally
                for (j = 0; j < rows; j++) {
                    int k;
                    int offset = (2 * i) * num_forces + 2 * j * cols;
                    for (k = 0; k < cols; k++) {
                        this->composite_frame->forces[offset + (cols - 1 - k)] =
                            this->frames[i]->forces[j * cols + k];
                    }
                }
            } else {
                // For the right side, we flip the columns vertically
                for (j = 0; j < rows; j++) {
                    int offset = (2 * (i - 4)) * num_forces + (2 * j + 1) * cols;
                    memcpy(&this->composite_frame->forces[offset],
                           &this->frames[i]->forces[(rows - 1 - j) * cols],
                           sizeof (int) * cols);
                }
            }
            this->updated[i] = false;
        }

        if (this->frames[i]->time > this->composite_frame->time) {
            this->composite_frame->time = this->frames[i]->time;
        }
    }

    return true;
}

