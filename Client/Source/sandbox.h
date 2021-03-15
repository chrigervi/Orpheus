/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include "orpheus_log.h"
#include "orpheus_pipe_buffer.h"

namespace orpheus
{
	namespace sandbox {

        inline std::chrono::milliseconds millis()
        {
            using namespace std::chrono;
            return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        }

        inline void testPipeBufferPerformance(unsigned int floats = 44100, unsigned int reps = 10)
        {
            using namespace std::chrono;

            //fill pipe buffer
            orpheus::PipeBuffer pb;
            float* values = new float[floats];
            juce::Random r;
            for (unsigned int i = 0; i < floats; i++)
            {
                values[i] = r.nextFloat();
            }

            milliseconds duration = milliseconds::zero();
            for (unsigned int i = 0; i < reps; i++)
            {
                milliseconds pre = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                pb.add_data(values, floats);
                milliseconds post = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

                duration += (post - pre);
            }

            delete[] values;

            std::stringstream s;
            s << "Copying " << floats * reps << " floats took " << duration.count() << "ms!" << std::endl;
            
            orpheus::Log::log(s.str());
        }

	}
}
