// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_COMMAND_HPP
#define BF_COMMAND_HPP

#include <cstdint>
#include <string>

class Command {
private:
    char cmd;
    int16_t bias;
    std::string label;

    void fixBias();

public:
    explicit Command(uint16_t _bin);
    explicit Command(char _cmd);
    Command (char _cmd, int16_t _bias);
    Command (char _cmd, std::string label);

    void Bias(int16_t _bias){bias = _bias;};
    int16_t Bias(){return bias;};

    void Label(std::string l){label = std::move(l);};
    std::string Label(){return label;};

    char CmdChar(){return cmd;};
    uint16_t GetCmd();
    uint16_t Id();

};

#endif //BF_COMMAND_HPP
