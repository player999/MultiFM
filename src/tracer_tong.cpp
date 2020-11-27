#include "tracer_tong.hpp"
#include <iostream>
using namespace std;
namespace DSP
{
    TracerTong::TracerTong()
    {
        clocks.push_back(std::tuple<clock_t, string, uint32_t>(clock(), "Constructor", 0));
    }

    void TracerTong::tick(string file, uint32_t linum)
    {
        tuple<clock_t, string, uint32_t> old = *(clocks.end() - 1);
        clock_t old_clock = get<0>(old);
        clock_t clk = clock();
        clocks.push_back(std::tuple<clock_t, string, uint32_t>(clk, file, linum));
        cout << file << ":" << linum << "\t" << "Spent " << clk - old_clock << " ticks." << endl;
    }

    void TracerTong::print()
    {
        for(auto it = clocks.begin() + 1; it != clocks.end(); it++)
        {
            cout << get<1>(*it) << ":" << get<2>(*it) << "\t" << "Spent " << get<0>(*it) - get<0>(*(it - 1)) << " ticks." << endl;
        }
        clocks.clear();
        clocks.push_back(std::tuple<clock_t, string, uint32_t>(clock(), "Constructor", 0));
    }
}
