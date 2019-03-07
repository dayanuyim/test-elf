#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <elf.h>
#include <vector>

namespace elf{

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::unique_ptr;
using std::ostringstream;
using std::move;

unique_ptr<char[]> readFile(const char *filename)
{
    std::ifstream infile(filename);

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);
    
    unique_ptr<char[]>buffer(new char[length]);
    infile.read(buffer.get(), length);
    return buffer;
}

////////////////////////////////////////////////////////////

string progtype(int value){
#define PROCESS_VAL(p) case(p): return #p;
    switch(value){
        PROCESS_VAL(PT_NULL);
        PROCESS_VAL(PT_LOAD);
        PROCESS_VAL(PT_DYNAMIC);
        PROCESS_VAL(PT_INTERP);
        PROCESS_VAL(PT_NOTE);
        PROCESS_VAL(PT_SHLIB);
        PROCESS_VAL(PT_PHDR);
        PROCESS_VAL(PT_TLS);
        PROCESS_VAL(PT_NUM);
        PROCESS_VAL(PT_LOOS);
        PROCESS_VAL(PT_GNU_EH_FRAME);
        PROCESS_VAL(PT_GNU_STACK);
        PROCESS_VAL(PT_GNU_RELRO);
        //PROCESS_VAL(PT_LOSUNW);
        PROCESS_VAL(PT_SUNWBSS);
        PROCESS_VAL(PT_SUNWSTACK);
        //PROCESS_VAL(PT_HISUNW);
        PROCESS_VAL(PT_HIOS);
        PROCESS_VAL(PT_LOPROC);
        PROCESS_VAL(PT_HIPROC);
        default:
            ostringstream sout;
            sout << "PT_INVALID_" << value;
            return sout.str();
    }
#undef PROCESS_VAL
}


class Phdr{
public:
    virtual int p_type() const = 0;
};

class Phdr32: public Phdr{
public:
    Phdr32(const Elf32_Phdr &phdr)
        :phdr_(phdr)
    { }

    virtual int p_type() const { return phdr_.p_type;}
private:
    Elf32_Phdr phdr_;
};

class Phdr64: public Phdr{
public:
    Phdr64(const Elf64_Phdr &phdr)
        :phdr_(phdr)
    { }

    virtual int p_type() const { return phdr_.p_type;}
private:
    Elf64_Phdr phdr_;
};

////////////////////////////////////////////////////////////

string elftype(int value){
#define PROCESS_VAL(p) case(p): return #p;
    switch(value){
        PROCESS_VAL(ET_NONE);
        PROCESS_VAL(ET_REL);
        PROCESS_VAL(ET_EXEC);
        PROCESS_VAL(ET_DYN);
        PROCESS_VAL(ET_CORE);
        PROCESS_VAL(ET_NUM);
        PROCESS_VAL(ET_LOOS);
        PROCESS_VAL(ET_HIOS);
        PROCESS_VAL(ET_LOPROC);
        PROCESS_VAL(ET_HIPROC);
        default:
            ostringstream sout;
            sout << "ET_INVALID_" << value;
            return sout.str();
    }
#undef PROCESS_VAL
}

class Ehdr{
public:
    virtual int e_type() const = 0;
    virtual uint64_t e_phoff() const = 0;
    virtual uint16_t e_phentsize() const = 0;
    virtual uint16_t e_phnum() const = 0;
private:
};

class Ehdr32: public Ehdr{
public:
    Ehdr32(const Elf32_Ehdr &hdr)
        :hdr_(hdr)
    {
    }

    virtual int e_type() const { return hdr_.e_type;}
    virtual uint64_t e_phoff() const { return hdr_.e_phoff;}
    virtual uint16_t e_phentsize() const { return hdr_.e_phentsize;}
    virtual uint16_t e_phnum() const { return hdr_.e_phnum;}
private:
    Elf32_Ehdr hdr_;
};

class Ehdr64: public Ehdr{
public:
    Ehdr64(const Elf64_Ehdr &hdr)
        :hdr_(hdr)
    {
    }

    virtual int e_type() const { return hdr_.e_type;}
    virtual uint64_t e_phoff() const { return hdr_.e_phoff;}
    virtual uint16_t e_phentsize() const { return hdr_.e_phentsize;}
    virtual uint16_t e_phnum() const { return hdr_.e_phnum;}
private:
private:
    Elf64_Ehdr hdr_;
};

//////////////////////////////////////////////////////////
class ELF{
public:
    ELF(const char *filename)
    {
        unique_ptr<char[]> buffer = readFile(filename);

        int cls = (int)buffer[EI_CLASS];
        is_32bits_ = (cls == ELFCLASS32)? true:
                     (cls == ELFCLASS64)? false:
                     throw std::invalid_argument("invalid elf class");

        ehdr_ = unique_ptr<Ehdr>(toEhdr(buffer.get()));

        for(uint16_t i = 0; i < e_phnum(); ++i)
            phdrs_.emplace_back(toPhdr(buffer.get() + e_phoff() + i * e_phentsize()));

        for(size_t i = 0; i < phdrs_.size(); ++i)
            cout << "#" << i << ": prog type: " << progtype(phdrs_[i]->p_type()) << endl;
    }

    int e_type() const { return ehdr_->e_type();}
    uint64_t e_phoff() const { return ehdr_->e_phoff();}
    uint16_t e_phentsize() const { return ehdr_->e_phentsize();}
    uint16_t e_phnum() const { return ehdr_->e_phnum();}

private:
    Ehdr* toEhdr(const char *buffer){
        return is_32bits_? (Ehdr*) new Ehdr32(*(Elf32_Ehdr*)buffer):
                           (Ehdr*) new Ehdr64(*(Elf64_Ehdr*)buffer);
    }

    Phdr* toPhdr(const char *buffer){
        return is_32bits_? (Phdr*) new Phdr32(*(Elf32_Phdr*)buffer):
                           (Phdr*) new Phdr64(*(Elf64_Phdr*)buffer);
    }

    bool is_32bits_;
    unique_ptr<Ehdr> ehdr_;
    vector<unique_ptr<Phdr>> phdrs_;
};

}; //namespace end
