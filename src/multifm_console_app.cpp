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
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <exception>
#include <future>
#include <iostream>
#include <ctime>
#include <cutedsp.hpp>

using namespace DSP;
using namespace std;
namespace po = boost::program_options;

bool is_parallel = true;
bool trace_timer = false;

#define DEBUG_TRACE_CHUNK (1)
#define SAMPLE_COUNT (200000L)

template <class T> Error process_chunk_single_freq(FmReceiver<T> *receivers, vector<T> *i, vector<T> *q)
{
    return receivers->processChunk(i, q);
}

static Error station_searching(queue<RfChunk> &queue, double fs, double cf, vector<double> &found_freqs)
{
    Error err;
    const uint32_t analyze_length = 1024 * 1024;
    vector<double> deinter_i;
    vector<double> deinter_q;

    deinter_i.reserve(analyze_length * 2);
    deinter_q.reserve(analyze_length * 2);

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        while((queue.size() > 0) && (deinter_i.size() < analyze_length))
        {
            RfChunk chunk = queue.front();
            deinter_i.insert(deinter_i.end(), chunk.I.begin(), chunk.I.end());
            deinter_q.insert(deinter_q.end(), chunk.Q.begin(), chunk.Q.end());
            queue.pop();
        }
    } while(deinter_i.size() < analyze_length);

    FmStationsFinder<double> finder((int64_t)fs, (int64_t) cf, analyze_length, 100e3, 1e3, 3, 5.0);
    err = finder.findStations(deinter_i.data(), deinter_q.data(), analyze_length, found_freqs);
    return err;
}


Error process_data(list<ConfigEntry> &configuration, string mp3_prefix, double fs, double cf, vector<double> f)
{
    #define EMPTY_CYCLE_COUNT (200)
    FILE *fh = NULL;
    size_t samples_a_time = SAMPLE_COUNT * 2;
    int8_t buffer[SAMPLE_COUNT * 2];
    size_t byte_count;
    Error  err;
#if DEBUG_TRACE_CHUNK == 1
    TracerTong tracer;
#endif
    vector<FmReceiver<double> *> receivers;
    RfSource *fsrc = createSource(configuration);
    queue<RfChunk> data_queue;
    size_t empty_cycles = 0;
    fsrc->registerQueue(&data_queue);
    fsrc->start();

    if(f.size() == 0)
    {
        err = station_searching(data_queue, fs, cf, f);
        if(err != SUCCESS)
        {
            delete fsrc;
            return err;
        }

        if(f.size() == 0)
        {
            cout << "No stations found. Terminating" << endl;
            goto L_error;
        }
    }

    receivers.resize(f.size());
    for(uint8_t ii; ii < f.size(); ii++)
    {
        receivers[ii] = new FmReceiver<double>(mp3_prefix, fs, cf, f[ii]);
    }

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        empty_cycles++;
        while(data_queue.size() > 0)
        {
            RfChunk chunk = data_queue.front();
            std::future<Error> fut[f.size()];

            empty_cycles = 0;
            for(uint8_t ii = 0; ii < f.size(); ii++)
            {
                if(false == is_parallel)
                {
                    err = receivers[ii]->processChunk(&chunk.I, &chunk.Q);
                    if(err != SUCCESS)
                    {
                        /* Error */
                        break;
                    }
                }
                else
                {
                    fut[ii] = async(process_chunk_single_freq<double>, receivers[ii], &chunk.I, &chunk.Q);
                }
            }

            if(true == is_parallel)
            {
                Error accumulated_error;
                for(uint8_t ii = 0; ii < f.size(); ii++)
                {
                    Error err = fut[ii].get();
                    if(SUCCESS != err)
                    {
                        accumulated_error = err;
                    }
                }
                err = accumulated_error;
            }
            data_queue.pop();
        }
        if(empty_cycles == EMPTY_CYCLE_COUNT) break;
    }

L_error:
    fsrc->stop();
    delete fsrc;
    for(uint8_t ii; ii < f.size(); ii++)
    {
        delete receivers[ii];
    }
    return err;
}

int main(int argc, char *argv[])
{
    string filename;
    double fs;
    double cf;
    vector<double> f;
    string mp3_prefix;
    list<ConfigEntry> configuration;

    po::options_description desc("MultiFM console app options");
    desc.add_options()
        ("help,h", "produce help message")
        ("data_file,d", po::value<string>(), "8-bit data file")
        ("hackrf", "use HackRF")
        ("rtlsdr", "use RTL SDR")
        ("out,o", po::value<string>()->default_value("multifm_"), "prefix for mp3 files")
        ("fs", po::value<double>(), "sampling rate, Hz")
        ("cf", po::value<double>()->default_value(100e6), "center frequency, Hz")
        ("lna", po::value<int>()->default_value(10), "LNA gain, db")
        ("vga", po::value<int>()->default_value(30), "VGA gain, db")
        ("frequencies,f", po::value<vector<double>>(), "frequency list, Hz")
        ("single,s", "single thread")
        ("trace", "output timer trace")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("single"))
    {
        is_parallel = false;
    }

    if (vm.count("trace"))
    {
        trace_timer = true;
    }

    if (vm.count("frequencies"))
    {
        f = vm["frequencies"].as<vector<double>>();
    }
    else
    {
        cout << "Did not set frequency list. Will search for stations\n";
    }

    mp3_prefix = vm["out"].as<string>();
    cf = vm["cf"].as<double>();

    for(vector<double>::iterator it = f.begin(); it != f.end(); it++)
    {
        double cur_freq = abs((*it) - cf);

        if(cur_freq > (fs / 2))
        {
            cout << "Frequency " << *it << "is out of range" << endl;
            exit(EINVAL);
        }
    }

    if (vm.count("data_file"))
    {
        filename = vm["data_file"].as<string>();
        if(vm.count("fs"))
        {
            fs = vm["fs"].as<double>();
        }
        else
        {
            fs = 20000000.0;
        }
        int64_t interval = (1000000000L * SAMPLE_COUNT) / ((int64_t)fs);
        configuration.push_back(ConfigEntry("source_type", "file"));
        configuration.push_back(ConfigEntry("file", filename));
        configuration.push_back(ConfigEntry("interval", interval));
        configuration.push_back(ConfigEntry("portion_size", (int64_t)SAMPLE_COUNT));
    }
    else if(vm.count("hackrf"))
    {
        int lna, vga;
        lna = vm["lna"].as<int>();
        vga = vm["vga"].as<int>();
        if(vm.count("fs"))
        {
            fs = vm["fs"].as<double>();
        }
        else
        {
            fs = 20000000.0;
        }
        configuration.push_back(ConfigEntry("source_type", "hackrf"));
        configuration.push_back(ConfigEntry("lna_gain", (int64_t)lna));
        configuration.push_back(ConfigEntry("vga_gain", (int64_t)vga));
        configuration.push_back(ConfigEntry("sampling_rate", fs));
        configuration.push_back(ConfigEntry("frequency", cf));
    }
    else if(vm.count("rtlsdr"))
    {
        configuration.push_back(ConfigEntry("source_type", "rtlsdr"));
        configuration.push_back(ConfigEntry("frequency", cf));
        if(vm.count("fs"))
        {
            fs = vm["fs"].as<double>();
        }
        else
        {
            fs = 1008000.0;
        }
        configuration.push_back(ConfigEntry("sampling_rate", fs));
    }
    else
    {
        cout << "Did not get input file filename\n";
        exit(EINVAL);
    }


    process_data(configuration, mp3_prefix, fs, cf, f);

    return 0;
}
