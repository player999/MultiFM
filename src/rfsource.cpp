#include "rfsource.hpp"
#include <algorithm>
#include <exception>
#include <cstdio>

using namespace DSP;

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

