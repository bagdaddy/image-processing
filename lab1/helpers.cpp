#include <iostream>
#include <iomanip>

uint32_t min(uint32_t* data, uint32_t n) {
    uint32_t minpx = data[0];

    for (int i = 0; i < n; i++) {
        if (minpx > data[i]) {
            minpx = data[i];
        }
    }

    return minpx;
};

uint32_t max(uint32_t* data, uint32_t n) {
    uint32_t maxpx = data[0];

    for (int i = 0; i < n; i++) {
        if (maxpx < data[i]) {
            maxpx = data[i];
        }
    }

    return maxpx;
};

float min(float* data, uint32_t n) {
    float minpx = data[0];

    for (int i = 0; i < n; i++) {
        if (minpx > data[i]) {
            minpx = data[i];
        }
    }

    return minpx;
};

float max(float* data, uint32_t n) {
    float maxpx = data[0];

    for (int i = 0; i < n; i++) {
        if (maxpx < data[i]) {
            maxpx = data[i];
        }
    }

    return maxpx;
};