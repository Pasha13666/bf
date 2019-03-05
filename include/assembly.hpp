// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 05.03.19.
//

#ifndef BF_ASSEMBLY_HPP
#define BF_ASSEMBLY_HPP

#include <vector>
#include <stdexcept>
#include <map>
#include "command.hpp"

class AssemblyParser {
public:
    void Parse(std::vector<::Command> &output, std::fstream &in);

private:
    constexpr static const char *spaces = " \n\r\t";
    using Handler = void (AssemblyParser::*)(std::vector<::Command> &output, std::vector<const char*> line, size_t lineno);


    enum class CommandType {
        CTRLIO,
        BUILTIN,
        EXTENSION
    };

    struct Command {
        Command(const char *name, char chr) noexcept : name(name), chr(chr), type(CommandType::CTRLIO) {}
        Command(const char *name, char chr, bool neg) noexcept : name(name), chr(chr), neg(neg), type(CommandType::BUILTIN) {}
        Command(const char *name, Handler fn) noexcept : name(name), fn(fn), type(CommandType::EXTENSION) {}

        const char *name;
        CommandType type;
        char chr = ' ';
        union {
            bool neg;
            Handler fn;
        };
    };

    static const Command commands[];

    void do_label(std::vector<::Command> &output, std::vector<const char*> line, size_t lineno);
    void do_jz(std::vector<::Command> &output, std::vector<const char*> line, size_t lineno);
    void do_jnz(std::vector<::Command> &output, std::vector<const char*> line, size_t lineno);

    void Linking(std::vector<::Command> &output);
    uint16_t parseBias(std::vector<const char*>, bool neg, size_t lineno);

    [[noreturn]] inline void parseError(const char *s, size_t lineno){
        throw std::runtime_error(s + std::to_string(lineno));
    }

    std::map<std::string, size_t> labels{};

};

#endif //BF_ASSEMBLY_HPP
