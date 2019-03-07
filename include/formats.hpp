// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_FORMATS_HPP
#define BF_FORMATS_HPP

#include <cstdint>

#define SWAP_ENDIANNESS 1

namespace bytecode {
    using Command = uint16_t;
    using Bias = uint16_t; // (u)int12_t

    enum class CommandId : uint16_t {
        CTRLIO = 0b000,
        ADD    = 0b001,
        ADA    = 0b010,
        JZ     = 0b011,
        JNZ    = 0b100
                // 5..6 free
    };

    enum class CtrlioBits {
        NOP = -1,
        // 0 free
        COUT = 1,
        CIN = 2,
        SYNC = 3, // Synchronous input - CIN | SYNC
        CLR_DATA = 4,
        CLR_AP = 5,
        // 6..8
        M8 = 9,
        M16 = 10,
        HALT = 11,
    };


    // Commands

    constexpr inline CommandId COMMAND_ID(Command cmd){
        return static_cast<CommandId>(cmd >> 13);
    }

    constexpr inline Bias COMMAND_BIAS(Command cmd){
        return static_cast<Bias>((cmd & 0xFFF) | ((cmd & 0x1000)? 0xF000: 0));
    }

    constexpr inline Command COMMAND(CommandId id, Bias bias){
        return static_cast<Command>((static_cast<Command>(id) << 13) | (bias & 0x1FFF));
    }


    // Ctrlio

    constexpr inline Bias CTRLIO_BIAS(CtrlioBits bit){
        if (bit == CtrlioBits::NOP)
            return 0;
        return static_cast<Bias>((1 << static_cast<Bias>(bit)) | (1 << 12));
    }

    constexpr inline CtrlioBits CTRLIO_BIT(Bias bias){
        if ((bias & (1U << 12)) == 0)
            return CtrlioBits::NOP;

        for (int i = 0; i < 12; ++i)
            if (bias & (1U << i))
                return static_cast<CtrlioBits>(i);

        return CtrlioBits::NOP;
    }

    constexpr inline Command COMMAND_C(CtrlioBits bit){
        return COMMAND(CommandId::CTRLIO, CTRLIO_BIAS(bit));
    }


    // Names

    const char *const CMD_NAMES[] = {
            "CIO", "ADD", "ADA", "JZ", "JNZ", "u5", "u6"
    };

    const char *const CTRLIO_BITS_NAMES[] = {
            "u0", "COUT", "CIN", "SYNC", "CLR_DATA", "CLR_AP", "u6", "u7", "u8", "M8", "M16", "HALT"
    };

}

namespace binary {
    // Bfpp binary file
    const uint16_t MAGIC = 0x4246;
    const uint16_t INDENT = 0x10;
    const uint16_t MEMORY_PTR_MAX = 65535;

    enum SectionType : uint8_t {
        SECTION_CODE = 1,
        SECTION_DATA = 2
    };

    enum Machine : uint8_t {
        MACHINE_16BIT = 0,
        MACHINE_8BIT = 1
    };

    struct Image {
        uint16_t Magic;
        Machine machine;
        uint8_t HeaderSize; // sizeof(Image) + sizeof(Section) * SectionNum
        uint8_t SectionNum;
        uint8_t flags; // = 0
        uint16_t IpEntry; //Initial IP address
        uint16_t ApEntry; //Initial AP address
    };

    struct Section{
        uint16_t FileBase; // From the beginning of file
        uint16_t MemoryBase; // Physical address to load
        uint16_t FileSize; // How much Bytes of data can be loaded to memory
        uint16_t MemorySize; // How much Words in Memory should be allocated
        SectionType type;
        uint8_t _pad;
    };
}

#endif //BF_FORMATS_HPP
