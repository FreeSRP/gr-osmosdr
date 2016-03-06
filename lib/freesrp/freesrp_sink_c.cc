#include "freesrp_sink_c.h"

using namespace FreeSRP;
using namespace std;

freesrp_sink_c_sptr make_freesrp_sink_c (const std::string &args)
{
    return gnuradio::get_initial_sptr(new freesrp_sink_c (args));
}

/*
 * Specify constraints on number of input and output streams.
 * This info is used to construct the input and output signatures
 * (2nd & 3rd args to gr_block's constructor).  The input and
 * output signatures are used by the runtime system to
 * check that a valid number and type of inputs and outputs
 * are connected to this block.  In this case, we accept
 * only 0 input and 1 output.
 */
static const int MIN_IN = 1;   // mininum number of input streams
static const int MAX_IN = 1;   // maximum number of input streams
static const int MIN_OUT = 0;  // minimum number of output streams
static const int MAX_OUT = 0;  // maximum number of output streams

freesrp_sink_c::freesrp_sink_c (const std::string & args) : gr::sync_block("freesrp_sink_c",
                                                            gr::io_signature::make (MIN_IN, MAX_IN, sizeof (gr_complex)),
                                                            gr::io_signature::make (MIN_OUT, MAX_OUT, sizeof (gr_complex))),
                                                            freesrp_common(args)
{
    if(_srp == nullptr)
    {
        throw runtime_error("FreeSRP not initialized!");
    }
}

bool freesrp_sink_c::start()
{
    _srp->start_tx();
    return true;
}

bool freesrp_sink_c::stop()
{
    _srp->stop_tx();
    return true;
}

int freesrp_sink_c::work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];

    int consumed = 0;

    for(int i = 0; i < noutput_items; i++)
    {
        FreeSRP::sample s;
        s.i = real(in[i]);
        s.q = imag(in[i]);
        while(!_srp->submit_tx_sample(s)) { /* Wait until the sample can be submitted */ }
        consumed++;
    }

    consume_each(consumed);

    return 0;
}

double freesrp_sink_c::set_sample_rate( double rate )
{
    std::cerr << "This is under development. Did not set the sample rate. " << endl;
    return 0;

    /*
    response r = _srp->send_cmd({SET_TX_SAMP_FREQ, rate});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set TX sample rate, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
    */
}

double freesrp_sink_c::get_sample_rate( void )
{
    response r = _srp->send_cmd({GET_TX_SAMP_FREQ, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get TX sample rate, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}

double freesrp_sink_c::set_center_freq( double freq, size_t chan )
{
    std::cerr << "This is under development. Did not set the center frequency. " << endl;
    return 0;

    /*
    response r = _srp->send_cmd({SET_TX_LO_FREQ, freq});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set TX LO frequency, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param * 1.0e6;
    }
     */
}

double freesrp_sink_c::get_center_freq( size_t chan )
{
    response r = _srp->send_cmd({GET_TX_LO_FREQ, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get TX LO frequency, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}

std::vector<std::string> freesrp_sink_c::get_gain_names( size_t chan )
{
    std::vector<std::string> names;

    names.push_back("TX_RF");

    return names;
}

osmosdr::gain_range_t freesrp_sink_c::get_gain_range(size_t chan)
{
    osmosdr::meta_range_t gain_ranges;

    gain_ranges.push_back(osmosdr::range_t(-89.75, 0, 0.25));

    return gain_ranges;
}

osmosdr::gain_range_t freesrp_sink_c::get_gain_range(const std::string& name, size_t chan)
{
    return get_gain_range(chan);
}

double freesrp_sink_c::set_gain(double gain, size_t chan)
{
    std::cerr << "This is under development. Did not set gain. " << endl;
    return 0;

    /*
    response r = _srp->send_cmd({SET_TX_ATTENUATION, -gain * 1000});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set TX RF attenuation, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return -r.param / 1000;
    }
    */
}

double freesrp_sink_c::set_gain(double gain, const std::string& name, size_t chan)
{
    return set_gain(gain, chan);
}

double freesrp_sink_c::get_gain(size_t chan)
{
    response r = _srp->send_cmd({GET_TX_ATTENUATION, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get TX RF attenuation, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return -r.param / 1000;
    }
}

double freesrp_sink_c::get_gain(const std::string& name, size_t chan)
{
    return get_gain(chan);
}

double freesrp_sink_c::set_bb_gain(double gain, size_t chan)
{
    return set_gain(gain, chan);
}

std::vector<std::string> freesrp_sink_c::get_antennas(size_t chan)
{
    std::vector<std::string> antennas;

    antennas.push_back(get_antenna(chan));

    return antennas;
}

std::string freesrp_sink_c::set_antenna(const std::string& antenna, size_t chan)
{
    return get_antenna(chan);
}

std::string freesrp_sink_c::get_antenna(size_t chan)
{
    return "TX";
}

double freesrp_sink_c::set_bandwidth(double bandwidth, size_t chan)
{
    std::cerr << "This is under development. Did not set the bandwidth. " << endl;
    return 0;

    /*
    response r = _srp->send_cmd({SET_TX_RF_BANDWIDTH, bandwidth});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not set TX RF bandwidth, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
     */
}

double freesrp_sink_c::get_bandwidth(size_t chan)
{
    response r = _srp->send_cmd({GET_TX_RF_BANDWIDTH, 0});
    if(r.error != CMD_OK)
    {
        std::cerr << "Could not get TX RF bandwidth, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}
