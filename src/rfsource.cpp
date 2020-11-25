#include "rfsource.hpp"
#include "interleaving.hpp"
#include "error.hpp"
#include <algorithm>
#include <exception>
#include <cstdio>
#include <chrono>

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

