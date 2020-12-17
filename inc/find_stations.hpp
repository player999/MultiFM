/******************************************************************************
Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
#ifndef _FIND_STATIONS_HPP_
#define _FIND_STATIONS_HPP_
#include "error.hpp"
#include <vector>
#include <cstdint>
namespace DSP
{
    Error findStations(double *in_i, double *in_q, size_t size, double fs, double cf, std::vector<double> &stations);

    /*! Radio station finder */
    template <class T> class FmStationsFinder
    {
        public:
            /** @brief Constructor for FmStationsFinder
             * @param[in] fs Sampling rate.
             * @param[in] cf Center frequency.
             * @param[in] num_points number of signal samples to analyze.
             * @param[in] channel_step step of the grid on which station frequencies are lying
             * @param[in] peak_width bandwidth to analyze (must be much smaller than actual signal BW)
             * @param[in] peak_points number of frequency points within peak_width.
             * @param[in] relative_diff coefficient that shows how much good signal peak height must ne higher that normal noise.
             * @return It is a constructor. Nothing returned.
             * */
            FmStationsFinder(int64_t fs, int64_t cf, size_t num_points,
                int64_t channel_step, int64_t peak_width,
                uint8_t peak_points, double relative_diff);

            /** @brief Find stations function
             * @param[in] in_i input I data
             * @param[in] in_q input Q data
             * @param[in] buflen input buffer length
             * @param[out] station_frequencies vector of station frequencies that are found
             * @return It is a constructor. Nothing returned.
             * */
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

