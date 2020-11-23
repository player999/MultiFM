
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>
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

static Error station_searching(FILE *fh, double fs, double cf, vector<double> &found_freqs)
{
    Error err;
    const uint32_t analyze_length = 1024 * 1024;
    vector<int8_t> buffer;
    vector<double> deinter_i;
    vector<double> deinter_q;

    buffer.resize(analyze_length * 2);
    deinter_i.resize(analyze_length);
    deinter_q.resize(analyze_length);

    (void)fread(buffer.data(), 1, analyze_length * 2, fh);

    err = deinterleave(buffer.data(), analyze_length * 2, deinter_i.data(), deinter_q.data());
    if(err != SUCCESS)
    {
        return err;
    }
    err = findStations(deinter_i.data(), deinter_q.data(), analyze_length, fs, cf, found_freqs);
    return err;
}

template <class T> Error process_data(string fname, string mp3_prefix, double fs, double cf, vector<double> f)
{
    #define SAMPLE_COUNT (200000)
    FILE *fh = NULL;
    size_t samples_a_time = SAMPLE_COUNT * 2;
    int8_t buffer[SAMPLE_COUNT * 2];
    size_t byte_count;
    Error  err;
#if DEBUG_TRACE_CHUNK == 1
    TracerTong tracer;
#endif
    vector<T> deinter_i;
    vector<T> deinter_q;
    vector<FmReceiver<T> *> receivers;

    deinter_i.resize(SAMPLE_COUNT);
    deinter_q.resize(SAMPLE_COUNT);

    fh = fopen(fname.c_str(), "rb");
    if(NULL == fh)
    {
        return FAIL;
    }

    if(f.size() == 0)
    {
        err = station_searching(fh, fs, cf, f);
        if(err != SUCCESS)
        {
            fclose(fh);
            return err;
        }
    }

    receivers.resize(f.size());

    for(uint8_t ii; ii < f.size(); ii++)
    {
        receivers[ii] = new FmReceiver<T>(mp3_prefix, fs, cf, f[ii]);
    }

    while(true)
    {
#if DEBUG_TRACE_CHUNK == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        byte_count = fread(buffer, 1, samples_a_time, fh);
        if(0 == byte_count) goto L_error;

        /*  Q = mat(2:2:end);
            I = mat(1:2:end); */

#if DEBUG_TRACE_CHUNK == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        err = deinterleave(buffer, byte_count, deinter_i, deinter_q);
        if(err != SUCCESS)
        {
            goto L_error;
        }

#if DEBUG_TRACE_CHUNK == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        std::future<Error> fut[f.size()];
        for(uint8_t ii = 0; ii < f.size(); ii++)
        {
            if(false == is_parallel)
            {
                err = receivers[ii]->processChunk(&deinter_i, &deinter_q);
                if(err != SUCCESS)
                {
                    goto L_error;
                }
            }
            else
            {
                fut[ii] = async(process_chunk_single_freq<T>, receivers[ii], &deinter_i, &deinter_q);
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
#if DEBUG_TRACE_DEMOD == 1
                // receivers[ii]->tracer.print();
#endif
            }
            err = accumulated_error;
        }
#if DEBUG_TRACE_CHUNK == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
    }
L_error:
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

    process_data<double>(filename, mp3_prefix, fs, cf, f);

    return 0;
}