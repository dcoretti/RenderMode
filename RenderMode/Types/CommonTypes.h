#pragma once

struct SystemBuffer {
    SystemBuffer(void * data, unsigned int sizeBytes) : data(data), sizeBytes(sizeBytes) {}
    SystemBuffer() = default;

    void * data{ nullptr };
    unsigned int sizeBytes{ 0 };
};