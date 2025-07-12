#include "interpreter/interpreter.h"
#include "constants.h"
#include "cpu/cpu.h"
#include "utils/types.h"
#include "vm/vm.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory/stack.h"

void interpret_op(VM* vm, uint16_t words[4]) {
  switch (words[0]) {
    // MOVI - move immediate value to register
    case 0x01: {
      uint16_t register_id = words[1];
      uint16_t value = words[2];

      vm->cpu.registers[register_id].value = value;

      vm->cpu.pc += 8;
      break;
    };

    // MOVR - move value from register to register
    case 0x02: {
      uint16_t register1_id = words[1];
      uint16_t register2_id = words[2];

      vm->cpu.registers[register1_id].value = vm->cpu.registers[register2_id].value;

      vm->cpu.pc += 8;
      break;
    };

    // ADD - set register with the sum of 2 selected registers
    case 0x03: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value + vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // SUB - set register with the difference of 2 selected registers
    case 0x04: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value - vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // MUL - set register with the product of 2 selected registers
    case 0x05: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value * vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // DIV - set register with the quotient of 2 selected registers
    case 0x06: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value / vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // MOD - set register with the modulus of 2 selected registers
    case 0x07: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value % vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // AND - bitwise AND of 2 registers into dst register
    case 0x08: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value & vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // OR - bitwise OR of 2 registers into dst register
    case 0x09: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value | vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // XOR - bitwise XOR of 2 registers into dst register
    case 0x0A: {
      uint16_t srid = words[1];
      uint16_t r1id = words[2];
      uint16_t r2id = words[3];

      vm->cpu.registers[srid].value = vm->cpu.registers[r1id].value ^ vm->cpu.registers[r2id].value;

      vm->cpu.pc += 8;
      break;
    };

    // NOT - bitwise NOT of one register into dst register
    case 0x0B: {
      uint16_t dst_id = words[1];
      uint16_t src_id = words[2];

      vm->cpu.registers[dst_id].value = ~vm->cpu.registers[src_id].value;

      vm->cpu.pc += 8;
      break;
    };

    // CMP - compare 2 registers and set cpu flag
    case 0x10: {
      uint16_t src1 = vm->cpu.registers[words[1]].value;
      uint16_t src2 = vm->cpu.registers[words[2]].value;

      if (src1 == src2) {
        vm->cpu.flags.cmp_flag = EQUAL;
      } else if (src1 > src2) {
        vm->cpu.flags.cmp_flag = GREATER;
      } else {
        vm->cpu.flags.cmp_flag = LESS;
      }

      vm->cpu.pc += 8;
      break;
    };

    // JE - jump if cpu flag for cmp is equal
    case 0x11: {
      uint16_t jmp_location = words[1];

      if (vm->cpu.flags.cmp_flag == EQUAL) {
        vm->cpu.pc = jmp_location;
      } else {
        vm->cpu.pc += 8;
      }

      break;
    }

    // JNE - jump if cpu flag for cmp is not equal
    case 0x12: {
      uint16_t jmp_location = words[1];

      if (vm->cpu.flags.cmp_flag != EQUAL) {
        vm->cpu.pc = jmp_location;
      } else {
        vm->cpu.pc += 8;
      }

      break;
    }

    // JL - jump if cpu flag for cmp is less than
    case 0x13: {
      uint16_t jmp_location = words[1];

      if (vm->cpu.flags.cmp_flag == LESS) {
        vm->cpu.pc = jmp_location;
      } else {
        vm->cpu.pc += 8;
      }

      break;
    }

    // JLE - jump if cpu flag for cmp is less than or equal to
    case 0x14: {
      uint16_t jmp_location = words[1];

      if ((vm->cpu.flags.cmp_flag == LESS) || (vm->cpu.flags.cmp_flag == EQUAL)) {
        vm->cpu.pc = jmp_location;
      } else {
        vm->cpu.pc += 8;
      }

      break;
    }

    // JG - jump if cpu flag for cmp is greater than
    case 0x15: {
      uint16_t jmp_location = words[1];

      if (vm->cpu.flags.cmp_flag == GREATER) {
        vm->cpu.pc = jmp_location;
      } else {
        vm->cpu.pc += 8;
      }

      break;
    }

    // JGE - jump if cpu flag for cmp is greater than or equal to
    case 0x16: {
      uint16_t jmp_location = words[1];

      if ((vm->cpu.flags.cmp_flag == GREATER) || (vm->cpu.flags.cmp_flag == EQUAL)) {
        vm->cpu.pc = jmp_location;
      } else {
        vm->cpu.pc += 8;
      }

      break;
    }

    // JMP - Jump to a specific fn/spot in code
    case 0x20: {
      uint16_t jmp_location = words[1];

      vm->cpu.pc = jmp_location;
      break;
    };

    // CALL - Jump to a subroutine after pushing current PC to stack
    case 0x21: {
      uint16_t jmp_location = words[1];

      push(vm->ram.memory, &vm->stack, vm->cpu.pc + 9);
      vm->cpu.pc = jmp_location;
      break;
    };

    // RET - Jump back to where the subroutine was called from
    case 0x22: {
      uint32_t location = pop(vm->ram.memory, &vm->stack);
      printf("\nLocation to jump to: %40X\n", location);

      vm->cpu.pc = location;

      break;
    }

    // SHL - Logical shift left
    case 0x40: {
      uint16_t dst = words[1];
      uint16_t src = words[2];
      uint16_t amt = words[3];

      vm->cpu.registers[dst].value = vm->cpu.registers[src].value << amt;

      vm->cpu.pc += 8;
      break;
    };

    // SHR - Logical shift right
    case 0x41: {
      uint16_t dst = words[1];
      uint16_t src = words[2];
      uint16_t amt = words[3];

      vm->cpu.registers[dst].value = vm->cpu.registers[src].value >> amt;

      vm->cpu.pc += 8;
      break;
    };

    // SAR - Arithmetic shift right
    case 0x42: {
      uint16_t dst = words[1];
      uint16_t src = words[2];
      uint16_t amt = words[3];

      int16_t val = (int16_t)vm->cpu.registers[src].value;
      vm->cpu.registers[dst].value = (uint16_t)(val >> amt);

      vm->cpu.pc += 8;
      break;
    };

    // ROL - Rotate left
    case 0x43: {
      uint16_t dst = words[1];
      uint16_t src = words[2];
      uint16_t amt = words[3] % 16;

      uint16_t val = vm->cpu.registers[src].value;
      vm->cpu.registers[dst].value = (val << amt) | (val >> (16 - amt));

      vm->cpu.pc += 8;
      break;
    };

    // ROR - Rotate right
    case 0x44: {
      uint16_t dst = words[1];
      uint16_t src = words[2];
      uint16_t amt = words[3] % 16;

      uint16_t val = vm->cpu.registers[src].value;
      vm->cpu.registers[dst].value = (val >> amt) | (val << (16 - amt));

      vm->cpu.pc += 8;
      break;
    };

    // LOAD - Load from memory into register
    case 0x50: {
      uint16_t dst = words[1];
      uint16_t addr = words[2];

      uint8_t* mem = (uint8_t*)vm->ram.memory;
      uint16_t value = mem[addr] | (mem[addr + 1] << 8);

      vm->cpu.registers[dst].value = value;

      vm->cpu.pc += 8;
      break;
    }

    // STRE - Store from register into memory
    case 0x51: {
      uint16_t src = words[1];
      uint16_t addr = words[2];

      if (addr <= PROGRAM_START+PROGRAM_MAX) {
        if (vm->cpu.flags.modification == NOTALLOWED) {
          printf("\nMEMSET Error: Attempted to modify program source\n");
          printf("\nMAX is: %04X, you set %04X\n", PROGRAM_START+PROGRAM_MAX, addr);
          printf("Set ALLOWMOD to enable this unsafe behavior\n");
          vm->cpu.flags.program_interrupt = EFINISH;
          return;
        }
      }

      uint16_t value = vm->cpu.registers[src].value;
      uint8_t* mem = (uint8_t*)vm->ram.memory;

      mem[addr] = value & 0xFF;
      mem[addr + 1] = (value >> 8) & 0xFF;

      vm->cpu.pc += 8;
      break;
    }

    case 0x80: {
        uint16_t reg_id = words[1];
        uint16_t str_idx = words[2];
        uint16_t dest_addr = words[3];

        if ((uint32_t)dest_addr >= RAM_SIZE) {
            printf("STRS Error: destination address out of RAM bounds\n");
            vm->cpu.flags.program_interrupt = EFINISH;
            return;
        }

        const char* str = get_string_from_vm(vm, str_idx);
        if (!str) {
            printf("STRS Error: invalid string index %u\n", str_idx);
            vm->cpu.flags.program_interrupt = EFINISH;
            return;
        }

        uint8_t* mem = vm->ram.memory;
        size_t len = strlen(str);

        if (dest_addr + len >= RAM_SIZE) {
            printf("STRS Error: string write overflows RAM\n");
            vm->cpu.flags.program_interrupt = EFINISH;
            return;
        }

        for (size_t i = 0; i <= len; i++) {
            mem[dest_addr + i] = (uint8_t)str[i];
        }

        vm->cpu.registers[reg_id].value = dest_addr;
        vm->cpu.pc += 8;
        break;
    }

    // CLSM addr, len
    case 0x81: {
      uint16_t addr = words[1];
      uint16_t len  = words[2];

      if (addr + len > RAM_SIZE) {
        printf("CLSM Error: Out-of-bounds clear at 0x%04X (len %u)\n", addr, len);
        vm->cpu.flags.program_interrupt = EFINISH;
        return;
      }

      uint8_t* mem = (uint8_t*)vm->ram.memory;
      for (uint16_t i = 0; i < len; i++) {
        mem[addr + i] = 0x00;
      }

      vm->cpu.pc += 8;
      break;
    }

    case 0x90: {
      vm->cpu.flags.modification = ALLOWED;
      vm->cpu.pc += 8;
      break;
    }

    case 0x91: {
      vm->cpu.flags.modification = NOTALLOWED;
      vm->cpu.pc += 8;
      break;
    }

    // PRINT
    case 0x60: {
      uint16_t reg = words[1];
      uint16_t flag = words[2];

      uint16_t value = vm->cpu.registers[reg].value;

      if (flag == 0x0000) {
        printf("%04X\n", value);
      } else {
        uint16_t addr = value;
        uint8_t* mem = vm->ram.memory;
        while (mem[addr] != '\0') {
          putchar(mem[addr]);
          addr++;
        }
        putchar('\n');
      }

      vm->cpu.pc += 8;
      break;
    }

    // HALT
    case 0x23: {
      vm->cpu.flags.program_interrupt = SFINISH;
      break;
    };

    default: {
      printf("\nInvalid operation, %04X\n", words[0]);
      vm->cpu.flags.program_interrupt = EFINISH;
      return;
    }
  }
}
