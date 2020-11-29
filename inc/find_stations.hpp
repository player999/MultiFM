#ifndef _FIND_STATIONS_HPP_
#define _FIND_STATIONS_HPP_
#include "error.hpp"
#include <vector>
#include <cstdint>
namespace DSP
{
    Error findStations(double *in_i, double *in_q, size_t size, double fs, double cf, std::vector<double> &stations);

    template <class T> class FmStationsFinder
    {
        public:
            FmStationsFinder(int64_t fs, int64_t cf, size_t num_points,
                int64_t channel_step, int64_t peak_width,
                uint8_t peak_points, double relative_diff);
            Error findStations(T *in_i, T *in_q, size_t buflen, std::vector<double> &station_frequencies);
        private:
            int64_t sampling_rate;
            int64_t central_frequency;
            size_t points_to_analyze;
            int64_t channel_step;
            int64_t peak_width;
            uint8_t peak_points;
            double peak_diff;
            std::vector<std::vector<T>> sinusoids_i;
            std::vector<std::vector<T>> sinusoids_q;
            std::vector<int64_t> frequencies;
    };
}
#endif /*_FIND_STATIONS_HPP_*/

