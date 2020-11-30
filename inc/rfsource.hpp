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
#ifndef _RFSOURCE_HPP_
#define _RFSOURCE_HPP_

#include <string>
#include <queue>
#include <vector>
#include <list>
#include <cstdint>
#include <thread>
#include <libhackrf/hackrf.h>
#include <rtl-sdr.h>
#include <future>

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

    class RtlSdrSource: public RfSource
    {
        public:
            RtlSdrSource(uint32_t cf, uint32_t fs);
            RtlSdrSource(uint32_t cf, uint32_t sampling_rate, bool automatic_gain, uint8_t gain, const std::string &serial);
            ~RtlSdrSource();
            void registerQueue(std::queue<RfChunk> *q);
            void start();
            void stop();
            void dataHandler(int8_t *buffer, size_t valid_length);
        private:
            std::queue<RfChunk> *iq_queue;
            rtlsdr_dev_t *device = NULL;
            std::future<void> dongle_th;
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