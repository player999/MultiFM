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
#ifndef DEVICEOPTIONS_H
#define DEVICEOPTIONS_H
#include <cstdint>
#include <vector>
#include <string>
#include "receiver_locator.h"

class DeviceOptions
{
public:
    DeviceOptions();
    virtual ~DeviceOptions() = 0;
    virtual int64_t minFrequency() = 0;
    virtual int64_t maxFrequency() = 0;
    virtual std::vector<double> gains(uint8_t idx) = 0;
    virtual int64_t minBandwidth() = 0;
    virtual int64_t maxBandwidth() = 0;
};

class HackrfDeviceOptions: public DeviceOptions
{
public:
    HackrfDeviceOptions(std::string serial);
    int64_t minFrequency();
    int64_t maxFrequency();
    std::vector<double> gains(uint8_t idx);
    int64_t minBandwidth();
    int64_t maxBandwidth();
    ~HackrfDeviceOptions();
private:
    int32_t _min_lna_gain;
    int32_t _max_lna_gain;
    int32_t _step_lna_gain;
    int32_t _min_vga_gain;
    int32_t _max_vga_gain;
    int32_t _step_vga_gain;
    int64_t _min_freq;
    int64_t _max_freq;
    int64_t _min_fs;
    int64_t _max_fs;
};

class RtlsdrDeviceOptions: public DeviceOptions
{
public:
    RtlsdrDeviceOptions(std::string serial);
    int64_t minFrequency();
    int64_t maxFrequency();
    std::vector<double> gains(uint8_t idx);
    int64_t minBandwidth();
    int64_t maxBandwidth();
    ~RtlsdrDeviceOptions();
private:
    std::vector<double> _gain;
    int64_t _min_freq;
    int64_t _max_freq;
    int64_t _min_fs;
    int64_t _max_fs;
};

DeviceOptions *createDeviceOptions(const Receiver &rcv);
#endif // DEVICEOPTIONS_H
