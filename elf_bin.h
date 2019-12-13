#ifndef __ELF_BIN_H
#define __ELF_BIN_H 1

#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <elf.h>

namespace iii{

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

////////////////////////////////////////////////////////////

struct Addr{
    Addr(int value = 0){ this->value = value;}
    int value;
};
ostream& operator<<(ostream &os, const Addr &addr);


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


ostream& operator<<(ostream &os, const ProgType& type);

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


ostream& operator<<(ostream &os, const SecType& type);



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

ostream& operator<<(ostream &os, const ElfType& type);

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
    virtual uint16_t e_shstrndx() const = 0;
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
    virtual const char unsigned * e_ident() const { return hdr_.e_ident;}
    virtual uint64_t e_phoff()     const { return hdr_.e_phoff;}
    virtual uint16_t e_phentsize() const { return hdr_.e_phentsize;}
    virtual uint16_t e_phnum()     const { return hdr_.e_phnum;}
    virtual uint64_t e_shoff()     const { return hdr_.e_shoff;}
    virtual uint16_t e_shentsize() const { return hdr_.e_shentsize;}
    virtual uint16_t e_shnum()     const { return hdr_.e_shnum;}
    virtual uint16_t e_shstrndx()  const { return hdr_.e_shstrndx;}
    virtual size_t   e_size()      const { return sizeof(hdr_);}
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
    virtual uint64_t e_phoff()     const { return hdr_.e_phoff;}
    virtual uint16_t e_phentsize() const { return hdr_.e_phentsize;}
    virtual uint16_t e_phnum()     const { return hdr_.e_phnum;}
    virtual uint64_t e_shoff()     const { return hdr_.e_shoff;}
    virtual uint16_t e_shentsize() const { return hdr_.e_shentsize;}
    virtual uint16_t e_shnum()     const { return hdr_.e_shnum;}
    virtual uint16_t e_shstrndx()  const { return hdr_.e_shstrndx;}
    virtual size_t e_size()        const { return sizeof(hdr_);}
private:
private:
    Elf64_Ehdr hdr_;
};

//////////////////////////////////////////////////////////
class ELF{
public:
    ELF(const char *filename);

    size_t filesize() const { return bin_.length(); }
    size_t e_size()        const { return ehdr_->e_size(); }
    int e_ident_class()    const { return ehdr_->e_ident()[EI_CLASS];}
    ElfType  e_type()      const { return ehdr_->e_type();}
    uint64_t e_phoff()     const { return ehdr_->e_phoff();}
    uint16_t e_phentsize() const { return ehdr_->e_phentsize();}
    uint16_t e_phnum()     const { return ehdr_->e_phnum();}
    uint64_t e_shoff()     const { return ehdr_->e_shoff();}
    uint16_t e_shentsize() const { return ehdr_->e_shentsize();}
    uint16_t e_shnum()     const { return ehdr_->e_shnum();}
    uint16_t e_shstrndx()  const { return ehdr_->e_shstrndx();}

    string get_sh_name(size_t i) const {
        uint64_t offset = shdrs_[e_shstrndx()]->sh_offset() + shdrs_[i]->sh_name();
        return string(bin_.c_str() + offset);
    }

    string dump_section(size_t i) const {
        const auto &shdr = shdrs_[i];
        return string(bin_.c_str() + shdr->sh_offset(), shdr->sh_size());
    }

    vector<string> dump_section_strs(size_t i) const;

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

    string bin_;
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

#endif
