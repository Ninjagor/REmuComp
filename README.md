<div align="center">

# 🐦 REmu Virtual Machine 🐦
**16-bit emulated virtual machine with a custom turing-complete assembly language**

</div>

<!--
<div style="display: flex;">
  <img width="40%" height="800" alt="remu-helloworld" src="https://github.com/user-attachments/assets/2272c733-e725-4f6d-ba6e-a864d18b5075" />
<img width="50%" height="800" alt="remuide" src="https://github.com/user-attachments/assets/74a661cc-d876-40a8-9afc-57a16fd494fd" />
</div>
-->

---

## 📚 Table of Contents
- [🔎 Overview](#-overview)
- [💻 Screenshots](#-screenshots)
- [⚙️ Installation](#️-installation)
- [🧪 Tech Debrief](#-tech-debrief)
- [🤝 Contributing](#-contributing)

---
# Overview
REmuVM is an emulated virtual machine. It has the the ability to address up to 64 kibabytes or RAM via a 16 bit address bus, and the amount of RAM can be configured based on required usage. By default, REmuVM's RAM comes as 64KB, partitioned in segments.

## Default REmuVM Memory Map (64 KB RAM)

| Segment         | Start Addr | End Addr | Size   | Description              |
|-----------------|------------|----------|--------|--------------------------|
| Program         | `0x0000`   | `0xA7FF`  | 42 KB  | Executable program code  |
| VRAM            | `0xA800`   | `0xBFFF`  | 6 KB   | Video memory             |
| Stack           | `0xC000`   | `0xC3FF`  | 1 KB   | Stack memory             |
| Spritesheet     | `0xC400`   | `0xC7FF`  | 1 KB   | Sprite pixel data        |
| Data Segment    | `0xC800`   | `0xF3FF`  | 11 KB  | For use during program|
| **Free** | `0xF400`   | `0xFFFF`  | 3 KB   | Reserved or unused       |

**Total RAM:** 64 KB (`0x0000`–`0xFFFF`)

42 KB RAM has been allocated generously for program data - However, odds are that no reasonable program built for REmuVM is going to come close to that limit. Therefore, this value can be shrunk/resized as needed, and the memory map will adjust accordingly.


### Other RAM partitions
REmuVM comes with a 1KB stack (which is not directly currently exposed to the assembly language's interface), but it is used internally in order to manage things like CALL stacks.

Additionally, 1 KB of RAM stores a built-in spritesheet containing the numbers 0-9 and a-z as sprites. This segment should NOT be modified by REmuVM programs.

Finally, there is a (default 11KB) data segment at the tail end of the RAM partition. This is used to store any values that a user may want to store in RAM during runtime, in addition to static strings and sprites that are loaded from the ROM to the RAM on-demand during runtime.

### Display
REmuVM comes with 6KB of VRAM, and it runs a 96x64 resolution display (that is upscaled by multiple factors). The VRAM *CAN* be manually manipulated during runtime, but this is highly discouraged, and sprites are recommended instead.

### Registers
REmuVM has 32 general purpose 16-bit cpu registers that can be modified and read from during program operation. These registers will be widely used for things like storing values and references to addresses on the virtual RAM.

### Flags
REmuVM's CPU has many flags, such as the `cmp` flag (which stores the result of the latest `CMP` instruction). A detailed map of the flags can be found later in the documentation.

### User input
REmuVM supports keyboard input, for the following keys, in the format below:
```
1234
qwer
asdf
zxcv
```

Which corresponds to:
```
123C
456D
789E
A0BF
```

### Timers
REmuVm has 2 timers running at 60hz - the delay timer and the sound timer. The delay timer is readable and writeble, and the sound timer is only writeable.

### Clock speed
REmuVM has a clock speed of 1000hz (0.001 MHz). This means it runs 1000 ops/sec.Meanwhile, the timers on REmuVM run at 60hz, which is a considerable slowdown. Another factor to note is that due to the 1000hz clock speed, the polling period for user input is also slowed down and not "instant" - it'll be delayed (not significantly, but noticeably) by a few fractions of a second.

## Program running process
### RASM
REmuVM supports a custom turing complete assembly language called RASM. RASM is a simple assembly-like language which is meant to be easy for beginners to learn. Here is an example RASM program that prints "Hello World" to the standard outpu of the REmuVM.
```
_START
    STRS R0, "Hello World!", 0xE000
    CALL _PRINT_HELLO
    HALT

_PRINT_HELLO
    PRINT R0, 1
    RET
```
More detailed documentation for RASM can be found later in the document.

### Simplified stages for a REmuVM Program

Below are the basic stages that a program goes through when running on a REmuVM. (Note: a more detailed description of this process can be found later on).

1.) **Assembler**  
- The assembler converts the RASM code to an intermediary bytecode.  
- It also packages static strings and sprites in a separate data section in the same file.

2.) **Program loading**  
-  The bytecode is extracted from the ROM and it is placed onto the virtual RAM, in the program segment. Each instruction is assigned an address, and labels (such as `_START`) that were mapped and linked in the assembler now have their correct addresses. (This means that a line of code that says `JMP _EXAMPLE` will have been replaced by something like `JMP 0x0020`, assuming 0x0020 is the address where the code for the label `_EXAMPLE` starts in the RAM).

3.) **String and Sprite Loading**
- Strings and sprites, which were seperated from the main program code using a `4x 0xFFFF` delimeter, by the assembler, are parsed and loaded onto the RAM lazily (when they are needed).

4.) **Interpreter**
- Each instruction is recieved coded into 8-byte instructions. For example: `0x0001 0x0002 0xFE02 0x0000`, which loads the value `0xFE02` into CPU register `R2`.
- The CPU maintains a program counter (`pc`) which tracks the intepreter's current "location" that it's interpreting in the RAM. 
- The interpreter reads through these 8 byte instructions and executes actions. On instructions like `JMP`, it moves the `pc` to the RAM address specified by `JMP`, which enables program control flow.

