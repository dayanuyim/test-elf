#ifndef __TTT_UTILS_H
#define __TTT_UTILS_H 1

#include <iostream>
#include <string>

namespace iii{

std::string readFile(const char *filename);

class OstreamFlagRecover{
public:
    OstreamFlagRecover(std::ostream &os)
        :os_(os), flags_(os.flags()){}

    ~OstreamFlagRecover(){
        os_.flags(flags_);
    }
private:
    std::ostream &os_;
    std::ios_base::fmtflags flags_;
};


} //end of namespace
#endif
