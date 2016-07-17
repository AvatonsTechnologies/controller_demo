#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <OVR_CAPI.h>

#include "controller.h"
#include "device.h"
#include "stick.h"

const float RAD_TO_DEG = 180.0f / (2.0f * 3.14159265358979323f);

int main() {
    VXboxJoystick stick;
    Device device;
    Controller controller(device);

    ovrSession session;
    ovrGraphicsLuid luid;
    {
        ovrResult result = ovr_Initialize(0);
        if (OVR_FAILURE(result)) {
            printf("Failed to initialize Rift SDK.");
            return 0;
        }
        result = ovr_Create(&session, &luid);
        if (OVR_FAILURE(result)) {
            printf("Failed to create Rift SDK session.");
            ovr_Shutdown();
            return 0;
        }
    }

    float c = 0.0f;
    float s = 0.0f;
    float radius = 0.0f;
    float direction[2] = { 0.0f, 0.0f };
    float yaw = 0.0f;
    while (true) {

        ovrTrackingState ts = ovr_GetTrackingState(
            session, ovr_GetTimeInSeconds(), 0);
        if (ts.StatusFlags & (ovrStatus_OrientationTracked |
                ovrStatus_PositionTracked)) {
            ovrPoseStatef pose = ts.HeadPose;
            ovrQuatf q = pose.ThePose.Orientation;
            yaw = atan2f(2 * (q.x * q.z + q.y * q.w),
                               1 - 2 * (q.y * q.y + q.z * q.z));
            c = cosf(yaw);
            s = sinf(yaw);
        }

        if (device.poll()) {
            frame_t frame = device.get_current_frame();
            controller.add_frame(frame);
            radius = controller.get_current_radius();
            controller.get_current_direction(direction);
        }

        stick_stat_t stat = {
            radius * (c * direction[0] + -s * direction[1]),
            radius * (s * direction[0] + c * direction[1])
        };

        stick.set_state(stat);
    }

    ovr_Destroy(session);
    ovr_Shutdown();
    return 0;
}
