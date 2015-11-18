#include "freesrp_common.h"

using namespace FreeSRP;
using namespace std;

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
