#ifndef _RFSOURCE_HPP_
#define _RFSOURCE_HPP_

#include <string>
#include <queue>
#include <vector>
#include <list>
#include <cstdint>
#include <thread>
#include <libhackrf/hackrf.h>

namespace DSP
{
    struct RfChunk
    {
        std::vector<double> I;
        std::vector<double> Q;
    };

    class RfSource
    {
        public:
            virtual void registerQueue(std::queue<RfChunk> *queue) = 0;
            virtual void start() = 0;
            virtual void stop() = 0;
    };

    class FileSource: public RfSource
    {
        public:
            FileSource(std::string &fname, size_t portion, uint64_t interval_nsec);
            ~FileSource();
            void registerQueue(std::queue<RfChunk> *q);
            void start();
            void stop();
            static void dataHandler(FileSource *fs);
        private:
            bool stopsignal;
            size_t portion_size;
            uint64_t interval;
            std::queue<RfChunk> *iq_queue;
            FILE *fh;
            std::thread *read_thread;
    };

    class HackrfSource: public RfSource
    {
        public:
            HackrfSource(double cf, double fs, uint8_t lna, uint8_t vga);
            HackrfSource(double cf, double fs, uint8_t lna, uint8_t vga, std::string &serial);
            ~HackrfSource();
            void registerQueue(std::queue<RfChunk> *q);
            void start();
            void stop();
            void dataHandler(int8_t *buffer, size_t valid_length);
        private:
            void commonConstructor(double cf, double fs, uint8_t lna, uint8_t vga);
            std::queue<RfChunk> *iq_queue;
            hackrf_device* device = NULL;
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
            ConfigEntry(const std::string &n, const std::string &v);
            ConfigEntry(const std::string &n, double v);
            ConfigEntry(const std::string &n, int64_t i);
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