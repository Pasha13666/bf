[![Build Status](https://travis-ci.com/Pasha13666/bf.svg?branch=master)](https://travis-ci.com/Pasha13666/bf)

# Brainfuck++

Another impementation of [@radiolok's bfutils](https://github.com/radiolok/bfutils).

## Work progress

#### Compiler

- [X] Cell clearing optimization
- [ ] Joining CTRLIO commaind
- [ ] Verbose output support
- [ ] Debugging support
- [ ] loop-shifting
- [ ] Generating HEX files.
- [ ] Generating assembly files.
- [ ] Binary-to-binary compiling support.
- [ ] Options for controlling code base address.

#### Emulator

- [X] Debugging support
- [X] Counting commands
- [X] Protected mode
- [ ] Rewrite CTRLIO.CIN command support
- [ ] Optimize for speed

## Bfpp

```
$ bf bfpp
Brainfuck++ compiler
Usage:
  bfpp [OPTION...]

  -O, --optimize arg   Optimization level (default is 1) (default: 1)
  -o, --output arg     Output file (default: a.out)
  -i, --input arg      Input file
  -e, --extended       Enable extended instruction set
  -S, --asm            Write assembly file
  -H, --hex            Write opcodes in hex
  -d, --debug          Write debug information
  -v, --verbose        Produce more output
  -h, --help           Show this message
  -l, --loop-shifting  Loop shifting on debug
```

## Bfrun

```
$ bf bfrun
Brainfuck++ emulator
Usage:
  bfrun [OPTION...]

  -i, --input arg   Input file
  -w, --8bit        Force 8 bit mode
  -W, --16bit       Force 16 bit mode
  -p, --protected   Run in protected mode
  -s, --statistics  Count executed instructions
  -v, --verbose     Produce more output
  -h, --help        Show this message
```

## Brainfuck++ binary format

Comparable with [@radiolok's bfpp binary files](https://github.com/radiolok/bfutils#binary).

