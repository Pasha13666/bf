// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//
#include "compiler.hpp"
#include "assembly.hpp"
#include <iomanip>

inline bool IsJoinable(char cmd) {
    return cmd == '+' || cmd == '-' || cmd == '<' || cmd == '>';
}

inline bool IsSymbol(char ch) {
    return (ch == '<') || (ch == '>') || (ch == '+') || (ch == '-')
           || (ch == '[') || (ch == ']') || (ch == '.') || (ch == ',');
}

void Compiler::Translate(std::istream &in, std::vector<Command> &output) {
    output.emplace_back('>', 0x0800);

    size_t CurrentBias = 0;
    int cmd, next_cmd;
    cmd = in.get();
    while (cmd != -1) {
        next_cmd = in.get();

        if (!IsSymbol(static_cast<char>(cmd))) {
            cmd = next_cmd;
            continue;
        }

        if (IsJoinable(static_cast<char>(cmd))) {
            CurrentBias++;
            if (!optimizeJoining || cmd != next_cmd) {
                output.emplace_back((char) cmd, CurrentBias);
                CurrentBias = 0;
            }
        } else {
            output.emplace_back((char) cmd);
            CurrentBias = 0;
        }

        cmd = next_cmd;
    }

    output.emplace_back('H', 0x00);
}

void Compiler::Linking(std::vector<Command> &output) {
    size_t CurrentIp = 0;
    size_t MaxIp = output.size();

    for (CurrentIp = 0; CurrentIp < MaxIp; CurrentIp++)
        if (output[CurrentIp].CmdChar() == '[' && output[CurrentIp].Bias() == 0)
            FindLoopEnd(output, CurrentIp);
}

void Compiler::FindLoopEnd(std::vector<Command> &output, size_t CurrentIp) {
    size_t i = 1;
    size_t NewIp = CurrentIp;
    size_t MaxIp = output.size();
    while (i) {
        NewIp++;
        if (NewIp >= MaxIp)
            throw std::runtime_error("Unclosed loop");

        switch (output[NewIp].CmdChar()) {
            case '[':
                i++;
                break;
            case ']':
                i--;
                break;
            default:break;
        }
    }

    //We found loop ending:
    output[NewIp].Bias(static_cast<bytecode::Bias>(CurrentIp - NewIp));
    output[CurrentIp].Bias(static_cast<bytecode::Bias>(NewIp - CurrentIp));
}

void Compiler::OptimizeClear(std::vector<Command> &output) {
    for (auto i = output.begin(); i != output.end(); i++) {
        if (i->CmdChar() == '['
            && output.end() - i >= 2
            && (i + 1)->CmdChar() == '+'
            && (i + 2)->CmdChar() == ']') {

            output.erase(i, i + 3);
            output.insert(i, Command{'D'});
        }
    }
}

void Compiler::Compile(std::fstream &in, Compiler::Format inf, std::fstream &out, Compiler::Format of) {
    if (inf == Format::HEX)
        throw std::runtime_error("Cannot use hex file as input");

    if (inf == Format::IMAGE && of != Format::IMAGE)
        throw std::runtime_error("Image can be converted only to image");


    if (inf == Format::IMAGE) {
        Image img(in);
        img.Write(out);
        return;
    }

    std::vector<Command> commands;
    commands.emplace_back('N');

    if (inf == Format::ASSEMBLY) {
        AssemblyParser p {};
        p.Parse(commands, in);
    } else Translate(in, commands);

    if (optimizeClearing)
        OptimizeClear(commands);
    Linking(commands);

    if (of == Format::SOURCE) {
        WriteSource(commands, out);
        return;
    }

    if (of == Format::ASSEMBLY) {
        WriteAssembly(commands, out);
        return;
    }

    if (of == Format::HEX) {
        WriteHex(commands, out);
        return;
    }

    Image img {binary::Machine::MACHINE_16BIT};
    Section code(commands, 0, static_cast<uint16_t>(commands.size() * 2));
    img.AddSection(code);
    img.Write(out);
}

void Compiler::WriteSource(std::vector<Command> &output, std::fstream &out) {
    for (auto i : output) {
        char ch = i.CmdChar();
        switch (i.Id()) {
            case bytecode::CommandId::CTRLIO:
            case bytecode::CommandId::JZ:
            case bytecode::CommandId::JNZ:
                out << ch;
                break;

            default:
                int16_t bias = i.Bias();
                if (bias < 0) {
                    switch (ch) {
                        case '+':
                            ch = '-';
                            break;
                        case '>':
                            ch = '<';
                            break;
                        default:break;
                    }
                    bias = -bias;
                }

                for (int j = 0; j < bias; ++j)
                    out << ch;
        }
    }
}

void Compiler::WriteHex(std::vector<Command> &output, std::fstream &out) {
    size_t ip = 0;
    size_t align = 0;

    out << "uint16_t application[] = {\n";
    for (auto &cmd : output) {
        if (ip != 0)
            out << ", ";

        if (align == 10) {
            out << "\n";
            align = 0;
        }
        out << "0x" << std::setfill('0') << std::setw(4) << std::hex << cmd.GetCmd();
        align++;
        ip++;
    }
    out << "};";
}

