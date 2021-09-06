#pragma once

#include "Project.h"

namespace re  {

template<typename T>
class MeanBuffer {
public:

    MeanBuffer(size_t samples, T initValue = 0, bool initialize = false) {
        m_samples_history_length = samples;
        m_accumulator = 0;
        m_pointer = 0;
        m_cycles = 0;
        m_sample_history = new T[samples];

        if(initialize)
            reset(initValue);
    }

    ~MeanBuffer() {
        delete []m_sample_history;
    }

    void reset(T sample) {
        for(size_t i = 0; i < m_samples_history_length; ++i)
            accumulate(sample);
    }

    void accumulate(T sample) {
        if (m_cycles != 0) {
            m_accumulator -= m_sample_history[m_pointer];
        }

        m_sample_history[m_pointer] = sample;
        m_accumulator += sample;

        m_pointer++;

        if (m_pointer == m_samples_history_length) {
            m_cycles++;
            m_pointer = 0;
        }
    }

    T mean() {
        if (m_cycles != 0) {
            return m_accumulator / T(m_samples_history_length);
        } else if (m_pointer > 0) {
            return m_accumulator /  T(m_pointer);
        } else {
            return 0;
        }
    }

    T getMin() {
        if (m_cycles != 0) {
            T m = m_sample_history[0];
            for(size_t i = 1; i < m_samples_history_length; ++i) {
                if(m_sample_history[i] < m) {
                    m = m_sample_history[i];
                }
            }

            return m;
        }
        else {
            return 0;
        }
    }

    T getMax() {
        if (m_cycles != 0) {
            T m = m_sample_history[0];
            for(size_t i = 1; i < m_samples_history_length; ++i) {
                if(m_sample_history[i] > m) {
                    m = m_sample_history[i];
                }
            }

            return m;
        }
        else {
            return 0;
        }
    }

    T accumulateAndGet(T sample) {
        accumulate(sample);
        return mean();
    }

private:
    size_t m_pointer;
    size_t m_samples_history_length;
    size_t m_cycles;
    T m_accumulator;
    T * m_sample_history;
};

using MeanBufferLong = MeanBuffer<Int64>;
using MeanBufferDouble = MeanBuffer<double>;
using MeanBufferFloat = MeanBuffer<float>;

typedef std::shared_ptr<MeanBufferLong> MeanBufferLongPtr;
typedef std::shared_ptr<MeanBufferDouble> MeanBufferDoublePtr;
typedef std::shared_ptr<MeanBufferFloat> MeanBufferFloatPtr;

}

