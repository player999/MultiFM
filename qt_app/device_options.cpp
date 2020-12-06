#include "device_options.h"
#include "receiver_locator.h"
#include "rtl-sdr.h"

using namespace std;

DeviceOptions::DeviceOptions() { }

HackrfDeviceOptions::HackrfDeviceOptions(std::string serial __attribute__((unused)))
{
    /*[-l gain_db] # RX LNA (IF) gain, 0-40dB, 8dB steps
      [-g gain_db] # RX VGA (baseband) gain, 0-62dB, 2dB steps*/
    _min_lna_gain = 0;
    _max_lna_gain = 40;
    _step_lna_gain = 8;

    _min_vga_gain = 0;
    _max_vga_gain = 62;
    _step_vga_gain = 2;

    _min_fs = 8000000;
    _max_fs = 20000000;

    _min_freq = 0;
    _max_freq = 7250000000;
}

HackrfDeviceOptions::~HackrfDeviceOptions() {}

std::vector<double> HackrfDeviceOptions::gains(uint8_t idx)
{
    std::vector<double> rval;
    if(0 == idx)
    {
        for(int32_t g = _min_lna_gain; g <= _max_lna_gain; g+= _step_lna_gain)
        {
            rval.push_back(g);
        }
    }
    else if(1 == idx)
    {
        for(int32_t g = _min_vga_gain; g <= _max_vga_gain; g+= _step_vga_gain)
        {
            rval.push_back(g);
        }
    }
    return rval;
}

int64_t HackrfDeviceOptions::minFrequency()
{
    return _min_freq;
}

int64_t HackrfDeviceOptions::maxFrequency()
{
    return _max_freq;
}

int64_t HackrfDeviceOptions::minBandwidth()
{
    return _min_fs;
}

int64_t HackrfDeviceOptions::maxBandwidth()
{
    return _max_fs;
}

RtlsdrDeviceOptions::RtlsdrDeviceOptions(std::string serial)
{
    int dev_idx;
    int err;
    int gain_count;
    rtlsdr_dev_t *dev;
    enum rtlsdr_tuner tuner_type;

    dev_idx = rtlsdr_get_index_by_serial(serial.c_str());
    if(dev_idx < 0) throw runtime_error("Could not find device with given serial");

    err = rtlsdr_open(&dev, dev_idx);
    if(err < 0) throw runtime_error("Could not open device");

    tuner_type = rtlsdr_get_tuner_type(dev);
    switch(tuner_type)
    {
        case RTLSDR_TUNER_E4000:
            _min_freq = 52000000;
            _max_freq = 2200000000;
            break;
        case RTLSDR_TUNER_FC0012:
            _min_freq = 22000000;
            _max_freq = 948600000;
            break;
        case RTLSDR_TUNER_FC0013:
            _min_freq = 22000000;
            _max_freq = 1100000000;
            break;
        case RTLSDR_TUNER_FC2580:
            _min_freq = 146000000;
            _max_freq = 308000000;
            break;
        case RTLSDR_TUNER_R820T:
        case RTLSDR_TUNER_R828D:
            _min_freq = 24000000;
            _max_freq = 1766000000;
            break;
        default:
            (void)rtlsdr_close(dev);
            throw runtime_error("Unknown tuner type");
    }
    _min_fs = 900001;
    _max_fs = 2400000;
    gain_count = rtlsdr_get_tuner_gains(dev, NULL);
    if(gain_count <= 0)
    {
        (void)rtlsdr_close(dev);
        throw runtime_error("Failed to get gains available");
    }

    vector<int> int_gain(gain_count);
    (void)rtlsdr_get_tuner_gains(dev, int_gain.data());

    for(int g: int_gain)
    {
        _gain.push_back(((double)g) / 10.0);
    }

    (void)rtlsdr_close(dev);
}

RtlsdrDeviceOptions::~RtlsdrDeviceOptions() {}

int64_t RtlsdrDeviceOptions::minFrequency()
{
    return _min_freq;
}

int64_t RtlsdrDeviceOptions::maxFrequency()
{
    return _max_freq;
}

std::vector<double> RtlsdrDeviceOptions::gains(uint8_t idx)
{
    if(idx == 0)
    {
        return _gain;
    }
    else
    {
        return vector<double>();
    }
}

int64_t RtlsdrDeviceOptions::minBandwidth()
{
    return _min_fs;
}

int64_t RtlsdrDeviceOptions::maxBandwidth()
{
    return _max_fs;
}

DeviceOptions *createDeviceOptions(const Receiver &rcv)
{
    if(rcv.getType() == ReceiverType::HACKRF)
    {
        return dynamic_cast<HackrfDeviceOptions*>(new HackrfDeviceOptions(rcv.getSerial().toStdString()));
    }
    else if(rcv.getType() == ReceiverType::RTLSDR)
    {
        return dynamic_cast<RtlsdrDeviceOptions*>(new RtlsdrDeviceOptions(rcv.getSerial().toStdString()));
    }
    else
    {
        return NULL;
    }
}
