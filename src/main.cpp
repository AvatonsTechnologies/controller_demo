// TODO: Several objects are not freed if the program exits early (which it
// often does)
// TODO: Subroutines should not terminate the program.

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "controller.h"
#include "device.h"
#include "stick.h"

extern "C" {

int main() {
    device_t* device = create_device();
    init_device_array(device);
    controller_info_t* controller = create_controller(device);
    stick_t* stick = acquire_stick();

    while (1) {
        if (poll_frame(device)) {
            float radius;
            float direction[2];
            frame_t frame;

            get_current_frame(device, &frame);
            new_coordinates(controller, &frame, &radius, direction);
            print_debug(controller);

            stick_stat_t stat = {
                radius * direction[0],
                radius * direction[1]
            };

            write_stick(stick, &stat);
        }
    }

    destroy_stick(stick);
    destroy_controller(controller);
    destroy_device(device);
    return 0;
}

}
