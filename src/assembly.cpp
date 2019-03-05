// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 04.03.19.
//
#include "assembly.hpp"
#include "compiler.hpp"
#include <cstring>
#include <algorithm>
#include <fstream>


const AssemblyParser::Command AssemblyParser::commands[] = {
        {"add",     '+', false},
        {"ada",     '>', false},

        {"sub",     '+', true},
        {"suba",    '>', true},
        {"sua",     '>', true},
        {"jzn",     '[', true},
        {"jnzn",    ']', true},

        {"cin",     's'},
        {"cout",    '.'},
        {"halt",    'H'},
        {"m8",      'L'},
        {"m16",     'F'},
        {"clr_ap",  'A'},
        {"clr_data", 'D'},
        {"sync",    'S'},
        {"scin",    ','},
        {"nop",     ' '},
        {"loop",    '['},
        {"endl",    ']'},
        {"[",       '['},
        {"]",       ']'},

        {"label",   &AssemblyParser::do_label},
        {"jz",      &AssemblyParser::do_jz},
        {"jnz",     &AssemblyParser::do_jnz},
};


void AssemblyParser::do_label(std::vector<::Command> &output, std::vector<const char*> args, size_t lineno) {
    if (args.size() != 2) parseError("Extra characters at end of line ", lineno);

    labels[args[1]] = output.size();
}


void AssemblyParser::do_jz(std::vector<::Command> &output, std::vector<const char*> args, size_t lineno) {
    if (args.size() != 2) parseError("Extra characters at end of line ", lineno);

    if (args[1][0] == '.') {
        output.emplace_back('[', &args[1][1]);
    } else output.emplace_back('[', parseBias(args, false, lineno));
}


void AssemblyParser::do_jnz(std::vector<::Command> &output, std::vector<const char*> args, size_t lineno) {
    if (args.size() != 2) parseError("Extra characters at end of line ", lineno);

    if (args[1][0] == '.') {
        output.emplace_back(']', &args[1][1]);
    } else output.emplace_back(']', parseBias(args, false, lineno));
}

void AssemblyParser::Parse(std::vector<::Command> &output, std::fstream &in) {
    std::string line;
    std::map<std::string, uint16_t> labels;
    size_t lineno = 0;
    std::vector<const char *> arguments;

    while (std::getline(in, line)) {
        lineno++;

        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        char *s = line.data();
        size_t i = 0, size = line.size();
        arguments.clear();

        while(i < size) {
            while (i < size && std::isspace(s[i]))
                s[i++] = 0;

            if (s[i] == '#') {
                s[i] = 0;
                break;
            }

            if (i >= size)
                break;

            arguments.push_back(&s[i]);
            while (i < size && !std::isspace(s[i]) && s[i] != '#')
                ++i;
        }

        if (arguments.empty())
            continue;

        const char *name = arguments[0];

        for (auto &cmd : commands)
            if (strcmp(name, cmd.name) == 0)
                switch (cmd.type) {
                    case CommandType::CTRLIO:
                        if (arguments.size() != 1) parseError("Extra characters at end of line ", lineno);
                        output.emplace_back(cmd.chr);
                        goto found;

                    case CommandType::BUILTIN:
                        output.emplace_back(cmd.chr, parseBias(arguments, cmd.neg, lineno));
                        goto found;

                    case CommandType::EXTENSION:
                        (this->*cmd.fn)(output, arguments, lineno);
                        goto found;
                }

        parseError("Unknown command at line ", lineno);

found:;
    }

    Linking(output);
}

uint16_t AssemblyParser::parseBias(std::vector<const char*> ss, bool neg, size_t lineno) {
    if (ss.size() > 2) parseError("Extra characters at end of line ", lineno);
    if (ss.size() == 1)
        return static_cast<uint16_t>(neg? -1: 1);

    char *endptr;
    long bias = strtol(ss[1], &endptr, 0);
    if (*endptr) parseError("Extra characters at end of line ", lineno);

    if (neg) bias = -bias;
    auto ul = static_cast<unsigned long>(bias);
    if (bias > 0x1FFF || bias < -0x1FFF) parseError("Bias out of range at line ", lineno);

    return static_cast<uint16_t>(ul);
}

void AssemblyParser::Linking(std::vector<::Command> &output) {
    uint16_t ip = 0;

    for (auto &cmd : output) {
        ip++;
        if ((cmd.CmdChar() == '[' || cmd.CmdChar() == ']') && !cmd.Label().empty())
            cmd.Bias(static_cast<int16_t>(labels[cmd.Label()] - ip));
    }
}


void Compiler::WriteAssembly(std::vector<Command> &output, std::fstream &out) {

}
