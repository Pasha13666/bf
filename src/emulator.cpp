// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#include "emulator.hpp"

Emulator::Emulator(const Image &img) : image(img) {
    machine = image.Machine();
    ip = image.IpEntry();
    ap = image.ApEntry();

    memory.fill(0);
    image.LoadShared(memory.data());
}

void Emulator::Run() {
    auto jumpMask = static_cast<uint16_t>(machine == binary::MACHINE_16BIT? 0xFFFF: 0xFF);
    unsigned long long executedInstructions = 0;
    if (debug) {
        fprintf(stderr, "Sections: \n");
        for (uint8_t i = 0; i < image.SectionNum(); ++i) {
            Section &s = image.GetSection(i);
            fprintf(stderr, "%s: 0x%.4x - 0x%.4x\n", s.Type() == binary::SECTION_CODE? "Code": "Data", s.MemoryBase(),
                    s.MemoryBase() + s.MemorySize());
        }
        fprintf(stderr, "WordMode = %i, ProtectedMode = %i, starting emulator...\n", machine, protectedMode);
        fprintf(stderr, " IP   AP  *AP    CMD  BIAS\n");
        fprintf(stderr, "%.4x %.4x %.4x\n", ip, ap, memory[ap]);
    }

    while (true) {
        ip++; // Note this
        uint16_t cmd = memory[ip];
        uint16_t bias = bytecode::COMMAND_BIAS(cmd);
        auto id = bytecode::COMMAND_ID(cmd);

        if (protectedMode) {
            for (uint8_t i = 0; i < image.SectionNum(); ++i) {
                auto &s = image.GetSection(i);
                if (s.Type() == binary::SECTION_CODE && ip >= s.MemoryBase() && ip < s.MemoryBase() + s.MemorySize())
                    goto ip_ok;
            }
            throw std::runtime_error("Memory access violation: trying to execute data");
ip_ok:;
        }

        if (debug) {
            fprintf(stderr, "%.4x %.4x %.4x   %-4s", ip, ap, memory[ap], bytecode::CMD_NAMES[(int)id]);

            if (id == bytecode::CommandId::CTRLIO)
                fprintf(stderr, " %s\n", bytecode::CTRLIO_BITS_NAMES[(int) bytecode::CTRLIO_BIT(bias)]);
            else fprintf(stderr, " %4i\n", *reinterpret_cast<int16_t *>(&bias));
        }

        switch (id) {
            case bytecode::CommandId::CTRLIO:

                switch (bytecode::CTRLIO_BIT(bias)) {
                    case bytecode::CtrlioBits::NOP:break;

                    case bytecode::CtrlioBits::COUT:
                        if (protectedMode) checkAp();
                        putchar(memory[ap] & 0xFF);
                        break;

                    case bytecode::CtrlioBits::CIN:
                        if (protectedMode) checkAp();
                        memory[ap] = getchar() & 0xFF;
                        break;

                    case bytecode::CtrlioBits::SYNC:
                        if (protectedMode) checkAp();
                        memory[ap] = getchar() & 0xFF;
                        break;

                    case bytecode::CtrlioBits::CLR_DATA:
                        if (protectedMode) checkAp();
                        memory[ap] = 0;
                        break;

                    case bytecode::CtrlioBits::CLR_AP:
                        ap = 0;
                        break;

                    case bytecode::CtrlioBits::M8:
                        machine = binary::MACHINE_8BIT;
                        jumpMask = 0xff;
                        break;

                    case bytecode::CtrlioBits::M16:
                        machine = binary::MACHINE_16BIT;
                        jumpMask = 0xffff;
                        break;

                    case bytecode::CtrlioBits::HALT:
                        goto end;
                }
                break;
            case bytecode::CommandId::ADD:
                if (protectedMode) checkAp();
                memory[ap] += bias;
                break;
            case bytecode::CommandId::ADA:
                ap += bias;
                break;
            case bytecode::CommandId::JZ:
                if (protectedMode) checkAp();
                if ((memory[ap] & jumpMask) == 0)
                    ip += bias;
                break;
            case bytecode::CommandId::JNZ:
                if (protectedMode) checkAp();
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

void Emulator::checkAp() {
    for (uint8_t i = 0; i < image.SectionNum(); ++i) {
        auto &s = image.GetSection(i);
        if (s.Type() == binary::SECTION_DATA && ap >= s.MemoryBase() && ap < s.MemoryBase() + s.MemorySize())
            return;
    }
    throw std::runtime_error("Memory access violation: AP outs of data");
}

