#include <iostream>
#include <stdexcept>
#include <string>
#include "elf_util.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::unique_ptr;
using namespace elf;

void printUsage()
{
    cerr << "usage: elfparser <elf-file>" << endl;
}

int main(int argc, char* argv[])
{
    if(argc != 2){
        printUsage();
        exit(1);
    }

    const char* filename = argv[1];
    ELF elf(filename);
    cout << "ELF Type: " << elftype(elf.e_type()) << endl;
    return 0;
}
