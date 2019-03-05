// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#include "emulator.hpp"

Emulator::Emulator(const BfppImage &img) : image(img) {
    wordMode = image.Machine() == 1; // TODO: WordMode
    ip = image.IpEntry();
    ap = image.ApEntry();

    memory.fill(0);
    image.LoadShared(memory.data());
}

void Emulator::Run() {
    auto jumpMask = static_cast<uint16_t>(wordMode? 0xFFFF: 0xFF);
    unsigned long long executedInstructions = 0;
    if (debug) {
        fprintf(stderr, "Sections: \n");
        for (uint8_t i = 0; i < image.SectionNum(); ++i) {
            BfppSection &s = image.Section(i);
            fprintf(stderr, "%s: 0x%.4x - 0x%.4x\n", s.Type() == bflang::SECTION_CODE? "Code": "Data", s.MemoryBase(),
                    s.MemoryBase() + s.MemorySize());
        }
        fprintf(stderr, "WordMode = %i, ProtectedMode = %i, starting emulator...\n", wordMode, protectedMode);
        fprintf(stderr, " IP   AP  *AP    CMD  BIAS\n");
        fprintf(stderr, "%.4x %.4x %.4x\n", ip, ap, memory[ap]);
    }

    while (true) {
        ip++; // Note this
        uint16_t cmd = memory[ip];
        uint16_t bias = bflang::COMMAND_BIAS(cmd);
        uint16_t id = bflang::COMMAND_ID(cmd);

        if (protectedMode) {
            for (uint8_t i = 0; i < image.SectionNum(); ++i) {
                auto &s = image.Section(i);
                if (s.Type() == bflang::SECTION_CODE && ip >= s.MemoryBase() && ip < s.MemoryBase() + s.MemorySize())
                    goto ip_ok;
            }
            fprintf(stderr, "Memory access violation: trying to execute data.\n");
            goto end;
ip_ok:
            for (uint8_t i = 0; i < image.SectionNum(); ++i) {
                auto &s = image.Section(i);
                if (s.Type() == bflang::SECTION_CODE && ap >= s.MemoryBase() && ap < s.MemoryBase() + s.MemorySize()) {
                    fprintf(stderr, "Memory access violation: AP points to code.\n");
                    fprintf(stderr, "Note: Ignore this error if program works normally without --protected option.\n");
                    goto end;
                }
            }
        }

        if (debug) {
            fprintf(stderr, "%.4x %.4x %.4x   %-4s", ip, ap, memory[ap], bflang::CMD_NAMES[id]);

            if (id != bflang::CTRLIO)
                fprintf(stderr, " %4i", *reinterpret_cast<int16_t*>(&bias));
            else if (bflang::IS_NOP(bias))
                fprintf(stderr, " NOP");
            else
                for (int i = 0; i < 12; ++i)
                    if (bias & (1U << i))
                        fprintf(stderr, " %s", bflang::CTRLIO_CMD_NAMES[i]);
            fprintf(stderr, "\n");
        }

        switch (id) {
            case bflang::CTRLIO:
                if (bflang::IS_NOP(bias))
                    break;

                if (bias & bflang::COUT)
                    putchar(memory[ap] & 0xFF);

                if (bias & bflang::CIN || bias & bflang::SYNC)
                    memory[ap] = getchar() & 0xFF;

                if (bias & bflang::CLR_DATA)
                    memory[ap] = 0;

                if (bias & bflang::CLR_AP)
                    ap = 0;

                if (bias & bflang::M8) {
                    wordMode = false;
                    jumpMask = 0xff;
                }

                if (bias & bflang::M16) {
                    wordMode = true;
                    jumpMask = 0xffff;
                }

                if (bias & bflang::CHALT)
                    goto end;

                break;
            case bflang::ADD:
                memory[ap] += bias;
                break;
            case bflang::ADA:
                ap += bias;
                break;
            case bflang::JZ:
                if ((memory[ap] & jumpMask) == 0)
                    ip += bias;
                break;
            case bflang::JNZ:
                if ((memory[ap] & jumpMask) != 0)
                    ip += bias;
                break;
            default:
                throw std::runtime_error("Unknown opcode");
        }

        executedInstructions++;
    }

end:;
    if (statistics) {
        fprintf(stderr, "Executed %llu instructions.\n", executedInstructions);
    }
}

