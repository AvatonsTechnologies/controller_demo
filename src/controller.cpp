#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "controller.h"
#include "device.h"
#include "signals.h"

// All of these constants are made up
const float DEAD_ZONE_RADIUS = 0.33f;

const float MIN_FORCE = 20000;

// A smoothing and differentiating kernel
const float DIFF_KERNEL[4] = {
    0.7f, -0.5f, -0.1f, -0.1f
};

const float STEP_HYSTERESIS_HIGH = 25000;
const float STEP_HYSTERESIS_LOW = 10000;

const float FREQ_LEVELS[5] = {
    0.05f, 0.1f, 0.15f, 0.2f, 0.25f
};

const int FREQUENCY_FRAMES[5] = {
    7, 18, 16, 14, 13
};

const float TRANSITION_KERNEL[4] = {
    0.165f, 0.33f, 0.33f, 0.165f
};

// Determines whether average direction ill-defined
const float MIN_DIR_LEN_SQ = 1.0e-4f;

const int DIR_FRAMES[5] = {
    5, 14, 12, 10, 9
};

Controller::Controller(const Device& device) :
        hysteresis_level(0),
        frequency_frames(1),
        dir_frames_to_average(1) {
    int rows = device.get_rows();
    int cols = device.get_cols();
    this->center[0] = (rows - 1) / 2.0f;
    this->center[1] = (cols - 1) / 2.0f;

    this->force_coeffs = (float*)malloc(rows * cols * sizeof(float));
    this->dir_coeffs[0] = (float*)malloc(rows * cols * sizeof(float));
    this->dir_coeffs[1] = (float*)malloc(rows * cols * sizeof(float));

    float r, r2;
    if (rows < cols) {
        r = DEAD_ZONE_RADIUS * rows / 2;
    } else {
        r = DEAD_ZONE_RADIUS * cols / 2;
    }
    r2 = r * r;
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            float x = (i - this->center[0]);
            float y = (j - this->center[1]);
            float lensq = x * x + y * y;
            if (lensq < r2) {
                this->force_coeffs[i * cols + j] = 0.0f;
                this->dir_coeffs[0][i * cols + j] = 0.0f;
                this->dir_coeffs[1][i * cols + j] = 0.0f;
            }
            else {
                float len = sqrtf((float)lensq);
                this->force_coeffs[i * cols + j] = 1.0f;
                this->dir_coeffs[0][i * cols + j] = x / len;
                this->dir_coeffs[1][i * cols + j] = y / len;
            }
        }
    }
}

Controller::~Controller() {
    free(this->force_coeffs);
    free(this->dir_coeffs[0]);
    free(this->dir_coeffs[1]);
}

float Controller::get_current_radius() const {
    return this->final_radius;
}

void Controller::get_current_direction(float* out) const {
    out[0] = this->sig_final_dir[0].get_head();
    out[1] = this->sig_final_dir[1].get_head();
}

void Controller::add_frame(const frame_t& frame) {

    // Compute force and direction

    float total_force = 0.0f;
    float avg_dir[2] = { 0.0f, 0.0f };
    int i, j;
    for (i = 0; i < frame.rows; i++) {
        for (j = 0; j < frame.cols; j++) {
            // Is it faster to check if outside dead zone here?
            int idx = i * frame.cols + j;
            total_force += this->force_coeffs[idx] * frame.forces[idx];
            avg_dir[0] += this->dir_coeffs[0][idx] * frame.forces[idx];
            avg_dir[1] += this->dir_coeffs[1][idx] * frame.forces[idx];
        }
    }

    float dir_len_sq = avg_dir[0] * avg_dir[0] + avg_dir[1] * avg_dir[1];
    if (dir_len_sq < MIN_DIR_LEN_SQ || total_force < MIN_FORCE) {
        avg_dir[0] = 0.0f;
        avg_dir[1] = 0.0f;
    } else {
        float dir_len = sqrtf(dir_len_sq);
        avg_dir[0] /= dir_len;
        avg_dir[1] /= dir_len;
    }

    this->sig_force.insert(total_force);
    this->sig_dir[0].insert(avg_dir[0]);
    this->sig_dir[1].insert(avg_dir[1]);

    // Identify steps

    float diff = this->sig_force.convolve_point(DIFF_KERNEL, 4, 0);
    switch (this->hysteresis_level) {
    case -1:
        if (diff > STEP_HYSTERESIS_HIGH) {
            this->hysteresis_level = 1;
            this->sig_steps.insert(1);
        } else {
            if (diff > -STEP_HYSTERESIS_LOW) {
                this->hysteresis_level = 0;
            }
            this->sig_steps.insert(0);
        }
    case 0:
        if (diff > STEP_HYSTERESIS_HIGH) {
            this->hysteresis_level = 1;
            this->sig_steps.insert(1);
        } else if (diff < -STEP_HYSTERESIS_LOW) {
            this->hysteresis_level = -1;
            this->sig_steps.insert(-1);
        } else {
            this->sig_steps.insert(0);
        }
    case 1:
        if (diff < -STEP_HYSTERESIS_HIGH) {
            this->hysteresis_level = -1;
            this->sig_steps.insert(-1);
        } else {
            if (diff < STEP_HYSTERESIS_LOW) {
                this->hysteresis_level = 0;
            }
            this->sig_steps.insert(0);
        }
    }

    // Update counters

    float prev_dir[2];
    prev_dir[0] = this->sig_final_dir[0][1];
    prev_dir[1] = this->sig_final_dir[1][1];
    if (prev_dir[0] * avg_dir[0] + prev_dir[1] * avg_dir[1] < -0.25f) {
        this->frequency_frames = 1;
        this->dir_frames_to_average = 1;
    }

    int level;

    if (this->sig_frequency.get_head() < FREQ_LEVELS[0]) {
        level = 0;
    } else if (this->sig_frequency.get_head() < FREQ_LEVELS[1]) {
        level = 1;
    } else if (this->sig_frequency.get_head() < FREQ_LEVELS[2]) {
        level = 2;
    } else if (this->sig_frequency.get_head() < FREQ_LEVELS[3]) {
        level = 3;
    } else {
        level = 4;
    }

    if (this->frequency_frames < FREQUENCY_FRAMES[level]) {
        this->frequency_frames++;
    } else if (this->frequency_frames > FREQUENCY_FRAMES[level]) {
        this->frequency_frames--;
    }
    if (this->dir_frames_to_average < DIR_FRAMES[level]) {
        this->dir_frames_to_average++;
    } else if (this->dir_frames_to_average > DIR_FRAMES[level]) {
        this->dir_frames_to_average--;
    }

    // Update frequency

    float frequency = 0;
    if (this->sig_frequency.get_head() < FREQ_LEVELS[0]) {
        // When moving very slowly or standing still, ignore up steps
        for (i = 0; i < this->frequency_frames; i++) {
            float a = this->sig_steps[i];
            if (a > 0) {
                frequency += a;
            }
        }
    } else {
        for (i = 0; i < this->frequency_frames; i++) {
            frequency += fabsf(this->sig_steps[i]);
        }
    }
    frequency /= this->frequency_frames;
    this->sig_frequency.insert(frequency);

    // Set radius

    float final_radius = 0.0f;
    float transition_level =
        this->sig_frequency.convolve_point(TRANSITION_KERNEL, 4, 0);
    if (transition_level > 0.25f) {
        this->final_radius = 1.0f;
    } else {
        // This constant too
        this->final_radius = 4.0f * transition_level;
    }
    this->sig_radius.insert(this->final_radius);

    // Average out direction

    float final_dir[2] = { 0.0f, 0.0f };
    for (i = 0; i < this->dir_frames_to_average; i++) {
        prev_dir[0] = this->sig_dir[0][i];
        prev_dir[1] = this->sig_dir[1][i];
        final_dir[0] += prev_dir[0];
        final_dir[1] += prev_dir[1];
    }
    float final_len_sq = final_dir[0] * final_dir[0] + final_dir[1] * final_dir[1];
    if (final_len_sq < MIN_DIR_LEN_SQ) {
        final_dir[0] = 0.0f;
        final_dir[1] = 0.0f;
    }  else {
        float final_len = sqrtf(final_len_sq);
        final_dir[0] /= final_len;
        final_dir[1] /= final_len;
    }
    this->sig_final_dir[0].insert(final_dir[0]);
    this->sig_final_dir[1].insert(final_dir[1]);
}
