#pragma once

// Here we find all the signal processing logic that determines the
// direction and degree of tilt of the virtual joystick. The cursor
// files provide interpolation between frames.

#include "signals.h"

class Device;

struct frame_s;
typedef struct frame_s frame_t;

class Controller {
public:
    Controller(const Device& device);
    ~Controller();
    float get_current_radius() const;
    void get_current_direction(float* out) const;
    void add_frame(const frame_t& frame);
private:
    float center[2];

    float* force_coeffs;
    float* dir_coeffs[2];

    CyclicBuf sig_force;
    CyclicBuf sig_steps;
    int hysteresis_level;
    // Number of frames to average when determining frequency
    int frequency_frames;
    CyclicBuf sig_frequency;
    CyclicBuf sig_radius;

    CyclicBuf sig_dir[2];
    // Average from this many recent frames
    int dir_frames_to_average;
    CyclicBuf sig_final_dir[2];

    float final_radius;
};
