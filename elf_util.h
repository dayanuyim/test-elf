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
using std::array;

class OstreamFlagRecover{
public:
    OstreamFlagRecover(ostream &os)
        :os_(os), flags_(os.flags()){}

    ~OstreamFlagRecover(){
        os_.flags(flags_);
    }
private:
    ostream &os_;
    std::ios_base::fmtflags flags_;
};

string readFile(const char *filename)
{
    std::ifstream infile(filename);

    //get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);
    
    unique_ptr<char[]>buffer(new char[length]);
    infile.read(buffer.get(), length);
    return string(buffer.get(), length);
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

    static ProgType of(uint32_t value);

private:
    ProgType(const string &name, uint32_t value):
        name_(name), value_(value)
    {}
    const string name_;
    const uint32_t value_;

public:
    const string &name() const { return name_;}
    uint32_t value() const { return value_;}
};

ProgType ProgType::of(uint32_t value){
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
    OstreamFlagRecover recover(cout);
    return os << type.name() << "(0x" << std::hex << type.value() << ")";
}


class Phdr{
public:
    virtual ProgType p_type() const = 0;
    virtual uint32_t p_flags() const  = 0;
    virtual uint64_t p_offset() const  = 0;
    virtual uint64_t p_vaddr() const  = 0;
    virtual uint64_t p_paddr() const  = 0;
    virtual uint64_t p_filesz() const  = 0;
    virtual uint64_t p_memsz() const  = 0;
    virtual uint64_t p_align() const  = 0;
};

class Phdr32: public Phdr{
public:
    Phdr32(const Elf32_Phdr &phdr)
        :phdr_(phdr)
    { }

    virtual ProgType p_type() const { return ProgType::of(phdr_.p_type);}
    virtual uint32_t p_flags() const { return phdr_.p_flags;}
    virtual uint64_t p_offset() const { return phdr_.p_offset;}
    virtual uint64_t p_vaddr() const { return phdr_.p_vaddr;}
    virtual uint64_t p_paddr() const { return phdr_.p_paddr;}
    virtual uint64_t p_filesz() const { return phdr_.p_filesz;}
    virtual uint64_t p_memsz() const { return phdr_.p_memsz;}
    virtual uint64_t p_align() const { return phdr_.p_align;}
private:
    Elf32_Phdr phdr_;
};

class Phdr64: public Phdr{
public:
    Phdr64(const Elf64_Phdr &phdr)
        :phdr_(phdr)
    { }

    virtual ProgType p_type() const { return ProgType::of(phdr_.p_type);}
    virtual uint32_t p_flags() const { return phdr_.p_flags;}
    virtual uint64_t p_offset() const { return phdr_.p_offset;}
    virtual uint64_t p_vaddr() const { return phdr_.p_vaddr;}
    virtual uint64_t p_paddr() const { return phdr_.p_paddr;}
    virtual uint64_t p_filesz() const { return phdr_.p_filesz;}
    virtual uint64_t p_memsz() const { return phdr_.p_memsz;}
    virtual uint64_t p_align() const { return phdr_.p_align;}
private:
    Elf64_Phdr phdr_;
};

////////////////////////////////////////////////////////////

class SecType{
public:
    static const SecType NULL_;		
    static const SecType PROGBITS;		
    static const SecType SYMTAB;		
    static const SecType STRTAB;		
    static const SecType RELA;		
    static const SecType HASH;		
    static const SecType DYNAMIC;		
    static const SecType NOTE;		
    static const SecType NOBITS;		
    static const SecType REL;		
    static const SecType SHLIB;		
    static const SecType DYNSYM;		
    static const SecType INIT_ARRAY;		
    static const SecType FINI_ARRAY;		
    static const SecType PREINIT_ARRAY;		
    static const SecType GROUP;		
    static const SecType SYMTAB_SHNDX;		
    static const SecType NUM;		
    static const SecType LOOS;		
    static const SecType GNU_ATTRIBUTES;		
    static const SecType GNU_HASH;		
    static const SecType GNU_LIBLIST;		
    static const SecType CHECKSUM;		
    //static const SecType LOSUNW;		
    static const SecType SUNW_move;
    static const SecType SUNW_COMDAT;
    static const SecType SUNW_syminfo;
    static const SecType GNU_verdef;
    static const SecType GNU_verneed;
    static const SecType GNU_versym;
    //static const SecType HISUNW;		
    //static const SecType HIOS;		
    static const SecType LOPROC;		
    static const SecType HIPROC;		
    static const SecType LOUSER;		
    static const SecType HIUSER;		

    static SecType of(uint32_t value);

private:
    SecType(const string &name, uint32_t value):
        name_(name), value_(value)
    {}
    const string name_;
    const uint32_t value_;

public:
    const string &name() const { return name_;}
    uint32_t value() const { return value_;}
};

SecType SecType::of(uint32_t value){
    switch(value){
        case SHT_NULL: return NULL_;		
        case SHT_PROGBITS: return PROGBITS;		
        case SHT_SYMTAB: return SYMTAB;		
        case SHT_STRTAB: return STRTAB;		
        case SHT_RELA: return RELA;		
        case SHT_HASH: return HASH;		
        case SHT_DYNAMIC: return DYNAMIC;		
        case SHT_NOTE: return NOTE;		
        case SHT_NOBITS: return NOBITS;		
        case SHT_REL: return REL;		
        case SHT_SHLIB: return SHLIB;		
        case SHT_DYNSYM: return DYNSYM;		
        case SHT_INIT_ARRAY: return INIT_ARRAY;		
        case SHT_FINI_ARRAY: return FINI_ARRAY;		
        case SHT_PREINIT_ARRAY: return PREINIT_ARRAY;		
        case SHT_GROUP: return GROUP;		
        case SHT_SYMTAB_SHNDX: return SYMTAB_SHNDX;		
        case SHT_NUM: return NUM;		
        case SHT_LOOS: return LOOS;		
        case SHT_GNU_ATTRIBUTES: return GNU_ATTRIBUTES;		
        case SHT_GNU_HASH: return GNU_HASH;		
        case SHT_GNU_LIBLIST: return GNU_LIBLIST;		
        case SHT_CHECKSUM: return CHECKSUM;		
        case SHT_SUNW_move: return SUNW_move;
        case SHT_SUNW_COMDAT: return SUNW_COMDAT;
        case SHT_SUNW_syminfo: return SUNW_syminfo;
        case SHT_GNU_verdef: return GNU_verdef;
        case SHT_GNU_verneed: return GNU_verneed;
        case SHT_GNU_versym: return GNU_versym;
        case SHT_LOPROC: return LOPROC;		
        case SHT_HIPROC: return HIPROC;		
        case SHT_LOUSER: return LOUSER;		
        case SHT_HIUSER: return HIUSER;		
    default:
        return SecType("Invalid", value);
    }
}

const SecType SecType::NULL_{"NULL", SHT_NULL};
const SecType SecType::PROGBITS{"PROGBITS", SHT_PROGBITS};
const SecType SecType::SYMTAB{"SYMTAB", SHT_SYMTAB};
const SecType SecType::STRTAB{"STRTAB", SHT_STRTAB};
const SecType SecType::RELA{"RELA", SHT_RELA};
const SecType SecType::HASH{"HASH", SHT_HASH};
const SecType SecType::DYNAMIC{"DYNAMIC", SHT_DYNAMIC};
const SecType SecType::NOTE{"NOTE", SHT_NOTE};
const SecType SecType::NOBITS{"NOBITS", SHT_NOBITS};
const SecType SecType::REL{"REL", SHT_REL};
const SecType SecType::SHLIB{"SHLIB", SHT_SHLIB};
const SecType SecType::DYNSYM{"DYNSYM", SHT_DYNSYM};
const SecType SecType::INIT_ARRAY{"INIT_ARRAY", SHT_INIT_ARRAY};
const SecType SecType::FINI_ARRAY{"FINI_ARRAY", SHT_FINI_ARRAY};
const SecType SecType::PREINIT_ARRAY{"PREINIT_ARRAY", SHT_PREINIT_ARRAY};
const SecType SecType::GROUP{"GROUP", SHT_GROUP};
const SecType SecType::SYMTAB_SHNDX{"SYMTAB_SHNDX", SHT_SYMTAB_SHNDX};
const SecType SecType::NUM{"NUM", SHT_NUM};
const SecType SecType::LOOS{"LOOS", SHT_LOOS};
const SecType SecType::GNU_ATTRIBUTES{"GNU_ATTRIBUTES", SHT_GNU_ATTRIBUTES};
const SecType SecType::GNU_HASH{"GNU_HASH", SHT_GNU_HASH};
const SecType SecType::GNU_LIBLIST{"GNU_LIBLIST", SHT_GNU_LIBLIST};
const SecType SecType::CHECKSUM{"CHECKSUM", SHT_CHECKSUM};
const SecType SecType::SUNW_move{"SUNW_move", SHT_SUNW_move};
const SecType SecType::SUNW_COMDAT{"SUNW_COMDAT", SHT_SUNW_COMDAT};
const SecType SecType::SUNW_syminfo{"SUNW_syminfo", SHT_SUNW_syminfo};
const SecType SecType::GNU_verdef{"GNU_verdef", SHT_GNU_verdef};
const SecType SecType::GNU_verneed{"GNU_verneed", SHT_GNU_verneed};
const SecType SecType::GNU_versym{"GNU_versym", SHT_GNU_versym};
const SecType SecType::LOPROC{"LOPROC", SHT_LOPROC};
const SecType SecType::HIPROC{"HIPROC", SHT_HIPROC};
const SecType SecType::LOUSER{"LOUSER", SHT_LOUSER};
const SecType SecType::HIUSER{"HIUSER", SHT_HIUSER};

ostream& operator<<(ostream &os, const SecType& type)
{
    OstreamFlagRecover recover(cout);
    return os << type.name() << "(0x" << std::hex << type.value() << ")";
}



class Shdr{
public:
    virtual SecType sh_type() const = 0;
    virtual uint32_t sh_name() const = 0;
    virtual uint64_t sh_flags() const = 0;
    virtual uint64_t sh_addr() const = 0;
    virtual uint64_t sh_offset() const = 0;
    virtual uint64_t sh_size() const = 0;
    virtual uint32_t sh_link() const = 0;
    virtual uint32_t sh_info() const = 0;
    virtual uint64_t sh_addralign() const = 0;
    virtual uint64_t sh_entsize() const = 0;
};

class Shdr32: public Shdr{
public:
    Shdr32(const Elf32_Shdr &shdr)
        :shdr_(shdr){}

    virtual SecType sh_type() const { return SecType::of(shdr_.sh_type);}
    virtual uint32_t sh_name() const { return shdr_.sh_name;}
    virtual uint64_t sh_flags() const { return shdr_.sh_flags;}
    virtual uint64_t sh_addr() const { return shdr_.sh_addr;}
    virtual uint64_t sh_offset() const { return shdr_.sh_offset;}
    virtual uint64_t sh_size() const { return shdr_.sh_size;}
    virtual uint32_t sh_link() const { return shdr_.sh_link;}
    virtual uint32_t sh_info() const { return shdr_.sh_info;}
    virtual uint64_t sh_addralign() const { return shdr_.sh_addralign;}
    virtual uint64_t sh_entsize() const { return shdr_.sh_entsize;}
private:
    Elf32_Shdr shdr_;
};

class Shdr64: public Shdr{
public:
    Shdr64(const Elf64_Shdr &shdr)
        :shdr_(shdr){}

    virtual SecType sh_type() const { return SecType::of(shdr_.sh_type);}
    virtual uint32_t sh_name() const { return shdr_.sh_name;}
    virtual uint64_t sh_flags() const { return shdr_.sh_flags;}
    virtual uint64_t sh_addr() const { return shdr_.sh_addr;}
    virtual uint64_t sh_offset() const { return shdr_.sh_offset;}
    virtual uint64_t sh_size() const { return shdr_.sh_size;}
    virtual uint32_t sh_link() const { return shdr_.sh_link;}
    virtual uint32_t sh_info() const { return shdr_.sh_info;}
    virtual uint64_t sh_addralign() const { return shdr_.sh_addralign;}
    virtual uint64_t sh_entsize() const { return shdr_.sh_entsize;}
private:
    Elf64_Shdr shdr_;
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

    static ElfType of(uint16_t value);
private:
    ElfType(const string &name, uint16_t value):
        name_(name), value_(value)
    {}
    const string name_;
    const uint16_t value_;

public:
    const string &name() const { return name_;}
    uint16_t value() const { return value_;}
};

ElfType ElfType::of(uint16_t value){
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
    OstreamFlagRecover recover(cout);
    return os << type.name() << "(0x" << std::hex << type.value() << ")";
}

class Ehdr{
public:
    virtual ElfType e_type() const = 0;
    virtual const unsigned char *e_ident() const = 0;
    virtual uint64_t e_phoff() const = 0;
    virtual uint16_t e_phentsize() const = 0;
    virtual uint16_t e_phnum() const = 0;
    virtual uint64_t e_shoff() const = 0;
    virtual uint16_t e_shentsize() const = 0;
    virtual uint16_t e_shnum() const = 0;
    virtual size_t e_size() const = 0;
private:
};

class Ehdr32: public Ehdr{
public:
    Ehdr32(const Elf32_Ehdr &hdr)
        :hdr_(hdr)
    {
    }

    virtual ElfType e_type() const { return ElfType::of(hdr_.e_type);}
    virtual const char unsigned *e_ident() const { return hdr_.e_ident;}
    virtual uint64_t e_phoff() const { return hdr_.e_phoff;}
    virtual uint16_t e_phentsize() const { return hdr_.e_phentsize;}
    virtual uint16_t e_phnum() const { return hdr_.e_phnum;}
    virtual uint64_t e_shoff() const { return hdr_.e_shoff;}
    virtual uint16_t e_shentsize() const { return hdr_.e_shentsize;}
    virtual uint16_t e_shnum() const { return hdr_.e_shnum;}
    virtual size_t e_size() const { return sizeof(hdr_);}
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
    virtual const char unsigned *e_ident() const { return hdr_.e_ident;}
    virtual uint64_t e_phoff() const { return hdr_.e_phoff;}
    virtual uint16_t e_phentsize() const { return hdr_.e_phentsize;}
    virtual uint16_t e_phnum() const { return hdr_.e_phnum;}
    virtual uint64_t e_shoff() const { return hdr_.e_shoff;}
    virtual uint16_t e_shentsize() const { return hdr_.e_shentsize;}
    virtual uint16_t e_shnum() const { return hdr_.e_shnum;}
    virtual size_t e_size() const { return sizeof(hdr_);}
private:
private:
    Elf64_Ehdr hdr_;
};

//////////////////////////////////////////////////////////
class ELF{
public:
    ELF(const char *filename)
    {
        buffer_ = readFile(filename);
        if(size() < EI_NIDENT)
            throw std::invalid_argument("invalid elf header len");

        ehdr_ = unique_ptr<Ehdr>(toEhdr(buffer_.c_str()));

        for(uint16_t i = 0; i < e_phnum(); ++i){
            uint64_t offset = e_phoff() + i * e_phentsize();
            phdrs_.emplace_back(toPhdr(buffer_.c_str() + offset));
        }

        for(uint16_t i = 0; i < e_shnum(); ++i){
            uint64_t offset = e_shoff() + i * e_shentsize();
            shdrs_.emplace_back(toShdr(buffer_.c_str() + offset));
        }
    }

    size_t size() const { return buffer_.length(); }
    size_t e_size() const { return ehdr_->e_size(); }
    int e_ident_class() const { return ehdr_->e_ident()[EI_CLASS];}
    ElfType e_type() const { return ehdr_->e_type();}
    uint64_t e_phoff() const { return ehdr_->e_phoff();}
    uint16_t e_phentsize() const { return ehdr_->e_phentsize();}
    uint16_t e_phnum() const { return ehdr_->e_phnum();}
    uint64_t e_shoff() const { return ehdr_->e_shoff();}
    uint16_t e_shentsize() const { return ehdr_->e_shentsize();}
    uint16_t e_shnum() const { return ehdr_->e_shnum();}
    const unique_ptr<Phdr> &phdr(size_t i) const { return phdrs_[i]; }
    const unique_ptr<Shdr> &shdr(size_t i) const { return shdrs_[i]; }

private:
    Ehdr* toEhdr(const void *buffer){
        unsigned char cls =  ((const unsigned char*)buffer)[EI_CLASS];
        if(cls == ELFCLASS32)
            return new Ehdr32(*(Elf32_Ehdr*)buffer);
        else if(cls == ELFCLASS64)
            return new Ehdr64(*(Elf64_Ehdr*)buffer);
        else
            throw std::invalid_argument("invalid elf class");
    }

    Phdr* toPhdr(const void *buffer){
        return (e_ident_class() == ELFCLASS32)?
                    (Phdr*) new Phdr32(*(Elf32_Phdr*)buffer):
                    (Phdr*) new Phdr64(*(Elf64_Phdr*)buffer);
    }

    Shdr* toShdr(const void *buffer){
        return (e_ident_class() == ELFCLASS32)?
                    (Shdr*) new Shdr32(*(Elf32_Shdr*)buffer):
                    (Shdr*) new Shdr64(*(Elf64_Shdr*)buffer);
    }

    string buffer_;
    unique_ptr<Ehdr> ehdr_;
    vector<unique_ptr<Phdr>> phdrs_;
    vector<unique_ptr<Shdr>> shdrs_;
};

/////////////////////////////////////////////////////////////////////

/*
class Elf64{
public:
    Elf64(const string &buffer)
        :buffer_(buffer)
    {
        ehdr_ = (Elf64_Ehdr*) buffer_.c_str();
        phdrs_ = (Elf64_Phdr*) (buffer_.c_str() + ehdr_->e_phoff);
        shdrs_ = (Elf64_Shdr*) (buffer_.c_str() + ehdr_->e_shoff);
    }

    size_t size() const { return buffer_.length(); }
    size_t e_size() const { return sizeof(Elf64_Ehdr); }
    int e_ident_class() const { return ehdr_->e_ident[EI_CLASS];}
    ElfType e_type() const { return ElfType.of(ehdr_->e_type);}
    uint64_t e_phoff() const { return ehdr_->e_phoff;}
    uint16_t e_phentsize() const { return ehdr_->e_phentsize;}
    uint16_t e_phnum() const { return ehdr_->e_phnum;}
    uint64_t e_shoff() const { return ehdr_->e_shoff;}
    uint16_t e_shentsize() const { return ehdr_->e_shentsize;}
    uint16_t e_shnum() const { return ehdr_->e_shnum();}

    const Elf64_Ehdr &ehdr() const { return *ehdr;}
    const Elf64_Phdr &phdr(size_t i) const { return phdrs_[i]; }
    const Elf64_Shdr &shdr(size_t i) const { return shdrs_[i]; }

private:
    const string buffer_;
    Elf64_Ehdr* ehdr_;
    Elf64_Phdr* phdrs_;
    Elf64_Shdr* shdrs_;
}

Elf64 parseElf(const char *filename)
{
    const string buffer = readFile(filename);
    if(size() < EI_NIDENT)
        throw std::invalid_argument("invalid elf header len");

    unsigned char cls =  buffer[EI_CLASS];
    if(cls == ELFCLASS32)
        return new Ehdr32(*(Elf32_Ehdr*)buffer);
    else if(cls == ELFCLASS64)
        return Elf64(buffer);
    else
        throw std::invalid_argument("invalid elf class");
}
*/

} //namespace end
