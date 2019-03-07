// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_COMMAND_HPP
#define BF_COMMAND_HPP

#include <cstdint>
#include <string>
#include "formats.hpp"

class Command {
private:
    bytecode::Command command;
    std::string label;

public:
    Command (char cmd, bytecode::Bias bias);

    explicit Command(bytecode::Command command) : command(command) {}
    explicit Command(char _cmd) : Command(_cmd, 0) {}
    Command (char _cmd, std::string label) : Command(_cmd, 0) {
        this->label = std::move(label);
    }

    char CmdChar();

    void Label(std::string l){label = std::move(l);};
    std::string Label(){return label;};

    void Bias(bytecode::Bias bias){
        command = bytecode::COMMAND(bytecode::COMMAND_ID(command), bias);
    };

    bytecode::Bias Bias(){ return bytecode::COMMAND_BIAS(command); }
    bytecode::CommandId Id() { return bytecode::COMMAND_ID(command); }
    bytecode::Command GetCmd() { return command; }

};

#endif //BF_COMMAND_HPP
