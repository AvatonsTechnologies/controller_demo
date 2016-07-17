// Structures to acquire and feed a virtual joystick

#pragma once

#include <stdexcept>
#include <string>

class JoystickError : public std::runtime_error {
public:
    explicit JoystickError(const std::string& what_arg) :
        std::runtime_error(what_arg) {}
    explicit JoystickError(const char* what_arg) :
        std::runtime_error(what_arg) {}
};

typedef struct stick_stat_s {
    // Axis values range from -1 to 1
    float axis_x;
    float axis_y;
} stick_stat_t;

// Interface for concrete joystick backends
class Joystick {
public:
    virtual ~Joystick() {};
    virtual void set_state(const stick_stat_t& stat) = 0;
};

// Joystick powered by regular old vJoy
class VJoyJoystick : public Joystick {
public:
    VJoyJoystick();
    ~VJoyJoystick();
    void set_state(const stick_stat_t& state);
private:
    unsigned int device_id;
};

// Joystick powered by modified ScpVBus/vJoy/vXbox
class VXboxJoystick : public Joystick {
public:
    VXboxJoystick();
    ~VXboxJoystick();
    void set_state(const stick_stat_t& state);
private:
    unsigned int device_id;
};
