#ifndef INCLUDED_FREESRP_COMMON_H
#define INCLUDED_FREESRP_COMMON_H

#include <vector>
#include <string>

#include <freesrp.hpp>

class freesrp_common
{
    public:
        static std::vector<std::string> get_devices();
};

#endif
