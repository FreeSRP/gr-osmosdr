#include "freesrp_sink_c.h"

using namespace FreeSRP;
using namespace std;

freesrp_sink_c_sptr make_freesrp_sink_c (const string &args)
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
 * only 1 input and 0 output.
 */
static const int MIN_IN = 1;   // mininum number of input streams
static const int MAX_IN = 1;   // maximum number of input streams
static const int MIN_OUT = 0;  // minimum number of output streams
static const int MAX_OUT = 0;  // maximum number of output streams

freesrp_sink_c::freesrp_sink_c (const string & args) : gr::sync_block("freesrp_sink_c",
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
    response res = _srp->send_cmd({SET_DATAPATH_EN, 1});
    if(res.error != CMD_OK)
    {
        return false;
    }
    _srp->start_tx(std::bind(&freesrp_sink_c::freesrp_tx_callback, this, std::placeholders::_1));
    return true;
}

bool freesrp_sink_c::stop()
{
    _srp->send_cmd({SET_DATAPATH_EN, 0});
    _srp->stop_tx();
    return true;
}

void freesrp_sink_c::freesrp_tx_callback(vector<sample>& samples)
{
    unique_lock<std::mutex> lk(_buf_mut);

    for(sample &s : samples)
    {
        if(!_buf_queue.try_dequeue(s))
        {
            s.i = 0;
            s.q = 0;
        }
        else
        {
            _buf_available_space++;
        }
    }

    _buf_cond.notify_one();
}

int freesrp_sink_c::work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items)
{
    const gr_complex *in = (const gr_complex *) input_items[0];

    unique_lock<std::mutex> lk(_buf_mut);

    // Wait until enough space is available
    while(_buf_available_space < (unsigned int) noutput_items)
    {
        _buf_cond.wait(lk);
    }

    for(int i = 0; i < noutput_items; ++i)
    {
        sample s;
        s.i = (int16_t) (real(in[i]) * 2047.0f);
        s.q = (int16_t) (imag(in[i]) * 2047.0f);

        if(!_buf_queue.try_enqueue(s))
        {
            throw runtime_error("Failed to add sample to buffer. This should never happen. Available space reported to be " + to_string(_buf_available_space) + " samples, noutput_items=" + to_string(noutput_items) + ", i=" + to_string(i));
        }
        else
        {
            _buf_available_space--;
        }
    }

    return noutput_items;
}

double freesrp_sink_c::set_sample_rate( double rate )
{
    command cmd = _srp->make_command(SET_TX_SAMP_FREQ, rate);
    response r = _srp->send_cmd(cmd);
    if(r.error != CMD_OK)
    {
        cerr << "Could not set TX sample rate, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return static_cast<double>(r.param);
    }
}

double freesrp_sink_c::get_sample_rate( void )
{
    response r = _srp->send_cmd({GET_TX_SAMP_FREQ, 0});
    if(r.error != CMD_OK)
    {
        cerr << "Could not get TX sample rate, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}

double freesrp_sink_c::set_center_freq( double freq, size_t chan )
{
    command cmd = _srp->make_command(SET_TX_LO_FREQ, freq);
    response r = _srp->send_cmd(cmd);
    if(r.error != CMD_OK)
    {
        cerr << "Could not set TX LO frequency, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return static_cast<double>(r.param);
    }
}

double freesrp_sink_c::get_center_freq( size_t chan )
{
    response r = _srp->send_cmd({GET_TX_LO_FREQ, 0});
    if(r.error != CMD_OK)
    {
        cerr << "Could not get TX LO frequency, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return static_cast<double>(r.param);
    }
}

vector<string> freesrp_sink_c::get_gain_names( size_t chan )
{
    vector<string> names;

    names.push_back("TX_RF");

    return names;
}

osmosdr::gain_range_t freesrp_sink_c::get_gain_range(size_t chan)
{
    osmosdr::meta_range_t gain_ranges;

    gain_ranges.push_back(osmosdr::range_t(0, 89.75, 0.25));

    return gain_ranges;
}

osmosdr::gain_range_t freesrp_sink_c::get_gain_range(const string& name, size_t chan)
{
    return get_gain_range(chan);
}

double freesrp_sink_c::set_gain(double gain, size_t chan)
{
    gain = get_gain_range().clip(gain);

    double atten = 89.75 - gain;

    command cmd = _srp->make_command(SET_TX_ATTENUATION, atten * 1000);
    response r = _srp->send_cmd(cmd);
    if(r.error != CMD_OK)
    {
        cerr << "Could not set TX attenuation, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return 89.75 - (((double) r.param) / 1000.0);
    }
}

double freesrp_sink_c::set_gain(double gain, const string& name, size_t chan)
{
    return set_gain(gain, chan);
}

double freesrp_sink_c::get_gain(size_t chan)
{
    response r = _srp->send_cmd({GET_TX_ATTENUATION, 0});
    if(r.error != CMD_OK)
    {
        cerr << "Could not get TX RF attenuation, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return 89.75 - (((double) r.param) / 1000.0);
    }
}

double freesrp_sink_c::get_gain(const string& name, size_t chan)
{
    return get_gain(chan);
}

double freesrp_sink_c::set_bb_gain(double gain, size_t chan)
{
    return set_gain(gain, chan);
}

vector<string> freesrp_sink_c::get_antennas(size_t chan)
{
    vector<string> antennas;

    antennas.push_back(get_antenna(chan));

    return antennas;
}

string freesrp_sink_c::set_antenna(const string& antenna, size_t chan)
{
    return get_antenna(chan);
}

string freesrp_sink_c::get_antenna(size_t chan)
{
    return "TX";
}

double freesrp_sink_c::set_bandwidth(double bandwidth, size_t chan)
{
    command cmd = _srp->make_command(SET_TX_RF_BANDWIDTH, bandwidth);
    response r = _srp->send_cmd(cmd);
    if(r.error != CMD_OK)
    {
        cerr << "Could not set TX RF bandwidth, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return static_cast<double>(r.param);
    }
}

double freesrp_sink_c::get_bandwidth(size_t chan)
{
    response r = _srp->send_cmd({GET_TX_RF_BANDWIDTH, 0});
    if(r.error != CMD_OK)
    {
        cerr << "Could not get TX RF bandwidth, error: " << r.error << endl;
        return 0;
    }
    else
    {
        return r.param;
    }
}
