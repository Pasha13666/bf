[![Build Status](https://travis-ci.com/Pasha13666/bf.svg?branch=master)](https://travis-ci.com/Pasha13666/bf)

# Brainfuck++

Another impementation of [@radiolok's bfutils](https://github.com/radiolok/bfutils).

## Work progress

#### Compiler

- [X] Cell clearing optimization
- [X] Generating HEX files.
- [X] Binary-to-binary compiling support.
- [X] Compiling assembly files
- [ ] Generating assembly files.
- [ ] Verbose output support
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

  -O, --optimize arg       Optimization level (default is 1) (default: 1)
  -o, --output arg         Output file (default: a.bin)
  -i, --input arg          Input file
  -X, --output-format arg  Format of output file
  -I, --input-format arg   Format of input file
  -v, --verbose            Produce more output
  -h, --help               Show this message

```

Supported values for `--input-format` and `--output-format`:

 - Brainfuck source file - bf, bfk, brainfuck, source
 - Assembly file - bfa, asm, s, assembly
 - Hex file (output only) - hex, c, cpp
 - Brainfuck++ binary file - bfi, img, bin, binary

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


Assembly support, file format autodetecting, fixes

Common:
 - CTRLIO.SYNC | CTRLIO.CIN for synchronous input (by default)
 - Unsupported HALT marked to remove
 - Not using IpEntry/ApEntry - not supported by HW
 -

Compiler:
 - Input/output file format autodetecting and new CLI options to override it
 - Removed extended commands support
 - Removed CTRLIO optimization (not supported by HW)
 - Reading (basic) assembly files
 - Support for writing hex files

Emulator:
 - Incrementing IP before command execution - as by HW
