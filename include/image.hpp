// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_IMAGE_HPP
#define BF_IMAGE_HPP

#include <fstream>
#include <vector>
#include <memory>
#include "formats.hpp"
#include "command.hpp"

class BfppSection {
public:
    BfppSection(std::vector<Command> &SectionCmd, uint16_t MemoryBase, uint16_t MemorySize);
    BfppSection(std::vector<uint16_t> &SectionData, uint16_t MemoryBase, uint16_t MemorySize);

    explicit BfppSection(std::fstream &File);

    uint8_t Type(){return Hdr.type;};
    uint16_t FileBase(){return Hdr.FileBase;};
    uint16_t MemoryBase(){return Hdr.MemoryBase;};
    uint16_t FileSize(){return Hdr.FileSize;};
    uint16_t MemorySize(){return Hdr.MemorySize;};

    void WriteHeader(std::fstream &File);
    void WriteData(std::fstream &File);

    uint16_t *GetData(){return Data.data();};

private:
    bflang::BfppSection Hdr;

    std::vector<uint16_t> Data;
};

class BfppImage {
public:
    explicit BfppImage(std::fstream &File);
    explicit BfppImage(uint8_t _machine);

    ~BfppImage() = default;

    void AddSection(BfppSection &section);
    uint8_t SectionNum(){return Hdr.SectionNum;};
    BfppSection &Section(uint8_t section){return Sections[section];};

    void IpEntry(uint16_t Ptr){Hdr.IpEntry = Ptr;};
    void ApEntry(uint16_t Ptr){Hdr.ApEntry = Ptr;};
    void Machine(uint8_t Ptr){Hdr.Machine = Ptr;};
    void Flags(uint8_t Ptr){Hdr.flags = Ptr;};

    uint8_t Flags(){return Hdr.flags;};
    uint8_t Machine(){return Hdr.Machine;};
    uint16_t IpEntry(){return Hdr.IpEntry;};
    uint16_t ApEntry(){return Hdr.ApEntry;};

    void Write(std::fstream &File);
    void LoadShared(uint16_t *data);

private:
    bflang::BfppImage Hdr;

    std::vector<BfppSection> Sections;
};

#endif //BF_IMAGE_HPP
