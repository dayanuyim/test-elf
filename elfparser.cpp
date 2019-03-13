#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include "elf_util.h"

using std::cout;
using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::string;
using std::setfill;
using std::setw;
using namespace elf;

struct Addr{
    Addr(int value = 0){ this->value = value;}
    int value;
};

ostream& operator<<(ostream &os, const Addr &addr)
{
    OstreamFlagRecover recover(os);
    return os << "0x" << setfill('0') << setw(8) << hex << addr.value;
}


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

    cout << "ELF Type: " << elf.e_type() << endl;

    // Elf header ====
    uint64_t offset = 0;
    cout << Addr(offset) << " ---EHdr-----" << endl;
    offset = elf.e_size();

    // Program Header
    if(elf.e_phnum() > 0){
        if(offset != elf.e_phoff())
            cout << Addr(offset) << " ------------" << endl;
        offset = elf.e_phoff();
        cout << Addr(offset) << " ---Phdr-----" << endl; 

        for(size_t i = 0; i < elf.e_phnum(); ++i){
            const auto &phdr = elf.phdr(i);
            cout << Addr(offset) << " Phdr[" << i << "] " << phdr->p_type()
                 << " 0x" << Addr(phdr->p_offset())
                 << "~0x" << Addr(phdr->p_offset() + phdr->p_filesz()) << endl;
            offset += elf.e_phentsize();
        }
    }

    // Sections ======================================
    if(elf.e_shnum() > 0){
        if(offset != elf.shdr(0)->sh_offset())
            cout << Addr(offset) << " ------------" << endl;
        offset = elf.shdr(0)->sh_offset();
        cout << Addr(offset) << " --- Sections -----" << endl; 

        for(size_t i = 0; i < elf.e_shnum(); ++i){
            const auto &shdr = elf.shdr(i);
            if(offset != shdr->sh_offset())
                cout << Addr(offset) << " ------------" << endl;
            offset = shdr->sh_offset();
            cout << Addr(offset) << " Section[" << i << "] " << shdr->sh_type() << endl;
            offset += shdr->sh_size();
        }
    }

    // Section Header ===================
    if(elf.e_shnum() > 0){
        if(offset != elf.e_shoff())
            cout << Addr(offset) << " ------------" << endl;
        offset = elf.e_shoff();
        cout << Addr(offset) << " ---Shdr-----" << endl; 

        for(size_t i = 0; i < elf.e_shnum(); ++i){
            const auto &shdr = elf.shdr(i);
            cout << Addr(offset) << " Shdr[" << i << "] " << shdr->sh_type() << endl;
            offset += elf.e_shentsize();
        }
    }

    //EOF ============================
    if(offset != elf.size())
        cout << Addr(offset) << " ------------" << endl;
    cout << Addr(elf.size()) << " ---EOF----" << endl;

    return 0;
}
