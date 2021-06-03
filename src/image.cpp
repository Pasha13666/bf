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
    a = static_cast<uint16_t>(a << 8u) | static_cast<uint16_t>(a >> 8u);
}

inline void swapLEtoBEa(void *a, size_t size) {
    for (size_t i = 0; i < size / 2; ++i)
        swapLEtoBE(static_cast<uint16_t *>(a)[i]);
}

#endif

Section::Section(std::vector<Command> &SectionCmd, uint16_t MemoryBase, uint16_t MemorySize) {
    Hdr.MemoryBase = MemoryBase;
    Hdr.MemorySize = MemorySize;
    Hdr.FileSize = static_cast<uint16_t>(SectionCmd.size() * sizeof(uint16_t));
    Hdr.type = binary::SECTION_CODE;

    if (Hdr.FileSize != 0)
        for (auto &cmd : SectionCmd)
            Data.push_back(cmd.GetCmd());
}

Section::Section(std::vector<uint16_t> &SectionData, uint16_t MemoryBase, uint16_t MemorySize) {
    Hdr.MemoryBase = MemoryBase;
    Hdr.MemorySize = MemorySize;
    Hdr.FileSize = static_cast<uint16_t>(SectionData.size() * sizeof(uint16_t));
    Hdr.type = binary::SECTION_DATA;

    if (Hdr.FileSize != 0)
        for (auto &byte : SectionData)
            Data.push_back(byte);
}

Section::Section(std::fstream &File) {
    File.read(reinterpret_cast<char *>(&Hdr), sizeof(binary::Section));
    swapLEtoBEa(&Hdr, sizeof(binary::Section));

    std::streampos Image_pos = File.tellg();

    Data.resize(MemorySize(), 0);
    if (FileSize() != 0) {
        File.seekg(FileBase());
        File.read(reinterpret_cast<char *>(Data.data()), FileSize());
        swapLEtoBEa(Data.data(), FileSize());
    }

    File.seekg(Image_pos);
}

void Section::WriteHeader(std::fstream &File) {
    Hdr.FileBase = static_cast<uint16_t>(File.tellp()); // XXX: ???

    swapLEtoBEa(&Hdr, sizeof(binary::Section));
    File.write(reinterpret_cast<char *>(&Hdr), sizeof(binary::Section));
    swapLEtoBEa(&Hdr, sizeof(binary::Section));
}

void Section::WriteData(std::fstream &File) {
    std::streampos FileBase = File.tellp();
    File.seekg(Hdr.FileBase);
    Hdr.FileBase = static_cast<uint16_t>(FileBase); // XXX: ???

    swapLEtoBEa(&Hdr, sizeof(binary::Section));
    File.write(reinterpret_cast<char *>(&Hdr), sizeof(binary::Section));
    swapLEtoBEa(&Hdr, sizeof(binary::Section));

    File.seekg(FileBase);


    if (Hdr.FileSize != 0) {
        swapLEtoBEa(Data.data(), Hdr.FileSize);
        File.write(reinterpret_cast<char *>(Data.data()), Hdr.FileSize);
        swapLEtoBEa(Data.data(), Hdr.FileSize);
    }
}

Section::Section(uint16_t MemoryBase, uint16_t MemorySize) {
    Hdr.MemoryBase = MemoryBase;
    Hdr.MemorySize = MemorySize;
    Hdr.FileSize = 0;
    Hdr.type = binary::SECTION_DATA;
}

Image::Image(std::fstream &File) {
    File.read(reinterpret_cast<char *>(&Hdr), sizeof(binary::Image));
    swapLEtoBE(Hdr.Magic);
    swapLEtoBE(Hdr.IpEntry);
    swapLEtoBE(Hdr.ApEntry);

    if (Hdr.Magic != binary::MAGIC)
        throw std::runtime_error("Bad image magic code");

    for (uint8_t section = 0; section < Hdr.SectionNum; ++section)
        Sections.emplace_back(File);
}

Image::Image(binary::Machine Machine) {
    Hdr.Magic = binary::MAGIC;
    Hdr.machine = Machine;
    Hdr.HeaderSize = sizeof(binary::Image);
    Hdr.SectionNum = 0;
    Hdr.flags = 0;
    Hdr.IpEntry = 0;
    Hdr.ApEntry = 0;
}

void Image::AddSection(::Section &section) {
    Sections.push_back(section);
    Hdr.SectionNum = static_cast<uint8_t>(Sections.size());
    Hdr.HeaderSize = sizeof(binary::Image) + sizeof(binary::Section) * Hdr.SectionNum;
}


void Image::Write(std::fstream &File) {
    swapLEtoBE(Hdr.Magic);
    swapLEtoBE(Hdr.IpEntry);
    swapLEtoBE(Hdr.ApEntry);

    File.write(reinterpret_cast<char *>(&Hdr), sizeof(binary::Image));

    swapLEtoBE(Hdr.Magic);
    swapLEtoBE(Hdr.IpEntry);
    swapLEtoBE(Hdr.ApEntry);

    for (auto &section : Sections)
        section.WriteHeader(File);

    for (auto &section : Sections)
        section.WriteData(File);
}

void Image::LoadShared(uint16_t *data) {
    for (uint8_t section = 0; section < SectionNum(); ++section) {
        ::Section CurrentSection = GetSection(section);
        for (uint16_t mem = 0; mem < CurrentSection.MemorySize(); ++mem)
            data[CurrentSection.MemoryBase() + mem] = CurrentSection.GetData()[mem];
    }
}
