#pragma once

// TODO: Use rational arithmetic

const size_t SIGNAL_LENGTH = 20;

class CyclicBuf {
public:
    CyclicBuf();

    float operator[](size_t i) const;
    float& operator[](size_t i);

    float get_head() const;
    void insert(float e);
    float convolve_point(const float* kernel, size_t kernel_size, size_t start)
        const;
private:
    float data[SIGNAL_LENGTH];
    size_t head;
};
