#include "freesrp_source_c.h"

using namespace FreeSRP;
using namespace std;

freesrp_source_c_sptr make_freesrp_source_c (const std::string &args)
{
    return gnuradio::get_initial_sptr(new freesrp_source_c (args));
}

freesrp_source_c::freesrp_source_c (const std::string & args) : freesrp_common(args)
{
    if(_srp == nullptr)
    {
        throw runtime_error("FreeSRP not initialized!");
    }
}

bool freesrp_source_c::start()
{
    // TODO: source start
    return false;
}

bool freesrp_source_c::stop()
{
    // TODO: source stop
    return false;
}

int freesrp_source_c::work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items)
{
    // TODO: source work
    return 0;
}

double freesrp_source_c::set_sample_rate( double rate )
{
    response r = _srp->send_cmd({SET_RX_SAMP_FREQ, rate});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set RX sample rate, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}

double freesrp_source_c::get_sample_rate( void )
{
    response r = _srp->send_cmd({GET_RX_SAMP_FREQ, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get RX sample rate, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}

double freesrp_source_c::set_center_freq( double freq, size_t chan )
{
    response r = _srp->send_cmd({SET_RX_SAMP_FREQ, freq / 1.0e6});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set RX LO frequency, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param * 1.0e6;
    }
}

double freesrp_source_c::get_center_freq( size_t chan )
{
    response r = _srp->send_cmd({GET_RX_SAMP_FREQ, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get RX LO frequency, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param * 1.0e6;
    }
}

std::vector<std::string> freesrp_source_c::get_gain_names( size_t chan )
{
    std::vector<std::string> names;

    names.push_back("RX_RF");

    return names;
}

osmosdr::gain_range_t freesrp_source_c::get_gain_range(size_t chan)
{
    osmosdr::meta_range_t gain_ranges;

    gain_ranges.push_back(osmosdr::range_t(0, 74, 1));

    return gain_ranges;
}

osmosdr::gain_range_t freesrp_source_c::get_gain_range(const std::string& name, size_t chan)
{
    return get_gain_range(chan);
}

double freesrp_source_c::set_gain(double gain, size_t chan)
{
    response r = _srp->send_cmd({SET_RX_RF_GAIN, gain * 1000});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set RX RF gain, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param / 1000;
    }
}

double freesrp_source_c::set_gain(double gain, const std::string& name, size_t chan)
{
    return set_gain(gain, chan);
}

double freesrp_source_c::get_gain(size_t chan)
{
    response r = _srp->send_cmd({GET_RX_RF_GAIN, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get RX RF gain, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param / 1000;
    }
}

double freesrp_source_c::get_gain(const std::string& name, size_t chan)
{
    return get_gain(chan);
}

double freesrp_source_c::set_bb_gain(double gain, size_t chan)
{
    return set_gain(gain, chan);
}

std::vector<std::string> freesrp_source_c::get_antennas(size_t chan)
{
    std::vector<std::string> antennas;

    antennas.push_back(get_antenna(chan));

    return antennas;
}

std::string freesrp_source_c::set_antenna(const std::string& antenna, size_t chan)
{
    return get_antenna(chan);
}

std::string freesrp_source_c::get_antenna(size_t chan)
{
    return "RX";
}

double freesrp_source_c::set_bandwidth(double bandwidth, size_t chan)
{
    response r = _srp->send_cmd({SET_RX_RF_BANDWIDTH, bandwidth});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set RX RF bandwidth, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}

double freesrp_source_c::get_bandwidth(size_t chan)
{
    response r = _srp->send_cmd({GET_RX_RF_BANDWIDTH, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get RX RF bandwidth, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}