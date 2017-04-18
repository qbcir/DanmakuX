#ifndef __dx_RANDOMHELPER_H__
#define __dx_RANDOMHELPER_H__

#include <random>

class RandHelper
{
public:
    RandHelper();
    RandHelper(uint32_t seed);
    float sampleNormal(float mean, float stddev);
    std::vector<float> sampleNormal(float mean, float stddev, size_t n);
    float sampleTruncNormal(float min, float max);
    std::vector<float> sampleTruncNormal(float min, float max, size_t n);
    float sampleUniform(float min, float max);
protected:
    std::mt19937 m_gen;
    static std::random_device m_rd;
};

class RandBits {
public:
    bool operator() (size_t x, size_t y) {
        if ((m_idx & 0xF) == 0) {
            m_rnd = std::rand();
        }
        return (m_rnd >> (m_idx & 0xF)) & 1;
    }
private:
    size_t m_idx = 0;
    uint16_t m_rnd = 0;
};

#endif // __dx_RANDOMHELPER_H__
