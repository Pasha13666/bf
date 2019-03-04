// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#include <compiler.hpp>

inline bool IsJoinable(char cmd) {
    return cmd == '+' || cmd == '-' || cmd == '<' || cmd == '>';
}

void Compiler::Translate(std::istream &in, std::vector<Command> &output) {
    //output.emplace_back(' ', 0);
    //output.emplace_back('>', 0x0800);

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

bool Compiler::IsSymbol(char ch) {
    return (ch == '<') || (ch == '>') || (ch == '+') || (ch == '-')
           || (ch == '[') || (ch == ']') || (ch == '.') || (ch == ',')
           || (extendedCommands && ((ch == 'A') || (ch == 'D') || (ch == 'L') || (ch == 'F')
                                    || (ch == 'H') || (ch == 'S') || (ch == '~')));
}

void Compiler::Linking(std::vector<Command> &output) {
//Let's Get Pointers for loops:
    //[ should get shift > 0
    //] should get shift < 0
    size_t CurrentIp = 0;
    size_t MaxIp = output.size();

    for (CurrentIp = 0; CurrentIp < MaxIp; CurrentIp++)
        if (output[CurrentIp].CmdChar() == '[')
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
    output[NewIp].Bias(static_cast<int16_t>(CurrentIp - NewIp));
    output[CurrentIp].Bias(static_cast<int16_t>(NewIp - CurrentIp));
}

void Compiler::OptimizeClear(std::vector<Command> &output) {
    for (auto i = output.begin(); i != output.end(); i++) {
        if (optimizeClearing
            && i->CmdChar() == '['
            && output.end() - i >= 2
            && (i + 1)->CmdChar() == '+'
            && (i + 2)->CmdChar() == ']') {

            output.erase(i, i + 3);
            output.insert(i, Command{'D'});
        }

        if (optimizeJoiningCtrlio
            && bflang::COMMAND_ID(i->GetCmd()) == bflang::CTRLIO) {
            // TODO: Join ctrlio commands
        }
    }
}

void Compiler::Compile(std::fstream &in, Compiler::Format inf, std::fstream &out, Compiler::Format of) {
    if (inf == Format::HEX)
        throw std::runtime_error("Cannot use hex file as input");

    if (inf == Format::ASSEMBLY)
        throw std::runtime_error("Reading assembly is not implemented");

    if (inf == Format::IMAGE && of != Format::IMAGE)
        throw std::runtime_error("Image can be converted only to image");


    if (inf == Format::IMAGE) {
        BfppImage img(in);
        img.Write(out);
        return;
    }

    std::vector<Command> commands;

    Translate(in, commands);
    if (optimizeClearing || optimizeJoining)
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

    BfppImage img(0);
    BfppSection code(commands, 0x800, static_cast<uint16_t>(commands.size() * 2));
    img.IpEntry(0x800);
    img.AddSection(code);
    img.Write(out);
}

void Compiler::WriteSource(std::vector<Command> &output, std::fstream &out) {
    for (auto i : output) {
        char ch = i.CmdChar();
        switch (i.Id()) {
            case bflang::CTRLIO:
            case bflang::HALT:
            case bflang::JZ:
            case bflang::JNZ:
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

}

void Compiler::WriteAssembly(std::vector<Command> &output, std::fstream &out) {

}

