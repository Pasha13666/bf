// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_IMAGE_HPP
#define BF_IMAGE_HPP

#include <fstream>
#include <vector>
#include "formats.hpp"
#include "command.hpp"

class Section {
public:
    Section(std::vector<Command> &SectionCmd, uint16_t MemoryBase, uint16_t MemorySize);
    Section(std::vector<uint16_t> &SectionData, uint16_t MemoryBase, uint16_t MemorySize);
    Section(uint16_t MemoryBase, uint16_t MemorySize);

    explicit Section(std::fstream &File);

    [[nodiscard]] uint8_t Type() const {return Hdr.type;};
    [[nodiscard]] uint16_t FileBase() const {return Hdr.FileBase;};
    [[nodiscard]] uint16_t MemoryBase() const {return Hdr.MemoryBase;};
    [[nodiscard]] uint16_t FileSize() const {return Hdr.FileSize;};
    [[nodiscard]] uint16_t MemorySize() const {return Hdr.MemorySize;};

    void WriteHeader(std::fstream &File);
    void WriteData(std::fstream &File);

    uint16_t *GetData(){return Data.data();};

private:
    binary::Section Hdr {};

    std::vector<uint16_t> Data {};
};

class Image {
public:
    explicit Image(std::fstream &File);
    explicit Image(binary::Machine Machine);

    ~Image() = default;

    void AddSection(Section &section);
    [[nodiscard]] uint8_t SectionNum() const {return Hdr.SectionNum;};
    Section &GetSection(uint8_t section){return Sections[section];};

    void IpEntry(uint16_t Ptr){Hdr.IpEntry = Ptr;};
    void ApEntry(uint16_t Ptr){Hdr.ApEntry = Ptr;};
    void Machine(binary::Machine Ptr){Hdr.machine = Ptr;};
    void Flags(uint8_t Ptr){Hdr.flags = Ptr;};

    [[nodiscard]] uint8_t Flags() const {return Hdr.flags;};
    [[nodiscard]] binary::Machine Machine() const {return Hdr.machine;};
    [[nodiscard]] uint16_t IpEntry() const {return Hdr.IpEntry;};
    [[nodiscard]] uint16_t ApEntry() const {return Hdr.ApEntry;};

    void Write(std::fstream &File);
    void LoadShared(uint16_t *data);

private:
    binary::Image Hdr {};

    std::vector<::Section> Sections {};
};

#endif //BF_IMAGE_HPP
