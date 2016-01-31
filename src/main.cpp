#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "controller.h"
#include "device.h"

extern "C" {

int main() {
    device_t* device = create_device();
    init_device_array(device);

    controller_info_t* controller = create_controller(device);

    while (1) {
        if (poll_frame(device)) {
            float radius;
            float direction[2];
            frame_t frame;

            get_current_frame(device, &frame);
            new_coordinates(controller, &frame, &radius, direction);
            print_debug(controller);
        }
    }

    destroy_controller(controller);
    destroy_device(device);
    return 0;
}

}
