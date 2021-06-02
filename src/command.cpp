// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//
#include <command.hpp>
#include <stdexcept>

#include "command.hpp"
#include "formats.hpp"


Command::Command(char cmd, bytecode::Bias bias)  {
    switch (cmd) {
        case '<':
            command = bytecode::COMMAND(bytecode::CommandId::ADA, static_cast<bytecode::Bias>(-bias));
            return;
        case '-':
            command = bytecode::COMMAND(bytecode::CommandId::ADD, static_cast<bytecode::Bias>(-bias));
            return;
        case '>':
            command = bytecode::COMMAND(bytecode::CommandId::ADA, bias);
            return;
        case '+':
            command = bytecode::COMMAND(bytecode::CommandId::ADD, bias);
            return;
        case '[':
            command = bytecode::COMMAND(bytecode::CommandId::JZ, bias);
            return;
        case ']':
            command = bytecode::COMMAND(bytecode::CommandId::JNZ, bias);
            return;
        case 'N':
            command = COMMAND_C(bytecode::CtrlioBits::NOP);
            return;
        case '.':
            command = COMMAND_C(bytecode::CtrlioBits::COUT);
            return;
        case 'C':
            command = COMMAND_C(bytecode::CtrlioBits::CIN);
            return;
        case ',':
            command = COMMAND_C(bytecode::CtrlioBits::SYNC);
            return;
        case 'D':
            command = COMMAND_C(bytecode::CtrlioBits::CLR_DATA);
            return;
        case 'A':
            command = COMMAND_C(bytecode::CtrlioBits::CLR_AP);
            return;
        case 'L':
            command = COMMAND_C(bytecode::CtrlioBits::M8);
            return;
        case 'F':
            command = COMMAND_C(bytecode::CtrlioBits::M16);
            return;
        case 'H':
            command = COMMAND_C(bytecode::CtrlioBits::HALT);
            return;
        default:
            throw std::runtime_error("Unknown command");
    }
}

char Command::CmdChar() {
    switch (bytecode::COMMAND_ID(command)){
        case bytecode::CommandId::ADD:
            return '+';
        case bytecode::CommandId::ADA:
            return '>';
        case bytecode::CommandId::JZ:
            return '[';
        case bytecode::CommandId::JNZ:
            return ']';

        case bytecode::CommandId::CTRLIO:
            switch (bytecode::CTRLIO_BIT(bytecode::COMMAND_BIAS(command))) {
                case bytecode::CtrlioBits::NOP:
                    return 'N';
                case bytecode::CtrlioBits::COUT:
                    return '.';
                case bytecode::CtrlioBits::CIN:
                    return 'C';
                case bytecode::CtrlioBits::SYNC:
                    return ',';
                case bytecode::CtrlioBits::CLR_DATA:
                    return 'D';
                case bytecode::CtrlioBits::CLR_AP:
                    return 'A';
                case bytecode::CtrlioBits::M8:
                    return 'L';
                case bytecode::CtrlioBits::M16:
                    return 'F';
                case bytecode::CtrlioBits::HALT:
                    return 'H';
            }
    }
}
