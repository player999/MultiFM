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
