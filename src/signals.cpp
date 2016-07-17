#include "signals.h"

CyclicBuf::CyclicBuf() : data(), head(0) {}

float CyclicBuf::operator[](size_t i) const {
    return this->data[(this->head + i) % SIGNAL_LENGTH];
}

float& CyclicBuf::operator[](size_t i) {
    return this->data[(this->head + i) % SIGNAL_LENGTH];
}

float CyclicBuf::get_head() const {
    return this->data[this->head];
}

void CyclicBuf::insert(float e) {
    if (this->head == 0) {
        this->head = SIGNAL_LENGTH - 1;
    } else {
        this->head--;
    }
    this->data[this->head] = e;
}

float CyclicBuf::convolve_point(const float* kernel, size_t kernel_size, size_t
        start) const {
    float val = 0;
    for (size_t j = 0; j < kernel_size; j++) {
        val += kernel[j] * (*this)[start + j];
    }
    return val;
}
