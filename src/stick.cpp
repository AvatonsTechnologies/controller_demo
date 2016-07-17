// Virtual gamepad/joystick. Presently a virtual XInput device powered by vJoy.
#include <sstream>

#include <windows.h>
#include <public.h>
#include <vjoyinterface.h>
#include <vXboxInterface.h>

#include "stick.h"

const unsigned int DEFAULT_VXBOX_DEV_ID = 1;
const unsigned int MAX_VXBOX_DEV_ID = 4;

const short VXBOX_AXIS_CENTER = 0;
const short VXBOX_AXIS_RADIUS = 32767;

VXboxJoystick::VXboxJoystick() {
    this->device_id = 0;

    if (!isVBusExists()) {
        throw JoystickError("ScpVBus not installed.");
    }

    UINT i;
    for (i = DEFAULT_VXBOX_DEV_ID; i <= MAX_VXBOX_DEV_ID; i++) {
        if (isControllerOwned(i) || !isControllerExists(i)) {
            this->device_id = i;
            break;
        }
    }
    if (this->device_id > MAX_VXBOX_DEV_ID) {
        throw JoystickError("No available vXbox devices");
    }

    if (!isControllerExists(i) && !PlugIn(i)) {
        UnPlug(i);
        if (!PlugIn(i)) {
            std::ostringstream s;
            s << "Failed to acquire vXbox device " << this->device_id;
            throw JoystickError(s.str());
        }
    }
}

VXboxJoystick::~VXboxJoystick() {
    UnPlug(this->device_id);
}

void VXboxJoystick::set_state(const stick_stat_t& stat) {
    short axisx = VXBOX_AXIS_CENTER + (short)(stat.axis_x * VXBOX_AXIS_RADIUS);
    short axisy = VXBOX_AXIS_CENTER + (short)(stat.axis_y * VXBOX_AXIS_RADIUS);
    short axisrx = VXBOX_AXIS_CENTER;
    short axisry = VXBOX_AXIS_CENTER;
    if (!SetAxisX(this->device_id, axisx)
            || !SetAxisY(this->device_id, axisy)
            || !SetAxisRx(this->device_id, axisrx)
            || !SetAxisRy(this->device_id, axisry)) {
        std::ostringstream s;
        s << "Couldn't write to vXbox device " << this->device_id;
        throw JoystickError(s.str());
    }
}

const unsigned int DEFAULT_VJOY_DEV_ID = 1;
const unsigned int MAX_VJOY_DEV_ID = 16;

const long VJOY_AXIS_CENTER = 16000L;
const long VJOY_AXIS_RADIUS = 16000L;

VJoyJoystick::VJoyJoystick() {
    this->device_id = 0;

    if (!vJoyEnabled()) {
        throw JoystickError("vJoy not available");
    }

    UINT i;
    for (i = DEFAULT_VJOY_DEV_ID; i <= MAX_VJOY_DEV_ID; i++) {
        VjdStat status = GetVJDStatus(i);
        if (status == VJD_STAT_OWN || status == VJD_STAT_FREE) {
            this->device_id = i;
            break;
        }
    }
    if (this->device_id > MAX_VJOY_DEV_ID) {
        throw JoystickError("No available vJoy devices");
    }

    if (!AcquireVJD(this->device_id)) {
        RelinquishVJD(this->device_id);

        std::ostringstream s;
        s << "Failed to acquire vJoy device " << this->device_id;
        throw JoystickError(s.str());
    }
}

VJoyJoystick::~VJoyJoystick() {
    RelinquishVJD(this->device_id);
}

void VJoyJoystick::set_state(const stick_stat_t& stat) {
    BYTE id = (BYTE)this->device_id;
    JOYSTICK_POSITION_V2 report;
    report.bDevice = id;
    report.wAxisX = VJOY_AXIS_CENTER + (short)(stat.axis_x * VJOY_AXIS_RADIUS);
    report.wAxisY = VJOY_AXIS_CENTER + (short)(stat.axis_y * VJOY_AXIS_RADIUS);
    report.wAxisXRot = VJOY_AXIS_CENTER;
    report.wAxisYRot = VJOY_AXIS_CENTER;
    if (!UpdateVJD(this->device_id, &report)) {
        std::ostringstream s;
        s << "Couldn't write to vJoy device " << this->device_id;
        throw JoystickError(s.str());
    }
}
