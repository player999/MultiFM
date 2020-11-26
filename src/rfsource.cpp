#include "rfsource.hpp"
#include "interleaving.hpp"
#include "error.hpp"
#include <algorithm>
#include <exception>
#include <string>
#include <cstdio>
#include <chrono>

extern "C"
{
    static int rx_callback(hackrf_transfer* transfer);
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
}
