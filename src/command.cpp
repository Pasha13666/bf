// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//
#include "command.hpp"
#include "formats.hpp"


Command::Command(uint16_t _bin){
    bias = bflang::COMMAND_BIAS(_bin);
    switch (bflang::COMMAND_ID(_bin)){
        case bflang::ADD:
            cmd = '+';
            break;
        case bflang::ADA:
            cmd = '>';
            break;
        case bflang::JZ:
            cmd = '[';
            break;
        case bflang::JNZ:
            cmd = ']';
            break;

        case bflang::CTRLIO:
            cmd = ' ';
            if (bflang::IS_NOP(static_cast<uint16_t>(bias)))
                return;
            if (bias & bflang::CIN)
                cmd = ',';
            if (bias & bflang::COUT)
                cmd = '.';
            if (bias & bflang::CLR_AP)
                cmd = 'A';
            if (bias & bflang::CLR_DATA)
                cmd = 'D';
            if (bias & bflang::M16)
                cmd = 'F';
            if (bias & bflang::M8)
                cmd = 'L';
            if (bias & bflang::CHALT)
                cmd = 'H';
        default:break;
    }
}

Command::Command(char _cmd) : cmd(_cmd), bias(0){
    fixBias();
}

Command::Command(char _cmd, int16_t _bias) : cmd(_cmd), bias(_bias) {
    switch (cmd){
        case '<':
            bias = -bias;
            cmd = '>';
            break;
        case '-':
            bias = -bias;
            cmd = '+';
            break;
        default:break;
    }
    fixBias();
}

Command::Command(char _cmd, std::string label) : cmd(_cmd), label(std::move(label)) {
    fixBias();
}

uint16_t Command::Id(){
    switch (cmd){
        case '>':
            return bflang::ADA;
        case '+':
            return bflang::ADD;
        case '[':
            return bflang::JZ;
        case ']':
            return bflang::JNZ;
        default:
            return bflang::CTRLIO;
    }
}

uint16_t Command::GetCmd(){
    return bflang::COMMAND(Id(), static_cast<uint16_t>(Bias() & 0x1FFF));
}

void Command::fixBias() {
    switch (cmd){
        case 'D':
            bias = bflang::CLR_DATA | 0x1000;
            break;
        case 'A':
            bias = bflang::CLR_AP | 0x1000;
            break;
        case 'L':
            bias = bflang::M8 | 0x1000;
            break;
        case 'F':
            bias = bflang::M16 | 0x1000;
            break;
        case 'S':
            bias = bflang::SYNC | 0x1000;
            break;
        case 's':
            bias = bflang::CIN | 0x1000;
            break;
        case 'H':
            bias = bflang::CHALT | 0x1000;
            break;
        case '.':
            bias = bflang::COUT | 0x1000;
            break;
        case ',':
            bias = bflang::CIN | bflang::SYNC | 0x1000;
            break;
        default:break;
    }
}
