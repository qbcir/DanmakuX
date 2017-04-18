#include "RandomHelper.h"
#include "cocos2d.h"

std::random_device RandHelper::m_rd;

RandHelper::RandHelper() :
    m_gen(m_rd())
{}

RandHelper::RandHelper(uint32_t seed) :
    m_gen(seed)
{}

std::vector<float> RandHelper::sampleNormal(float mean, float stddev, size_t n) {
    std::vector<float> vs;
    std::normal_distribution<float> nd(mean, stddev);
    for (size_t i = 0; i < n; i++) {
        vs.emplace_back(nd(m_gen));
    }
    return vs;
}

float RandHelper::sampleNormal(float mean, float stddev) {
    std::normal_distribution<float> nd(mean, stddev);
    return nd(m_gen);
}

float RandHelper::sampleTruncNormal(float min, float max) {
    float mean = (max + min) / 2;
    float stddev = (max - min) / 12;
    float v = sampleNormal(mean, stddev);
    if (v < min) {
        v = min;
    }
    if (v > max) {
        v = max;
    }
    return v;
}

std::vector<float> RandHelper::sampleTruncNormal(float min, float max, size_t n) {
    float mean = (max + min) / 2;
    float stddev = (max - min) / 6;
    std::vector<float> vs = sampleNormal(mean, stddev, n);
    for (size_t i = 0; i < vs.size(); i++) {
        if (vs[i] < min) {
            vs[i] = min;
        }
        if (vs[i] > max) {
            vs[i] = max;
        }
    }
    return vs;
}

float RandHelper::sampleUniform(float min, float max) {
    std::uniform_real_distribution<float> rd(min, max);
    return rd(m_gen);
}

