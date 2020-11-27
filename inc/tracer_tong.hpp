#ifndef _TRACER_TONG_HPP_
#define _TRACER_TONG_HPP_

#include <string>
#include <vector>
#include <tuple>
#include <cstdint>

namespace DSP
{
    class TracerTong
    {
        public:
            TracerTong();
            void tick(std::string file, uint32_t linum);
            void print();
        private:
            std::vector<std::tuple<clock_t, std::string, uint32_t>> clocks;
    };
}

#endif /*_TRACER_TONG_HPP_*/
