// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_FORMATS_HPP
#define BF_FORMATS_HPP

#include <cstdint>

#define SWAP_ENDIANNESS 1

namespace bflang {
    const char *const CMD_NAMES[] = {
            "CIO", "ADD", "ADA", "JZ", "JNZ", "u5", "u6"
    };

    const char *const CTRLIO_CMD_NAMES[] = {
            "u0", "COUT", "CIN", "SYNC", "CLR_DATA", "CLR_AP", "u6", "u7", "u8", "M8", "M16", "HALT"
    };

    // Commands
    constexpr inline uint16_t COMMAND_ID(uint16_t cmd){
        return cmd >> 13;
    }

    constexpr inline uint16_t COMMAND_BIAS(uint16_t cmd){
        return static_cast<uint16_t>((cmd & 0xFFF) | ((cmd & 0x1000)? 0xF000: 0));
    }

    constexpr inline uint16_t COMMAND(uint16_t id, uint16_t bias){
        return static_cast<uint16_t>((id << 13) | (bias & 0x1FFF));
    }

    constexpr inline bool IS_NOP(uint16_t bias){
        return (bias & (1U << 12)) == 0;
    }

    const uint16_t CTRLIO = 0b000;
    const uint16_t ADD    = 0b001;
    const uint16_t ADA    = 0b010;
    const uint16_t JZ     = 0b011;
    const uint16_t JNZ    = 0b100; // 5..6 free

    // CTRLIO bits
    const int COUT = 1U << 1;
    const int CIN = 1U << 2;
    const int SYNC = 1U << 3; // Synchronous input - CIN | SYNC
    const int CLR_DATA = 1U << 4;
    const int CLR_AP = 1U << 5;
    const int M8 = 1U << 9;
    const int M16 = 1U << 10;
    const int CHALT = 1U << 11;

    // Bfpp binary file
    const uint16_t MAGIC = 0x4246;
    const uint16_t INDENT = 0x10;
    const uint16_t MEMORY_PTR_MAX = 65535;
    const uint16_t SECTION_CODE = 1;
    const uint16_t SECTION_DATA = 2;
    struct BfppImage {
        uint16_t Magic; //BF should contain "BF" symbols
        uint8_t Machine; //8bit of 16bit mode
        uint8_t HeaderSize;
        uint8_t SectionNum;
        uint8_t flags;
        uint16_t IpEntry;//IP start address
        uint16_t ApEntry;//AP start address
    };

    struct BfppSection{
        uint16_t FileBase; //Relative shift from the beginning of file
        uint16_t MemoryBase; //Physical address to load
        uint16_t FileSize; //How much Bytes of data can be loaded to memory
        uint16_t MemorySize; //How much Words in Memory should be allocated
        uint8_t type;
        uint8_t pad;
    };
}

#endif //BF_FORMATS_HPP
