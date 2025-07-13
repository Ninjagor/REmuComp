#include "interpreter/interpreter.h"
#include "constants.h"
#include "cpu/cpu.h"
#include "graphics/spritesheet.h"
#include "utils/types.h"
#include "vm/vm.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory/stack.h"

void interpret_op(VM* vm, uint16_t words[4]) {
  switch (words[0]) {
    case 0x01: {
      uint16_t register_id = words[1];
      uint16_t value = words[2];
      vm->cpu.registers[register_id].value = value;
      vm->cpu.pc += 8;
      break;
    }

    case 0x02: {
      uint16_t r1 = words[1], r2 = words[2];
      vm->cpu.registers[r1].value = vm->cpu.registers[r2].value;
      vm->cpu.pc += 8;
      break;
    }

    case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
    case 0x08: case 0x09: case 0x0A: {
      uint16_t dst = words[1], r1 = words[2], r2 = words[3];
      switch (words[0]) {
        case 0x03: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value + vm->cpu.registers[r2].value; break;
        case 0x04: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value - vm->cpu.registers[r2].value; break;
        case 0x05: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value * vm->cpu.registers[r2].value; break;
        case 0x06: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value / vm->cpu.registers[r2].value; break;
        case 0x07: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value % vm->cpu.registers[r2].value; break;
        case 0x08: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value & vm->cpu.registers[r2].value; break;
        case 0x09: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value | vm->cpu.registers[r2].value; break;
        case 0x0A: vm->cpu.registers[dst].value = vm->cpu.registers[r1].value ^ vm->cpu.registers[r2].value; break;
      }
      vm->cpu.pc += 8;
      break;
    }

    case 0x0B: {
      uint16_t dst = words[1], src = words[2];
      vm->cpu.registers[dst].value = ~vm->cpu.registers[src].value;
      vm->cpu.pc += 8;
      break;
    }

    case 0x10: {
      uint16_t v1 = vm->cpu.registers[words[1]].value;
      uint16_t v2 = vm->cpu.registers[words[2]].value;
      vm->cpu.flags.cmp_flag = (v1 == v2) ? EQUAL : (v1 > v2 ? GREATER : LESS);
      vm->cpu.pc += 8;
      break;
    }

    case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: {
      uint16_t loc = words[1];
      uint8_t cond = 0;
      switch (words[0]) {
        case 0x11: cond = (vm->cpu.flags.cmp_flag == EQUAL); break;
        case 0x12: cond = (vm->cpu.flags.cmp_flag != EQUAL); break;
        case 0x13: cond = (vm->cpu.flags.cmp_flag == LESS); break;
        case 0x14: cond = (vm->cpu.flags.cmp_flag == LESS || vm->cpu.flags.cmp_flag == EQUAL); break;
        case 0x15: cond = (vm->cpu.flags.cmp_flag == GREATER); break;
        case 0x16: cond = (vm->cpu.flags.cmp_flag == GREATER || vm->cpu.flags.cmp_flag == EQUAL); break;
      }
      vm->cpu.pc = cond ? loc : vm->cpu.pc + 8;
      break;
    }

    case 0x20:
      vm->cpu.pc = words[1];
      break;

    case 0x21: {
      if (push(&vm->ram, &vm->stack, vm->cpu.pc + 8) != SUCCESS)  {
        printf("Stack overflow on CALL\n");
        vm->cpu.flags.program_interrupt = EFINISH;
        return;
      }
      vm->cpu.pc = words[1];
      break;
    }

    case 0x24:  // CEQ - Call if equal
    case 0x25:  // CNE - Call if not equal
    case 0x26:  // CL  - Call if less
    case 0x27:  // CG  - Call if greater
    case 0x28:  // CLE - Call if less or equal
    case 0x29:  // CGE - Call if greater or equal
    {
        uint16_t target = words[1];
        uint8_t cond = 0;
        switch (words[0]) {
            case 0x24: cond = (vm->cpu.flags.cmp_flag == EQUAL); break;
            case 0x25: cond = (vm->cpu.flags.cmp_flag != EQUAL); break;
            case 0x26: cond = (vm->cpu.flags.cmp_flag == LESS); break;
            case 0x27: cond = (vm->cpu.flags.cmp_flag == GREATER); break;
            case 0x28: cond = (vm->cpu.flags.cmp_flag == LESS || vm->cpu.flags.cmp_flag == EQUAL); break;
            case 0x29: cond = (vm->cpu.flags.cmp_flag == GREATER || vm->cpu.flags.cmp_flag == EQUAL); break;
        }
        if (cond) {
            if (push(&vm->ram, &vm->stack, vm->cpu.pc + 8) != SUCCESS) {
                printf("Stack overflow on conditional CALL\n");
                vm->cpu.flags.program_interrupt = EFINISH;
                return;
            }
            vm->cpu.pc = target;
        } else {
            vm->cpu.pc += 8;
        }
        break;
    }

    case 0x22: {
      uint16_t ret;
      if (pop(&vm->ram, &vm->stack, &ret) != SUCCESS) {
        printf("Stack underflow on RET\n");
        vm->cpu.flags.program_interrupt = EFINISH;
        return;
      }
      vm->cpu.pc = ret;
      break;
    }

    case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: {
      uint16_t dst = words[1], src = words[2], amt = words[3] % 16;
      uint16_t val = vm->cpu.registers[src].value;
      switch (words[0]) {
        case 0x40: vm->cpu.registers[dst].value = val << amt; break;
        case 0x41: vm->cpu.registers[dst].value = val >> amt; break;
        case 0x42: vm->cpu.registers[dst].value = (uint16_t)((int16_t)val >> amt); break;
        case 0x43: vm->cpu.registers[dst].value = (val << amt) | (val >> (16 - amt)); break;
        case 0x44: vm->cpu.registers[dst].value = (val >> amt) | (val << (16 - amt)); break;
      }
      vm->cpu.pc += 8;
      break;
    }

    case 0x50: {
      uint16_t dst = words[1], addr = words[2];
      uint8_t* mem = (uint8_t*)vm->ram.memory;
      vm->cpu.registers[dst].value = mem[addr] | (mem[addr + 1] << 8);
      vm->cpu.pc += 8;
      break;
    }

    case 0x51: {
      uint16_t src = words[1], addr = words[2];
      if (addr <= PROGRAM_START + PROGRAM_MAX && vm->cpu.flags.modification == NOTALLOWED) {
        printf("MEMSET Error: Attempted to modify program source\n");
        vm->cpu.flags.program_interrupt = EFINISH;
        return;
      }
      uint16_t val = vm->cpu.registers[src].value;
      ((uint8_t*)vm->ram.memory)[addr] = val & 0xFF;
      ((uint8_t*)vm->ram.memory)[addr + 1] = (val >> 8) & 0xFF;
      vm->cpu.pc += 8;
      break;
    }

    case 0x52: case 0x54: {
      uint16_t id = vm->cpu.registers[words[1]].value;
      uint16_t x  = vm->cpu.registers[words[2]].value;
      uint16_t y  = vm->cpu.registers[words[3]].value;
      if (x >= SCREEN_WIDTH - SPRITE_SIZE) x = SCREEN_WIDTH - SPRITE_SIZE;
      if (y >= SCREEN_HEIGHT - SPRITE_SIZE) y = SCREEN_HEIGHT - SPRITE_SIZE;

      // if (id < 48 || id > 90) {
      //   vm->cpu.pc += 8;
      //   break;
      // }

      const uint8_t* sprite = &vm->spritesheet[(id-0) * SPRITESHEET_SPRITE_SIZE];
      for (int row = 0; row < SPRITE_SIZE; row++) {
        uint8_t bits = sprite[row];
        for (int col = 0; col < SPRITE_SIZE; col++) {
          if (bits & (1 << (7 - col))) vm->vram[(y + row) * SCREEN_WIDTH + (x + col)] = 1;
        }
      }
      vm->cpu.pc += 8;
      break;
    }

    case 0x55: {
        uint16_t addr = vm->cpu.registers[words[1]].value;
        uint16_t x = vm->cpu.registers[words[2]].value;
        uint16_t y = vm->cpu.registers[words[3]].value;

        if (x >= SCREEN_WIDTH - SPRITE_SIZE) x = SCREEN_WIDTH - SPRITE_SIZE;
        if (y >= SCREEN_HEIGHT - SPRITE_SIZE) y = SCREEN_HEIGHT - SPRITE_SIZE;

        uint8_t* mem = (uint8_t*)vm->ram.memory;

        if (addr + SPRITE_SIZE > RAM_SIZE) {
            printf("DRAW SPRITE Error: Address out of range\n");
            vm->cpu.flags.program_interrupt = EFINISH;
            return;
        }

        for (int row = 0; row < SPRITE_SIZE; row++) {
            uint8_t bits = mem[addr + row];
            for (int col = 0; col < SPRITE_SIZE; col++) {
                if (bits & (1 << (7 - col))) {
                    vm->vram[(y + row) * SCREEN_WIDTH + (x + col)] = 1;
                }
            }
        }

        vm->cpu.pc += 8;
        break;
    }

    // case 0x53: {
    //   uint16_t dst = words[1];
    //   uint16_t addr = vm->cpu.registers[words[2]].value;
    //   if (addr >= RAM_SIZE - 1) {
    //     printf("LOADB Error: out-of-bounds access\n");
    //     vm->cpu.flags.program_interrupt = EFINISH;
    //     return;
    //   }
    //   uint8_t* mem = (uint8_t*)vm->ram.memory;
    //   uint16_t val = mem[addr] | (mem[addr + 1] << 8);
    //   vm->cpu.registers[dst].value = val;
    //   vm->cpu.pc += 8;
    //   break;
    // }


  case 0x53: {
    uint16_t dst = words[1];
    uint16_t addr = vm->cpu.registers[words[2]].value;
    if (addr >= RAM_SIZE-1) {
      printf("LOADB Error: out-of-bounds access\n");
      vm->cpu.flags.program_interrupt = EFINISH;
      return;
    }
    uint8_t* mem = (uint8_t*)vm->ram.memory;
    uint16_t val = mem[addr];  // Just one byte, zero-extended
    vm->cpu.registers[dst].value = val;
    vm->cpu.pc += 8;
    break;
  }

    case 0x80: {
      uint16_t reg = words[1], idx = words[2], addr = words[3];
      const char* str = get_string_from_vm(vm, idx);
      if (!str || addr + strlen(str) >= RAM_SIZE) {
        printf("STRS Error\n");
        vm->cpu.flags.program_interrupt = EFINISH;
        return;
      }
      for (size_t i = 0; i <= strlen(str); i++) {
        ((uint8_t*)vm->ram.memory)[addr + i] = (uint8_t)str[i];
      }
      vm->cpu.registers[reg].value = addr;
      vm->cpu.pc += 8;
      break;
    }

    case 0x60: {
      uint16_t reg = words[1], flag = words[2];
      uint16_t val = vm->cpu.registers[reg].value;
      uint8_t* mem = (uint8_t*)vm->ram.memory;
      if (flag == 0x0000) {
        // Print the raw 16-bit value in hex
        printf("%04X\n", val);
      } else {
        // Print string byte-by-byte
        while (mem[val] != '\0') {
          // printf("ADDR: \"%04x\"\nVALUE: \"%02x\"\nCHAR: '%c' (0x%02x)\n", val, mem[val], mem[val], mem[val]);
          putchar(mem[val]);
          val++;
        }
        putchar('\n');
      }
      vm->cpu.pc += 8;
      break;
    }

    case 0x70: {
      vm->cpu.flags.draw_flag = 1;
      vm->cpu.pc += 8;
      break;
    }


    case 0x72: {
      vm->cpu.flags.graphics_initialized = AWAITING_INITIALIZATION;
      vm->cpu.pc += 8;
      break;
    }


    case 0x71: {
      memset(vm->vram, 0, 96 * 64);
      // vm->cpu.flags.draw_flag = 1;
      vm->cpu.pc += 8;
      break;
    }

    case 0x81: {
      uint16_t addr = words[1], len = words[2];
      if (addr + len > RAM_SIZE) {
        printf("CLSM Error\n");
        vm->cpu.flags.program_interrupt = EFINISH;
        return;
      }
      memset(&((uint8_t*)vm->ram.memory)[addr], 0, len);
      vm->cpu.pc += 8;
      break;
    }

    // LDS
    case 0x83: {
        uint16_t reg = words[1], idx = words[2], addr = words[3];
        const uint8_t* sprite = get_sprite_from_vm(vm, idx);

        if (!sprite || addr + SPRITE_SIZE > RAM_SIZE) {
            printf("\nLDS idx=%u, addr=%u, sprite_count=%zu\n", idx, addr, vm->sprite_count);
            printf("LDS Error\n");
            vm->cpu.flags.program_interrupt = EFINISH;
            return;
        } else {

            uint8_t* ram_bytes = (uint8_t*)vm->ram.memory;
            for (int i = 0; i < SPRITE_SIZE; i++) {
                ram_bytes[addr + i] = sprite[i];
            }

            if (reg < CPU_REGISTER_COUNT) {
                vm->cpu.registers[reg].value = addr;
            } else {
                printf("LDS Error: invalid register %u\n", reg);
                vm->cpu.flags.program_interrupt = EFINISH;
                return;
            }

// printf("Sprite loaded to RAM at address 0x%04X:\n", addr);
    // for (int i = 0; i < SPRITE_SIZE; i++) {
        // printf("%02X ", ram_bytes[addr + i]);
    // }
    // printf("\n");
        }


        vm->cpu.pc += 8;
        break;
    }

    case 0x90:
      vm->cpu.flags.modification = ALLOWED;
      vm->cpu.pc += 8;
      break;

    case 0x91:
      vm->cpu.flags.modification = NOTALLOWED;
      vm->cpu.pc += 8;
      break;

    case 0x23:
      vm->cpu.flags.program_interrupt = SFINISH;
      break;

    default:
      printf("Invalid operation %04X\n", words[0]);
      vm->cpu.flags.program_interrupt = EFINISH;
      break;
  }
}
