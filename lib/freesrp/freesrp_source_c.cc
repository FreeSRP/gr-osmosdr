#include "freesrp_source_c.h"

using namespace FreeSRP;
using namespace std;

freesrp_source_c_sptr make_freesrp_source_c (const std::string &args)
{
    return gnuradio::get_initial_sptr(new freesrp_source_c (args));
}

freesrp_source_c::freesrp_source_c (const std::string & args)
{

}
