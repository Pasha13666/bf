// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 04.03.19.
//
#include <sstream>
#include "compiler.hpp"

const char *const spaces = " \n\r\t";

void Compiler::WriteAssembly(std::vector<Command> &output, std::fstream &out) {

}

void Compiler::ReadAssembly(std::vector<Command> &output, std::fstream &in) {
    std::string line;

    while (std::getline(in, line)) {
        line.erase(line.find_last_not_of(spaces) + 1, line.find_first_not_of(spaces));

        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss{line};
        std::string cmd;
        ss >> cmd;
        uint16_t id, bias;
        bool neg = false;

        if (cmd == "ADD")
            id = bflang::ADD;
        else if (cmd == "ADA")
            id = bflang::ADA;
        else if (cmd == "JZ")
            id = bflang::JZ;
        else if (cmd == "JNZ")
            id = bflang::JNZ;

        else if (cmd == "SUB") {
            id = bflang::ADD;
            neg = true;
        } else if (cmd == "SUBA") {
            id = bflang::ADA;
            neg = true;
        } else if (cmd == "JZN") {
            id = bflang::JZ;
            neg = true;
        } else if (cmd == "JNZN") {
            id = bflang::JNZ;
            neg = true;

        } else if (cmd == "LOOP" || cmd == "[") {
            output.emplace_back('[');
            continue;

        } else if (cmd == "END" || cmd == "]") {
            output.emplace_back(']');
            continue;

        } else if (cmd == "NOP") {
            output.emplace_back(bflang::COMMAND(bflang::CTRLIO, 0));
            continue;

        } else {
            if (cmd == "HALT")
                bias = bflang::CHALT;
            else if (cmd == "CIN")
                bias = bflang::CIN;
            else if (cmd == "COUT")
                bias = bflang::COUT;
            else if (cmd == "M8")
                bias = bflang::M8;
            else if (cmd == "M16")
                bias = bflang::M16;
            else if (cmd == "CLR_AP")
                bias = bflang::CLR_AP;
            else if (cmd == "CLR_DATA")
                bias = bflang::CLR_DATA;
            else if (cmd == "SYNC")
                bias = bflang::SYNC;
            else if (cmd == "SCIN")
                bias = bflang::SYNC | bflang::CIN;
            else throw std::runtime_error("Unknown command");

            output.emplace_back(bflang::COMMAND(bflang::CTRLIO, static_cast<uint16_t>(bias | 0x1000)));
            continue;
        }

        if (ss.eof())
            bias = 1;
        else {
            ss >> cmd;
            bias = std::stoi(cmd, nullptr, 0);
        }
        if (neg) bias = static_cast<uint16_t>(-bias);
        output.emplace_back(bflang::COMMAND(id, bias));
    }
}
