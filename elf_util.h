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
using std::ostream;
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

class ProgType{
public:
    static const ProgType NULL_;
    static const ProgType LOAD;
    static const ProgType DYNAMIC;
    static const ProgType INTERP;
    static const ProgType NOTE;
    static const ProgType SHLIB;
    static const ProgType PHDR;
    static const ProgType TLS;
    static const ProgType NUM;
    static const ProgType LOOS;
    static const ProgType GNU_EH_FRAME;
    static const ProgType GNU_STACK;
    static const ProgType GNU_RELRO;
    //static const ProgType LOSUNW;
    static const ProgType SUNWBSS;
    static const ProgType SUNWSTACK;
    //static const ProgType HISUNW;
    static const ProgType HIOS;
    static const ProgType LOPROC;
    static const ProgType HIPROC;

    static ProgType of(int value){
        switch(value){
        case PT_NULL: return NULL_;
        case PT_LOAD: return LOAD;
        case PT_DYNAMIC: return DYNAMIC;
        case PT_INTERP: return INTERP;
        case PT_NOTE: return NOTE;
        case PT_SHLIB: return SHLIB;
        case PT_PHDR: return PHDR;
        case PT_TLS: return TLS;
        case PT_NUM: return NUM;
        case PT_LOOS: return LOOS;
        case PT_GNU_EH_FRAME: return GNU_EH_FRAME;
        case PT_GNU_STACK: return GNU_STACK;
        case PT_GNU_RELRO: return GNU_RELRO;
        case PT_SUNWBSS: return SUNWBSS;
        case PT_SUNWSTACK: return SUNWSTACK;
        case PT_HIOS: return HIOS;
        case PT_LOPROC: return LOPROC;
        case PT_HIPROC: return HIPROC;
        default:
            return ProgType("Invalid", value);
        }
    }
private:
    ProgType(const string &name, int value):
        name_(name), value_(value)
    {}
    const string name_;
    const int value_;

public:
    const string &name() const { return name_;}
    int value() const { return value_;}
};

const ProgType ProgType::NULL_{"NULL_", PT_NULL};
const ProgType ProgType::LOAD{"LOAD", PT_LOAD};
const ProgType ProgType::DYNAMIC{"DYNAMIC", PT_DYNAMIC};
const ProgType ProgType::INTERP{"INTERP", PT_INTERP};
const ProgType ProgType::NOTE{"NOTE", PT_NOTE};
const ProgType ProgType::SHLIB{"SHLIB", PT_SHLIB};
const ProgType ProgType::PHDR{"PHDR", PT_PHDR};
const ProgType ProgType::TLS{"TLS", PT_TLS};
const ProgType ProgType::NUM{"NUM", PT_NUM};
const ProgType ProgType::LOOS{"LOOS", PT_LOOS};
const ProgType ProgType::GNU_EH_FRAME{"GNU_EH_FRAME", PT_GNU_EH_FRAME};
const ProgType ProgType::GNU_STACK{"GNU_STACK", PT_GNU_STACK};
const ProgType ProgType::GNU_RELRO{"GNU_RELRO", PT_GNU_RELRO};
const ProgType ProgType::SUNWBSS{"SUNWBSS", PT_SUNWBSS};
const ProgType ProgType::SUNWSTACK{"SUNWSTACK", PT_SUNWSTACK};
const ProgType ProgType::HIOS{"HIOS", PT_HIOS};
const ProgType ProgType::LOPROC{"LOPROC", PT_LOPROC};
const ProgType ProgType::HIPROC{"HIPROC", PT_HIPROC};

ostream& operator<<(ostream &os, const ProgType& type)
{
    return os << type.name() << "(0x" << std::hex << type.value() << ")";
}


class Phdr{
public:
    virtual ProgType p_type() const = 0;
};

class Phdr32: public Phdr{
public:
    Phdr32(const Elf32_Phdr &phdr)
        :phdr_(phdr)
    { }

    virtual ProgType p_type() const { return ProgType::of(phdr_.p_type);}
private:
    Elf32_Phdr phdr_;
};

class Phdr64: public Phdr{
public:
    Phdr64(const Elf64_Phdr &phdr)
        :phdr_(phdr)
    { }

    virtual ProgType p_type() const { return ProgType::of(phdr_.p_type);}
private:
    Elf64_Phdr phdr_;
};

////////////////////////////////////////////////////////////

class ElfType{
public:
    static const ElfType NONE;
    static const ElfType REL;
    static const ElfType EXEC;
    static const ElfType DYN;
    static const ElfType CORE;
    static const ElfType NUM;
    static const ElfType LOOS;
    static const ElfType HIOS;
    static const ElfType LOPROC;
    static const ElfType HIPROC;

    static ElfType of(int value){
        switch(value){
        case ET_NONE: return NONE;
        case ET_REL: return REL;
        case ET_EXEC: return EXEC;
        case ET_DYN: return DYN;
        case ET_CORE: return CORE;
        case ET_NUM: return NUM;
        case ET_LOOS: return LOOS;
        case ET_HIOS: return HIOS;
        case ET_LOPROC: return LOPROC;
        case ET_HIPROC: return HIPROC;
        default:
            return ElfType("Invalid", value);
        }
    }
private:
    ElfType(const string &name, int value):
        name_(name), value_(value)
    {}
    const string name_;
    const int value_;

public:
    const string &name() const { return name_;}
    int value() const { return value_;}
};

const ElfType ElfType::NONE{"None", ET_NONE};
const ElfType ElfType::REL{"Rel", ET_REL};
const ElfType ElfType::EXEC{"Exec", ET_EXEC};
const ElfType ElfType::DYN{"Dyn", ET_DYN};
const ElfType ElfType::CORE{"Core", ET_CORE};
const ElfType ElfType::NUM{"Num", ET_NUM};
const ElfType ElfType::LOOS{"LoOS", ET_LOOS};
const ElfType ElfType::HIOS{"HiOS", ET_HIOS};
const ElfType ElfType::LOPROC{"LoProc", ET_LOPROC};
const ElfType ElfType::HIPROC{"HiProc", ET_HIPROC};

ostream& operator<<(ostream &os, const ElfType& type)
{
    return os << type.name() << "(0x" << std::hex << type.value() << ")";
}

class Ehdr{
public:
    virtual ElfType e_type() const = 0;
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

    virtual ElfType e_type() const { return ElfType::of(hdr_.e_type);}
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

    virtual ElfType e_type() const { return ElfType::of(hdr_.e_type);}
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

        for(uint16_t i = 0; i < e_phnum(); ++i){
            const char *buf = buffer.get() + e_phoff() + i * e_phentsize();
            phdrs_.emplace_back(toPhdr(buf));
        }

        for(size_t i = 0; i < phdrs_.size(); ++i)
            cout << "#" << i << ": prog type: " << phdrs_[i]->p_type() << endl;
    }

    ElfType e_type() const { return ehdr_->e_type();}
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

} //namespace end
