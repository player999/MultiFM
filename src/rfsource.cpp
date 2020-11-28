#include "rfsource.hpp"
#include "interleaving.hpp"
#include "error.hpp"
#include <algorithm>
#include <exception>
#include <string>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <cstring>

extern "C"
{
    static int rx_callback(hackrf_transfer* transfer);
    static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);
}
namespace DSP
{
    bool operator== (const ConfigEntry &c1, const ConfigEntry &c2)
    {
        return (c1.name == c2.name);
    }

    ConfigEntry::ConfigEntry(const std::string &n)
    {
        name = n;
        type = NONE;
    }

    ConfigEntry::ConfigEntry()
    {
        name = "defaultName";
        type = NONE;
    }

    ConfigEntry::ConfigEntry(const std::string &n, const std::string &v)
    {
        name = n;
        type = STRING;
        s = v;
    }

    ConfigEntry::ConfigEntry(const std::string &n, double v)
    {
        name = n;
        type = FLOAT;
        d = v;
    }

    ConfigEntry::ConfigEntry(const std::string &n, int64_t v)
    {
        name = n;
        type = INTEGER;
        i = v;
    }

    std::string &ConfigEntry::getName()
    {
        return name;
    }

    ConfigType ConfigEntry::getType()
    {
        return type;
    }

    std::string &ConfigEntry::getString()
    {
        return s;
    }

    double ConfigEntry::getFloat()
    {
        return d;
    }

    int64_t ConfigEntry::getInt()
    {
        return i;
    }

    FileSource::FileSource(std::string &fname, size_t portion, uint64_t interval_nsec)
    {
        portion_size = portion;
        interval = interval_nsec;
        fh = fopen(fname.c_str(), "rb");
    }

    FileSource::~FileSource()
    {
        fclose(fh);
    }

    void FileSource::registerQueue(std::queue<RfChunk> *q)
    {
        iq_queue = q;
    }

    void FileSource::dataHandler(FileSource *fs)
    {
        while(true)
        {
            RfChunk chunk;
            int8_t *buffer;
            Error err;
            if(true == fs->stopsignal)
            {
                return;
            }

            buffer = new int8_t[fs->portion_size];
            size_t actual_size = fread(buffer,1,fs->portion_size, fs->fh);
            if(0 == actual_size)
            {
                /* End of the file */
                delete buffer;
                return;
            }
            err = deinterleave(buffer, actual_size, chunk.I, chunk.Q);
            delete buffer;
            if(err != SUCCESS)
            {
                return;
            }
            fs->iq_queue->push(chunk);
            std::this_thread::sleep_for(std::chrono::nanoseconds(fs->interval));
        }
    }

    void FileSource::start()
    {
        stopsignal = false;
        read_thread = new std::thread(dataHandler, this);
        return;
    }

    void FileSource::stop()
    {
        stopsignal = true;
        read_thread->join();
        delete read_thread;
        return;
    }

    HackrfSource::HackrfSource(double cf, double fs, uint8_t lna, uint8_t vga)
    {
        int result;
        result = hackrf_init();
	    if(result != HACKRF_SUCCESS)
        {
            std::__throw_runtime_error("Failed to init hackrf");
        }

        result = hackrf_open(&device);
	    if(result != HACKRF_SUCCESS)
        {
            std::__throw_runtime_error("hackrf_open_by_serial() failed");
	    }

        commonConstructor(cf, fs, lna, vga);
    }

    HackrfSource::HackrfSource(double cf, double fs, uint8_t lna, uint8_t vga, std::string &serial)
    {
        int result;
        result = hackrf_init();
	    if(result != HACKRF_SUCCESS)
        {
            std::__throw_runtime_error("Failed to init hackrf");
        }

        result = hackrf_open_by_serial(serial.c_str(), &device);
	    if(result != HACKRF_SUCCESS)
        {
            std::__throw_runtime_error("hackrf_open_by_serial() failed");
	    }

        commonConstructor(cf, fs, lna, vga);
    }

    void HackrfSource::commonConstructor(double cf, double fs, uint8_t lna, uint8_t vga)
    {
        int result;
        result = hackrf_set_sample_rate(device, (uint32_t)fs);
        if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("hackrf_set_sample_rate()");
        }

        result = hackrf_set_baseband_filter_bandwidth(device, (uint32_t)(fs/2));
        if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("hackrf_set_baseband_filter_bandwidth()");
        }

        result = hackrf_set_freq(device, (uint32_t)cf);
		if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("hackrf_set_freq()");
		}

        result = hackrf_set_vga_gain(device, vga);
        if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("hackrf_set_vga_gain()");
		}

		result = hackrf_set_lna_gain(device, lna);
        if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("hackrf_set_lna_gain()");
		}
    }

    HackrfSource::~HackrfSource()
    {
        hackrf_close(device);
        hackrf_exit();
    }

    void HackrfSource::registerQueue(std::queue<RfChunk> *q)
    {
        iq_queue = q;
    }

    void HackrfSource::start()
    {
        int result;
        result = hackrf_start_rx(device, rx_callback, this);
        if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("Failed to stop receive process");
        }
    }

    void HackrfSource::stop()
    {
        int result;
        result = hackrf_stop_rx(device);
        if( result != HACKRF_SUCCESS )
        {
            std::__throw_runtime_error("Failed to stop receive process");
        }
    }

    void HackrfSource::dataHandler(int8_t *buffer, size_t valid_length)
    {
        RfChunk chunk;
        Error err;
        err = deinterleave(buffer, valid_length, chunk.I, chunk.Q);
        if(err != SUCCESS)
        {
            return;
        }
        iq_queue->push(chunk);
    }

    RtlSdrSource::RtlSdrSource(uint32_t cf, uint32_t fs): RtlSdrSource(cf, fs, true, (uint8_t)0, std::string("")) {}

    static int verbose_device_search(char *s)
    {
        int i, device_count, device, offset;
        char *s2;
        char vendor[256], product[256], serial[256];
        device_count = rtlsdr_get_device_count();
        if (!device_count) return -1;
        for (i = 0; i < device_count; i++)
        {
            rtlsdr_get_device_usb_strings(i, vendor, product, serial);
        }
        fprintf(stderr, "\n");
        /* does string look like raw id number */
        device = (int)strtol(s, &s2, 0);
        if (s2[0] == '\0' && device >= 0 && device < device_count) {
            return device;
        }
        /* does string exact match a serial */
        for (i = 0; i < device_count; i++) {
            rtlsdr_get_device_usb_strings(i, vendor, product, serial);
            if (strcmp(s, serial) != 0) {
                continue;}
            device = i;
            return device;
        }
        /* does string prefix match a serial */
        for (i = 0; i < device_count; i++) {
            rtlsdr_get_device_usb_strings(i, vendor, product, serial);
            if (strncmp(s, serial, strlen(s)) != 0) {
                continue;}
            device = i;
            return device;
        }
        /* does string suffix match a serial */
        for (i = 0; i < device_count; i++) {
            rtlsdr_get_device_usb_strings(i, vendor, product, serial);
            offset = strlen(serial) - strlen(s);
            if (offset < 0) {
                continue;}
            if (strncmp(s, serial+offset, strlen(s)) != 0) {
                continue;}
            device = i;
            return device;
        }
        return -1;
    }

    RtlSdrSource::RtlSdrSource(uint32_t cf, uint32_t sampling_rate, bool automatic_gain, uint8_t gain, const std::string &serial)
    {
        char vendor[256], product[256], cserial[256];
        int retval;
        int devnum;

        int device_count = rtlsdr_get_device_count();
        if(device_count == 0)
        {
            std::__throw_runtime_error("Could not find any RTLSDR device");
        }

        if(serial.length() != 0)
        {
            devnum = rtlsdr_get_index_by_serial(serial.c_str());
            if(retval = rtlsdr_open(&device, devnum))
            {
                std::__throw_runtime_error("Failed to open RTLSDR. Error #" + retval);
            }
        }
        else
        {
            if(retval = rtlsdr_open(&device, verbose_device_search((char *)"0")))
            {
                std::__throw_runtime_error("Failed to open RTLSDR. Error #" + retval);
            }
        }

        retval = rtlsdr_set_bias_tee(device, 1);
        if (retval)
        {
            std::__throw_runtime_error("Failed to run rtlsdr_set_bias_tee()");
        }

        retval = rtlsdr_reset_buffer(device);
        if (retval)
        {
            std::__throw_runtime_error("Failed to run rtlsdr_reset_buffer()");
        }

        retval = rtlsdr_set_center_freq(device, (uint32_t)cf);
        if (retval)
        {
            std::__throw_runtime_error("Failed to run rtlsdr_set_center_freq()");
        }

        retval = rtlsdr_set_sample_rate(device, (uint32_t)sampling_rate);
        if (retval)
        {
            std::__throw_runtime_error("Failed to run rtlsdr_set_sample_rate()");
        }

        retval = rtlsdr_set_tuner_bandwidth(device, (uint32_t)(sampling_rate / 2));
        if (retval)
        {
            std::__throw_runtime_error("Failed to run rtlsdr_set_tuner_bandwidth()");
        }

        retval = rtlsdr_set_tuner_gain_mode(device, automatic_gain?0:1);
        if (retval)
        {
            std::__throw_runtime_error("Failed to run rtlsdr_set_tuner_gain_mode()");
        }

        if(false == automatic_gain)
        {
            retval = rtlsdr_set_tuner_gain(device, gain);
            if (retval)
            {
                std::__throw_runtime_error("Failed to run rtlsdr_set_tuner_gain()");
            }
        }
    }

    RtlSdrSource::~RtlSdrSource()
    {
        (void)rtlsdr_close(device);
    }

    void RtlSdrSource::registerQueue(std::queue<RfChunk> *q)
    {
        iq_queue = q;
    }

    void RtlSdrSource::start()
    {
        dongle_th = std::async([=]() {
            int error = rtlsdr_read_async(device, rtlsdr_callback, static_cast<void *>(this), 0, 0);
            if(error != 0)
            {
                printf("Error code %d\n", error);
                std::__throw_runtime_error("Failed to start receiving with rtlsdr_read_async()");
            }
        });
    }

    void RtlSdrSource::stop()
    {
        rtlsdr_cancel_async(device);
        dongle_th.wait();
    }

    void RtlSdrSource::dataHandler(int8_t *buffer, size_t valid_length)
    {
        RfChunk chunk;
        Error err;
        err = deinterleave(buffer, valid_length, chunk.I, chunk.Q);
        if(err != SUCCESS)
        {
            return;
        }
        iq_queue->push(chunk);
    }

    RfSource *createSource(std::list<ConfigEntry> &configs)
    {
        auto source_type = std::find(configs.begin(), configs.end(), ConfigEntry("source_type"));
        if ("file" == source_type->getString())
        {
            std::string filename = std::find(configs.begin(), configs.end(), ConfigEntry("file"))->getString();
            int64_t interval = std::find(configs.begin(), configs.end(), ConfigEntry("interval"))->getInt();
            size_t portion_size = std::find(configs.begin(), configs.end(), ConfigEntry("portion_size"))->getInt();
            return new FileSource(filename, portion_size, interval);
        }
        else if("hackrf" == source_type->getString())
        {
            double fs = std::find(configs.begin(), configs.end(), ConfigEntry("sampling_rate"))->getFloat();
            double f = std::find(configs.begin(), configs.end(), ConfigEntry("frequency"))->getFloat();
            uint8_t lna = (uint8_t)std::find(configs.begin(), configs.end(), ConfigEntry("lna_gain"))->getInt();
            uint8_t vga = (uint8_t)std::find(configs.begin(), configs.end(), ConfigEntry("vga_gain"))->getInt();
            return new HackrfSource(f, fs, lna, vga);
        }
        else if("rtlsdr" == source_type->getString())
        {
            double f = std::find(configs.begin(), configs.end(), ConfigEntry("frequency"))->getFloat();
            double fs = std::find(configs.begin(), configs.end(), ConfigEntry("sampling_rate"))->getFloat();
            return new RtlSdrSource((uint32_t) f, (uint32_t)fs);
        }
        else
        {
            std::__throw_runtime_error("Unknown RF source type");
        }
    }
}

extern "C"
{
    static int rx_callback(hackrf_transfer* transfer)
    {
        DSP::HackrfSource *hckrfsrc = (DSP::HackrfSource *)transfer->rx_ctx;
        hckrfsrc->dataHandler((int8_t *)transfer->buffer, transfer->valid_length);
        return 0;
    }

    static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
    {
        int8_t *signed_buf = (int8_t *)buf;
        DSP::RtlSdrSource *rtlsdrsrc = (DSP::RtlSdrSource *)ctx;
        for(uint32_t ii = 0; ii < len; ii++)
        {
            signed_buf[ii] = (int8_t)((int)buf[ii] - 128);
        }
        rtlsdrsrc->dataHandler((int8_t *)buf, len);
        return;
    }
}
