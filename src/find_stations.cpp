#include "find_stations.hpp"
#include "fft.hpp"
#include "filter.hpp"
#include "mixer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

// spectrum = abs(fftshift(fft(mat)));
// spectrum_mean = movmean(spectrum,1001);
// spectrum_sq = movmean(spectrum,1001) > 15000;
// spectrum_diff = diff(spectrum_sq);

// risings = find(spectrum_diff == 1);
// fallings = find(spectrum_diff == -1);
// widths = fallings - risings;
// idxs = find(((widths > 500) & (widths < 10000)) == 1);
// risings = risings(idxs);
// fallings = fallings(idxs);
// stations = Cf + (((risings + fallings) / 2) - (N/2)) * (Fs/N);

using namespace std;
namespace DSP
{
    Error findStations(double *in_i, double *in_q, size_t size, double fs, double cf, std::vector<double> &stations)
    {
        Error err;
        FFT<double> transformer;
        vector<double> fft_i;
        vector<double> fft_q;
        vector<double> fft_abs;
        size_t half_size;

        fft_i.resize(size);
        fft_q.resize(size);
        fft_abs.resize(size);
        err = transformer.transform2(in_i, in_q, fft_i.data(), fft_q.data(), size);
        if(SUCCESS != err) return err;

        half_size = size / 2;
        double *fft_i_ptr = fft_i.data();
        double *fft_q_ptr = fft_q.data();
        for(size_t ii; ii < size; ii++)
        {
            size_t offset;
            if(ii < half_size)
            {
                offset = half_size + ii;
            }
            else
            {
                offset = ii - half_size;
            }
            fft_abs[ii] = fft_i_ptr[offset] * fft_i_ptr[offset] + fft_q_ptr[offset] * fft_q_ptr[offset];
        }

        vector<double> mean_coefficients;
        const uint32_t mean_size = 1001;
        const double mean_coef = 1.0/((double)mean_size);
        mean_coefficients.resize(mean_size);
        for(uint16_t ii = 0; ii < mean_size; ii++)
        {
            mean_coefficients[ii] = mean_coef;
        }
        FirFilter<double> mean_filter(mean_coefficients);
        vector<double> filtered_fft;
        filtered_fft.resize(fft_abs.size());
        err = mean_filter.executeReal(fft_abs.data(), filtered_fft.data(), fft_abs.size());
        if(err != SUCCESS)
        {
            return err;
        }
        const double margin = 800000000;
        for(size_t ii = 0; ii < filtered_fft.size(); ii++)
        {
            if(filtered_fft[ii] > margin)
            {
                filtered_fft[ii] = 1;
            }
            else
            {
                filtered_fft[ii] = 0;
            }
        }

        vector<int64_t> risings;
        vector<int64_t> fallings;
        for(int64_t ii = 0; ii < filtered_fft.size() - 1; ii++)
        {
            if((filtered_fft[ii+1] - filtered_fft[ii]) == 1)
            {
                risings.push_back(ii);
            }
            else if((filtered_fft[ii+1] - filtered_fft[ii]) == -1)
            {
                fallings.push_back(ii);
            }
        }
        filtered_fft[filtered_fft.size() - 1] = 0;
        if(risings.size() != fallings.size())
        {
            return FAIL;
        }

        for(size_t ii = 0; ii < risings.size(); ii++)
        {
            if(((fallings[ii] - risings[ii]) > 100) && ((fallings[ii] - risings[ii]) < 15000))
            {
                double station_frequency = cf + ((fallings[ii] + risings[ii]) / 2 - ((int64_t)size / 2)) * (fs / size);
                stations.push_back(station_frequency);
            }
        }
        return SUCCESS;
    }

    template <class T> FmStationsFinder<T>::FmStationsFinder(int64_t fs, int64_t cf, size_t num_points,
        int64_t channel_step, int64_t peak_width, uint8_t peak_points, double relative_diff):
            sampling_rate(fs), central_frequency(cf), points_to_analyze(num_points), channel_step(channel_step),
            peak_width(peak_width), peak_points(peak_points), peak_diff(relative_diff)
    {
        frequencies.reserve(fs / channel_step);
        frequencies.push_back(0);
        for(int64_t ff = channel_step; ff < (fs/2); ff += channel_step)
        {
            frequencies.push_back(-ff);
            frequencies.push_back(ff);
        }
        std::sort(frequencies.begin(), frequencies.end());
        sinusoids_i.resize(frequencies.size());
        sinusoids_q.resize(frequencies.size());

        T points_count = peak_points * 2 + 1;
        for(size_t ii = 0; ii < frequencies.size(); ii++)
        {
            std::vector<T> &idata = sinusoids_i[ii];
            std::vector<T> &qdata = sinusoids_q[ii];
            idata.resize(num_points);
            qdata.resize(num_points);

            for(size_t jj = 0; jj < points_to_analyze; jj++)
            {
                T value_i = 0;
                T value_q = 0;
                T current_time = (double)jj / (double)fs;
                for(int8_t nn = -peak_points; nn <= peak_points; nn++)
                {
                    T cur_freq = (T)frequencies[ii] + nn * ((T)peak_width / points_count);
                    value_i += cos(2 * M_PI * cur_freq  * current_time);
                    value_q += -sin(2 * M_PI * cur_freq  * current_time);
                }
                value_i /= points_count;
                value_q /= points_count;
                idata[jj] = value_i;
                qdata[jj] = value_q;
            }
        }
    }

    template <class T> Error FmStationsFinder<T>::findStations(T *in_i, T *in_q, size_t buflen, std::vector<double> &station_frequencies)
    {
        if(buflen < points_to_analyze)
        {
            return FAIL;
        }

        std::vector<T> amps(frequencies.size());
        for(uint32_t ii = 0; ii < frequencies.size(); ii++)
        {
            T amp_i = 0;
            T amp_q = 0;
            std::vector<T> &idata = sinusoids_i[ii];
            std::vector<T> &qdata = sinusoids_q[ii];
            for(size_t jj = 0; jj < points_to_analyze; jj++)
            {
                T out_i, out_q;
                out_i = idata[jj] * in_i[jj] - qdata[jj] * in_q[jj];
                out_q = idata[jj] * in_q[jj] + qdata[jj] * in_i[jj];
                amp_i += out_i;
                amp_q += out_q;
            }
            amps[ii] = (amp_i * amp_i) + (amp_q * amp_q);
        }

        for(uint32_t ii = 0; ii < frequencies.size() - 2; ii++)
        {
            T lower = (amps[ii + 0] + amps[ii + 2]) / 2;
            T upper = amps[ii + 1];

            if((upper / lower) > peak_diff)
            {
                station_frequencies.push_back(central_frequency + frequencies[ii + 1]);
            }
        }

        return SUCCESS;
    }

    template class FmStationsFinder<float>;
    template class FmStationsFinder<double>;
}

