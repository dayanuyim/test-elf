#include <fstream>
#include <memory>
#include <string>
#include "utils.h"

namespace iii{

std::string readFile(const char *filename)
{
    std::ifstream infile(filename);

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);
    
    std::unique_ptr<char[]>buffer(new char[length]);
    infile.read(buffer.get(), length);
    return std::string(buffer.get(), length);
}

} //end of namespace
