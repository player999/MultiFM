
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

#define DEBUG_TRACE_DEMOD (1)
#define DEBUG_TRACE_CHUNK (1)

class TracerTong
{
    public:
        TracerTong()
        {
            clocks.push_back(std::tuple<clock_t, string, uint32_t>(clock(), "Constructor", 0));
        }

        void tick(string file, uint32_t linum)
        {
            tuple<clock_t, string, uint32_t> old = *(clocks.end() - 1);
            clock_t old_clock = get<0>(old);
            clock_t clk = clock();
            clocks.push_back(std::tuple<clock_t, string, uint32_t>(clk, file, linum));
            cout << file << ":" << linum << "\t" << "Spent " << clk - old_clock << " ticks." << endl;
        }

        void print()
        {
            for(auto it = clocks.begin() + 1; it != clocks.end(); it++)
            {
                cout << get<1>(*it) << ":" << get<2>(*it) << "\t" << "Spent " << get<0>(*it) - get<0>(*(it - 1)) << " ticks." << endl;
            }
            clocks.clear();
            clocks.push_back(std::tuple<clock_t, string, uint32_t>(clock(), "Constructor", 0));
        }
    private:
        vector<std::tuple<clock_t, string, uint32_t>> clocks;
};

template <class T> class FmReceiver
{
    public:
        FmReceiver(string mp3_suffix, double fs, double cf, double f)
        {
            relative_freq = f - cf;
            if(abs(relative_freq) > (fs/2))
            {
                __throw_runtime_error("Frequency is invalid");
            }
            this->fs = fs;
            decimator1 = (uint32_t)(fs / 400e3); //200e3 BW
            decimator2 = (fs/decimator1)/44100;

            encoder = new AudioEncoder(mp3_suffix + to_string((size_t)f) + ".mp3");
            lowpass = new LpFirFilter<T>(fs, 100e3, 96);
            lowpass_audio = new LpFirFilter<T>((fs/decimator1), 44100/2, 96);
            mixer = new Mixer<T>(fs, f, 1000000);

            mixed_i.reserve(1000000);
            mixed_q.reserve(1000000);
            filter1_i.reserve(1000000);
            filter1_q.reserve(1000000);
            omega.reserve(1000000);
            omega_diff.reserve(1000000);
            demodulated.reserve(1000000);
        }

        ~FmReceiver()
        {
            delete encoder;
            delete lowpass;
            delete lowpass_audio;
            delete mixer;
        }

        Error processChunk(const vector<T> *buffer_i, const vector<T> *buffer_q)
        {
            Error err;

            /* [Im,Qm] = mixer_ml(Fs, station, I, Q); */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            mixed_i.resize(buffer_i->size());
            mixed_q.resize(buffer_q->size());
            //err = mix(fs, relative_freq, buffer_i->data(), buffer_q->data(), mixed_i.data(), mixed_q.data(), buffer_i->size());
            err = mixer->mix(buffer_i->data(), buffer_q->data(), mixed_i.data(), mixed_q.data(), buffer_i->size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* [If,Qf] = filter_ml('lowpass', 256, Fs, 100e3, decimator, Im, Qm); */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            filter1_i.resize(mixed_i.size() / decimator1);
            filter1_q.resize(mixed_q.size() / decimator1);
            err = lowpass->executeCpxDecim(mixed_i.data(), mixed_q.data(), filter1_i.data(), filter1_q.data(), decimator1, mixed_i.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* omega = atan_ml(If,Qf); */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            omega.resize(filter1_i.size());
            err = ataniq(filter1_i.data(), filter1_q.data(), omega.data(), filter1_i.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* y = diff_ml(omega); */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            omega_diff.resize(omega.size());
            err = diffangle(omega.data(), omega_diff.data(), omega.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* [z1,~] = filter_ml('lowpass', 256, Fs/decimator, AFs/2, (Fs/decimator)/AFs, y, y); */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            demodulated.resize(omega_diff.size() / decimator2);
            err = lowpass_audio->executeRealDecim(omega_diff.data(), demodulated.data(), decimator2, omega_diff.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* z1 = z1 .* 6000; */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            for(size_t ii = 0; ii < demodulated.size(); ii++)
            {
                demodulated[ii] *= 6000;
            }

            /* audio_encoder_ml('shit.mp3', z1); */
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
            err = encoder->encode(demodulated.data(), demodulated.size());
            if(err != SUCCESS)
            {
                return err;
            }
#if DEBUG_TRACE_DEMOD == 1
            if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif

            return SUCCESS;
        }

#if DEBUG_TRACE_DEMOD == 1
        TracerTong tracer;
#endif
    private:
        double fs;
        double relative_freq;
        AudioEncoder *encoder;
        LpFirFilter<T> *lowpass;
        LpFirFilter<T> *lowpass_audio;
        Mixer<T> *mixer;
        uint32_t decimator1;
        uint32_t decimator2;

        std::vector<T> deinter_i;
        std::vector<T> deinter_q;
        std::vector<T> mixed_i;
        std::vector<T> mixed_q;
        std::vector<T> filter1_i;
        std::vector<T> filter1_q;
        std::vector<T> omega;
        std::vector<T> omega_diff;
        std::vector<T> demodulated;
};

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

    err = findStations(deinter_i.data(), deinter_q.data(), analyze_length, fs, cf, found_freqs);
    return err;
}


Error process_from_file(string fname, string mp3_prefix, double fs, double cf, vector<double> f)
{
    #define SAMPLE_COUNT (200000L)
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
    list<ConfigEntry> configuration;
    int64_t interval = (1000000000L * SAMPLE_COUNT) / ((int64_t)fs);
    configuration.push_back(ConfigEntry("source_type", "file"));
    configuration.push_back(ConfigEntry("file", fname));
    configuration.push_back(ConfigEntry("interval", interval));
    configuration.push_back(ConfigEntry("portion_size", (int64_t)SAMPLE_COUNT));
    RfSource *fsrc = createSource(configuration);
    queue<RfChunk> data_queue;
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
    }

    receivers.resize(f.size());
    for(uint8_t ii; ii < f.size(); ii++)
    {
        receivers[ii] = new FmReceiver<double>(mp3_prefix, fs, cf, f[ii]);
    }

    size_t empty_cycles = 0;
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

    po::options_description desc("MultiFM console app options");
    desc.add_options()
        ("help,h", "produce help message")
        ("data,d", po::value<string>(), "8-bit data")
        ("out,o", po::value<string>()->default_value("multifm_"), "prefix for mp3 files")
        ("fs", po::value<double>()->default_value(20e6), "sampling rate")
        ("cf", po::value<double>()->default_value(100e6), "center frequency")
        ("frequencies,f", po::value<vector<double>>(), "frequency list")
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

    if (vm.count("data"))
    {
        filename = vm["data"].as<string>();
    }
    else
    {
        cout << "Did not get input file filename\n";
        exit(EINVAL);
    }

    fs = vm["fs"].as<double>();

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

    //process_data<double>(filename, mp3_prefix, fs, cf, f);
    process_from_file(filename, mp3_prefix, fs, cf, f);

    return 0;
}