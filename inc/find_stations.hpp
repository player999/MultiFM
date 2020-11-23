#ifndef _FIND_STATIONS_HPP_
#define _FIND_STATIONS_HPP_
#include "error.hpp"
#include <vector>
namespace DSP
{
    Error findStations(double *in_i, double *in_q, size_t size, double fs, double cf, std::vector<double> &stations);
}
#endif /*_FIND_STATIONS_HPP_*/