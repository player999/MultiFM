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
    virtual ~DeviceOptions(){};
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
