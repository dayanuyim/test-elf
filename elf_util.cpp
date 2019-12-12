#include "elf_util.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace elf{

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

//////////////////////////////////////////////////////////////////////

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
////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////

vector<string> ELF::dump_section_strs(size_t i) const {

    vector<string> strs;

    const auto &shdr = shdrs_[i];
    const char *sec = bin_.c_str() + shdr->sh_offset();
    const char *sec_end = sec + shdr->sh_size();

    while(sec < sec_end){
        const char *cp;
        for(cp = sec; cp < sec_end; ++cp){
            if(*cp == 0)
                break;
        }
        strs.emplace_back(sec, cp-sec);
        sec = cp + 1;
    }

    return strs;
}

} //namespace end
