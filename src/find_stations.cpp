#include "find_stations.hpp"
#include "fft.hpp"
#include "filter.hpp"
#include <iostream>

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

        for(size_t ii = 0; ii < stations.size(); ii++)
        {
            cout << stations[ii] << endl;
        }

        return SUCCESS;
    }
}