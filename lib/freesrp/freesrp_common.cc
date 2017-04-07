#include "freesrp_common.h"

#include <cstdlib>

#include <boost/make_shared.hpp>

#include <arg_helpers.h>

using namespace FreeSRP;
using namespace std;

boost::shared_ptr<FreeSRP::FreeSRP> freesrp_common::_srp;

freesrp_common::freesrp_common(const std::string &args)
{
    dict_t dict = params_to_dict(args);

    if(!_srp)
    {
        try
        {
            if(dict.count("fx3"))
            {
                if(Util::find_fx3())
                {
                    // Upload firmware to FX3
                    string firmware_path = string(getenv("HOME")) + "/.freesrp/fx3.img";
                    if(dict["fx3"].length() > 0)
                    {
                        firmware_path = dict["fx3"];
                    }
                    Util::find_fx3(true, firmware_path);
                    cout << "FX3 programmed with '" << firmware_path << "'" << endl;
                    // Give FX3 time to re-enumerate
                    this_thread::sleep_for(chrono::milliseconds(600));
                }
                else
                {
                    cout << "No FX3 in bootloader mode found" << endl;
                }
            }

            _srp.reset(new FreeSRP::FreeSRP());

            if(dict.count("fpga") || !_srp->fpga_loaded())
            {
                string bitstream_path = string(getenv("HOME")) + "/.freesrp/fpga.bin";
                if(dict["fpga"].length() > 0)
                {
                    bitstream_path = dict["fpga"];
                }
                fpga_status stat = _srp->load_fpga(bitstream_path);
                switch(stat)
                {
                case FPGA_CONFIG_ERROR:
                    throw runtime_error("Could not load FPGA configuration!");
                case FPGA_CONFIG_SKIPPED:
                    cout << "FPGA already configured. Restart the FreeSRP to load a new bitstream." << endl;
                    break;
                case FPGA_CONFIG_DONE:
                    cout << "FPGA configured with '" << bitstream_path << "'" << endl;
                    break;
                }
            }

            cout << "Connected to FreeSRP" << endl;

            if(dict.count("loopback"))
            {
                response res = _srp->send_cmd({SET_LOOPBACK_EN, 1});
                if(res.error == CMD_OK)
                {
                    cout << "AD9364 in loopback mode" << endl;
                }
                else
                {
                    throw runtime_error("Could not put AD9364 into loopback mode!");
                }
            }
            else
            {
                response res = _srp->send_cmd({SET_LOOPBACK_EN, 0});
                if(res.error != CMD_OK)
                {
                    throw runtime_error("Error disabling AD9364 loopback mode!");
                }
            }
        }
        catch(const runtime_error& e)
        {
            cerr << "FreeSRP Error: " << e.what() << endl;
            throw runtime_error(e.what());
        }
    }
}

std::vector<std::string> freesrp_common::get_devices()
{
    std::vector<std::string> devices;

    try
    {
        FreeSRP::FreeSRP srp;

        string str;
        str = "freesrp=0,label='FreeSRP'";

        devices.push_back(str);
    }
    catch(const ConnectionError &err)
    {
        // No FreeSRP found.
    }

    return devices;
}

size_t freesrp_common::get_num_channels( void )
{
    return 1;
}

osmosdr::meta_range_t freesrp_common::get_sample_rates( void )
{
    osmosdr::meta_range_t sample_rates;
    sample_rates.push_back(osmosdr::range_t(1e6, 61.44e6, 1));
    sample_rates.push_back(osmosdr::range_t(1e6));
    sample_rates.push_back(osmosdr::range_t(8e6));
    sample_rates.push_back(osmosdr::range_t(16e6));
    sample_rates.push_back(osmosdr::range_t(20e6));
    sample_rates.push_back(osmosdr::range_t(40e6));
    sample_rates.push_back(osmosdr::range_t(50e6));
    sample_rates.push_back(osmosdr::range_t(61.44e6));
    
    return sample_rates;
}

osmosdr::freq_range_t freesrp_common::get_freq_range(size_t chan)
{
    osmosdr::meta_range_t freq_ranges;

    freq_ranges.push_back(osmosdr::range_t(7e7, 6e9, 2.4));

    return freq_ranges;
}


osmosdr::freq_range_t freesrp_common::get_bandwidth_range(size_t chan)
{
    //TODO: Verify bandwidth ranges

    osmosdr::meta_range_t sample_rates;
    sample_rates.push_back(osmosdr::range_t(2e5, 56e6, 1));
    sample_rates.push_back(osmosdr::range_t(2e5));
    sample_rates.push_back(osmosdr::range_t(1e6));
    sample_rates.push_back(osmosdr::range_t(8e6));
    sample_rates.push_back(osmosdr::range_t(16e6));
    sample_rates.push_back(osmosdr::range_t(20e6));
    sample_rates.push_back(osmosdr::range_t(40e6));
    sample_rates.push_back(osmosdr::range_t(50e6));
    sample_rates.push_back(osmosdr::range_t(56e6));

    return sample_rates;
}


double freesrp_common::set_freq_corr( double ppm, size_t chan )
{
    // TODO: Set DCXO tuning
    return 0;
}

double freesrp_common::get_freq_corr( size_t chan )
{
    // TODO: Get DCXO tuning
    return 0;
}