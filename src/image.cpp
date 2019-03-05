// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//
#include "image.hpp"

#ifndef SWAP_ENDIANNESS

inline void swapLEtoBE(uint16_t &a) {
    (void) a;
}

inline void swapLEtoBEa(void *a, size_t size) {
    (void) a;
    (void) size;
}

#else

inline void swapLEtoBE(uint16_t &a) {
    a = (a << 8) | (a >> 8);
}

inline void swapLEtoBEa(void *a, size_t size) {
    for (size_t i = 0; i < size / 2; ++i)
        swapLEtoBE(static_cast<uint16_t *>(a)[i]);
}

#endif

BfppSection::BfppSection(std::vector<Command> &SectionCmd, uint16_t MemoryBase, uint16_t MemorySize) {
    Hdr.MemoryBase = MemoryBase;
    Hdr.MemorySize = MemorySize;
    Hdr.FileSize = static_cast<uint16_t>(SectionCmd.size() * sizeof(uint16_t));
    Hdr.type = bflang::SECTION_CODE;

    if (Hdr.FileSize != 0)
        for (auto &cmd : SectionCmd)
            Data.push_back(cmd.GetCmd());
}

BfppSection::BfppSection(std::vector<uint16_t> &SectionData, uint16_t MemoryBase, uint16_t MemorySize) {
    Hdr.MemoryBase = MemoryBase;
    Hdr.MemorySize = MemorySize;
    Hdr.FileSize = static_cast<uint16_t>(SectionData.size() * sizeof(uint16_t));
    Hdr.type = bflang::SECTION_DATA;

    if (Hdr.FileSize != 0)
        for (auto &byte : SectionData)
            Data.push_back(byte);
}

BfppSection::BfppSection(std::fstream &File) {
    File.read(reinterpret_cast<char *>(&Hdr), sizeof(bflang::BfppSection));
    swapLEtoBEa(&Hdr, sizeof(bflang::BfppSection));

    std::streampos Image_pos = File.tellg();

    Data.resize(MemorySize(), 0);
    if (FileSize() != 0) {
        File.seekg(FileBase());
        File.read(reinterpret_cast<char *>(Data.data()), FileSize());
        swapLEtoBEa(Data.data(), FileSize());
    }

    File.seekg(Image_pos);
}

void BfppSection::WriteHeader(std::fstream &File) {
    Hdr.FileBase = static_cast<uint16_t>(File.tellp()); // XXX: ???

    swapLEtoBEa(&Hdr, sizeof(bflang::BfppSection));
    File.write(reinterpret_cast<char *>(&Hdr), sizeof(bflang::BfppSection));
    swapLEtoBEa(&Hdr, sizeof(bflang::BfppSection));
}

void BfppSection::WriteData(std::fstream &File) {
    std::streampos FileBase = File.tellp();
    // Valgrind says:
    //   Syscall param write(buf) points to uninitialised byte(s)
    //   Address 0x4db2a22 is 18 bytes inside a block of size 8,192 alloc'd
    //   std::basic_filebuf<char, std::char_traits<char> >::_M_allocate_internal_buffer()
    // Bug in stdlib?
    File.seekg(Hdr.FileBase);
    Hdr.FileBase = static_cast<uint16_t>(FileBase); // XXX: ???

    swapLEtoBEa(&Hdr, sizeof(bflang::BfppSection));
    File.write(reinterpret_cast<char *>(&Hdr), sizeof(bflang::BfppSection));
    swapLEtoBEa(&Hdr, sizeof(bflang::BfppSection));

    File.seekg(FileBase);


    if (Hdr.FileSize != 0) {
        swapLEtoBEa(Data.data(), Hdr.FileSize);
        File.write(reinterpret_cast<char *>(Data.data()), Hdr.FileSize);
        swapLEtoBEa(Data.data(), Hdr.FileSize);
    }
}

BfppImage::BfppImage(std::fstream &File) {
    File.read(reinterpret_cast<char *>(&Hdr), sizeof(bflang::BfppImage));
    swapLEtoBE(Hdr.Magic);
    swapLEtoBE(Hdr.IpEntry);
    swapLEtoBE(Hdr.ApEntry);

    if (Hdr.Magic != bflang::MAGIC)
        throw std::runtime_error("Bad image magic code");

    for (uint8_t section = 0; section < Hdr.SectionNum; ++section)
        Sections.emplace_back(File);
}

BfppImage::BfppImage(uint8_t _Machine) {
    Hdr.Magic = bflang::MAGIC;
    Hdr.Machine = _Machine;
    Hdr.HeaderSize = sizeof(bflang::BfppImage);
    Hdr.SectionNum = 0;
    Hdr.flags = 0;
    Hdr.IpEntry = 0;
    Hdr.ApEntry = 0;
}

void BfppImage::AddSection(BfppSection &section) {
    Sections.push_back(section);
    Hdr.SectionNum = static_cast<uint8_t>(Sections.size());
}


void BfppImage::Write(std::fstream &File) {
    swapLEtoBE(Hdr.Magic);
    swapLEtoBE(Hdr.IpEntry);
    swapLEtoBE(Hdr.ApEntry);
    Hdr.HeaderSize += sizeof(bflang::BfppSection) * Hdr.SectionNum;
    File.write(reinterpret_cast<char *>(&Hdr), sizeof(bflang::BfppImage));

    swapLEtoBE(Hdr.Magic);
    swapLEtoBE(Hdr.IpEntry);
    swapLEtoBE(Hdr.ApEntry);

    for (auto &section : Sections)
        section.WriteHeader(File);

    for (auto &section : Sections)
        section.WriteData(File);
}

void BfppImage::LoadShared(uint16_t *data) {
    for (uint8_t section = 0; section < SectionNum(); ++section) {
        BfppSection CurrentSection = Section(section);
        for (uint16_t mem = 0; mem < CurrentSection.MemorySize(); ++mem)
            data[CurrentSection.MemoryBase() + mem] = CurrentSection.GetData()[mem];
    }
}
