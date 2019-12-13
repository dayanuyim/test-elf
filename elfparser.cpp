#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include "elf_bin.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::ostringstream;
using namespace iii;


void printUsage()
{
    cerr << "usage: elfparser <elf-file>" << endl;
}

string str(ostringstream &sout)
{
    string s = sout.str();
    sout.str("");
    sout.clear();
    return s;
}

map<uint64_t,string> makeLayout(const ELF &elf)
{
    ostringstream sout;
    map<uint64_t,string> addrmap;

    // Elf header ====
    uint64_t offset = 0;
    addrmap[offset] = "----EHdr-----"; 

    offset = elf.e_size();
    addrmap[offset] = "-------------";

    // Program Header
    if(elf.e_phnum() > 0){
        offset = elf.e_phoff();
        addrmap[offset] = "----Phdr-----"; 

        for(size_t i = 0; i < elf.e_phnum(); ++i){
            const auto &phdr = elf.phdr(i);
            sout << "Phdr[" << i << "] " << phdr->p_type()
                 << " " << Addr(phdr->p_offset())
                 << "~" << Addr(phdr->p_offset() + phdr->p_filesz());
            addrmap[offset] = str(sout);
            offset += elf.e_phentsize();
        }

        addrmap[offset] = "-------------";
    }

    // Sections ======================================
    if(elf.e_shnum() > 0){
        offset = elf.shdr(0)->sh_offset();
        addrmap[offset] = "---- Sections -----"; 

        for(size_t i = 0; i < elf.e_shnum(); ++i){
            const auto &shdr = elf.shdr(i);

            offset = shdr->sh_offset();
            sout << "Section[" << i << "] " << shdr->sh_type();
            addrmap[offset] = str(sout);

            offset += shdr->sh_size();
            addrmap[offset] ="-------------";
        }
    }

    // Section Header ===================
    if(elf.e_shnum() > 0){
        offset = elf.e_shoff();
        addrmap[offset] ="----Shdr-----"; 

        for(size_t i = 0; i < elf.e_shnum(); ++i){
            const auto &shdr = elf.shdr(i);
            const string &shname = elf.get_sh_name(i);
            const auto &shtype = shdr->sh_type();

            sout << "Shdr[" << i << "] " << shtype << " " << shname;
            addrmap[offset] = str(sout);

            offset += elf.e_shentsize();
            addrmap[offset] = "-------------";
        }
    }

    addrmap[elf.filesize()] =  "----EOF----";
    return addrmap;
}

vector<string> findGccCmdArgs(const ELF &elf)
{
    for(size_t i = 0; i < elf.e_shnum(); ++i){
        const string &shname = elf.get_sh_name(i);
        if(shname.compare(".GCC.command.line") == 0)
            return elf.dump_section_strs(i);
    }

    return vector<string>{};
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

    map<uint64_t, string> layout = makeLayout(elf);
    for(auto it = layout.cbegin(); it != layout.cend(); ++it)
        cout << Addr(it->first) << " " << it->second << endl;

    vector<string> args = findGccCmdArgs(elf);
    if(args.empty())
        cout << "Gcc Args: N/A" << endl;
    else{
        cout << "Gcc Args:" << endl;
        for(auto it = args.cbegin(); it != args.cend(); ++it)
            cout << "  " << *it << endl;
    }

    return 0;
}
