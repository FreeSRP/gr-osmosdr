#include "freesrp_sink_c.h"

using namespace FreeSRP;
using namespace std;

freesrp_sink_c_sptr make_freesrp_sink_c (const std::string &args)
{
    return gnuradio::get_initial_sptr(new freesrp_sink_c (args));
}

freesrp_sink_c::freesrp_sink_c (const std::string & args)
{

}
