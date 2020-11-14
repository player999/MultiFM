
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <exception>
#include <future>
#include <iostream>
#include <cutedsp.hpp>

using namespace DSP;
using namespace std;
namespace po = boost::program_options;

bool is_parallel = true;

class FmReceiver
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
            lowpass = new LpFirFilter<double>(fs, 100e3, 96);
            lowpass_audio = new LpFirFilter<double>((fs/decimator1), 44100/2, 96);

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
        }

        Error processChunk(const vector<double> *buffer_i, const vector<double> *buffer_q)
        {
            Error err;

            /* [Im,Qm] = mixer_ml(Fs, station, I, Q); */
            mixed_i.resize(buffer_i->size());
            mixed_q.resize(buffer_q->size());
            err = mix(fs, relative_freq, buffer_i->data(), buffer_q->data(), mixed_i.data(), mixed_q.data(), buffer_i->size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* [If,Qf] = filter_ml('lowpass', 256, Fs, 100e3, decimator, Im, Qm); */
            filter1_i.resize(mixed_i.size() / decimator1);
            filter1_q.resize(mixed_q.size() / decimator1);
            err = lowpass->executeCpxDecim(mixed_i.data(), mixed_q.data(), filter1_i.data(), filter1_q.data(), decimator1, mixed_i.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* omega = atan_ml(If,Qf); */
            omega.resize(filter1_i.size());
            err = ataniq(filter1_i.data(), filter1_q.data(), omega.data(), filter1_i.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* y = diff_ml(omega); */
            omega_diff.resize(omega.size());
            err = diffangle(omega.data(), omega_diff.data(), omega.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* [z1,~] = filter_ml('lowpass', 256, Fs/decimator, AFs/2, (Fs/decimator)/AFs, y, y); */
            demodulated.resize(omega_diff.size() / decimator2);
            err = lowpass_audio->executeRealDecim(omega_diff.data(), demodulated.data(), decimator2, omega_diff.size());
            if(err != SUCCESS)
            {
                return err;
            }

            /* z1 = z1 .* 6000; */
            for(size_t ii = 0; ii < demodulated.size(); ii++)
            {
                demodulated[ii] *= 6000;
            }

            /* audio_encoder_ml('shit.mp3', z1); */
            err = encoder->encode(demodulated.data(), demodulated.size());
            if(err != SUCCESS)
            {
                return err;
            }

            return SUCCESS;
        }

    private:
        double fs;
        double relative_freq;
        AudioEncoder *encoder;
        LpFirFilter<double> *lowpass;
        LpFirFilter<double> *lowpass_audio;
        uint32_t decimator1;
        uint32_t decimator2;

        std::vector<double> deinter_i;
        std::vector<double> deinter_q;
        std::vector<double> mixed_i;
        std::vector<double> mixed_q;
        std::vector<double> filter1_i;
        std::vector<double> filter1_q;
        std::vector<double> omega;
        std::vector<double> omega_diff;
        std::vector<double> demodulated;
};

Error process_chunk_single_freq(FmReceiver *receivers, vector<double> *i, vector<double> *q)
{
    return receivers->processChunk(i, q);
}

Error process_data(string fname, string mp3_prefix, double fs, double cf, vector<double> f)
{
    #define SAMPLE_COUNT (200000)
    FILE *fh = NULL;
    size_t samples_a_time = SAMPLE_COUNT * 2;
    int8_t buffer[SAMPLE_COUNT * 2];
    size_t byte_count;
    Error  err;
    vector<double> deinter_i;
    vector<double> deinter_q;
    FmReceiver *receivers[f.size()];

    deinter_i.resize(SAMPLE_COUNT);
    deinter_q.resize(SAMPLE_COUNT);

    fh = fopen(fname.c_str(), "rb");
    if(NULL == fh)
    {
        return FAIL;
    }

    for(uint8_t ii; ii < f.size(); ii++)
    {
        receivers[ii] = new FmReceiver(mp3_prefix, fs, cf, f[ii]);
    }

    while(true)
    {
        byte_count = fread(buffer, 1, samples_a_time, fh);
        if(0 == byte_count) goto L_error;

        /*  Q = mat(2:2:end);
            I = mat(1:2:end); */
        err = deinterleave(buffer, byte_count, deinter_i, deinter_q);
        if(err != SUCCESS)
        {
            goto L_error;
        }

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
                fut[ii] = async(process_chunk_single_freq, receivers[ii], &deinter_i, &deinter_q);
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
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
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
        cout << "Did not set frequency list\n";
        exit(EINVAL);
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

    process_data(filename, mp3_prefix, fs, cf, f);

    return 0;
}