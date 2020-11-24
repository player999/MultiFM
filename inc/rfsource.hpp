#ifndef _RFSOURCE_HPP_
#define _RFSOURCE_HPP_

#include <string>
#include <queue>
#include <vector>
#include <list>
#include <cstdint>

namespace DSP
{
    struct RfChunk
    {
        std::vector<double> I;
        std::vector<double> Q;
    };

    class RfSource
    {
        RfSource();
        virtual void registerQueue(std::queue<RfChunk> *queue) = 0;
    };

    class FileSource: public RfSource
    {
        public:
            FileSource(std::string &fname, size_t portion, uint64_t interval_nsec);
            ~FileSource();
            void registerQueue(std::queue<RfChunk> *q);

        private:
            std::queue<RfChunk> *queue;
            FILE *fh;
            size_t portion_size;
            uint64_t interval;

    };

    enum ConfigType
    {
        STRING,
        FLOAT,
        INTEGER,
        NONE
    };

    class ConfigEntry
    {
        public:
            ConfigEntry();
            ConfigEntry(const std::string &n);
            ~ConfigEntry();
            std::string &getName();
            ConfigType getType();
            std::string &getString();
            double getFloat();
            int64_t getInt();

            std::string name;
            ConfigType type;
            std::string s;
            double d;
            int64_t i;

        friend bool operator== (const ConfigEntry &c1, const ConfigEntry &c2);
    };

    RfSource *createSource(std::list<ConfigEntry> &configs);
}

#endif /*_RFSOURCE_HPP_*/