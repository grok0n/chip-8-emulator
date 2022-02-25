#include "cpu.h"
#include <iostream>

chip8_ram::chip8_ram ()
{
	// set all memory register to 0
	for (int i=0;i<MEM_SIZE;i++)
	{
		ram[i] = INIT_REG_VALUE;
	}
}

// retrieve 8 bit value at memory address addr
int8_t chip8_ram::fetch (uint16_t addr)
{
	return ram[addr%MEM_SIZE]; // prevent access to non-existing memory address
}

chip8_cpu::chip8_cpu ()
{
	ram = chip8_ram ();
	reset ();
}

void chip8_cpu::load_program (string filename)
{

}

void chip8_cpu::reset ()
{
	for (int i=0;i<REGFILE_SIZE;i++)
	{
		reg_file[i] = INIT_REG_VALUE;
	}	
	pc = INIT_REG_VALUE;
	sp = INIT_REG_VALUE;
}

void chip8_cpu::step ()
{
	// check for delay
	if (dt != 0)
	{
		--dt;
		return;
	}

	// fetch
	uint16_t instr = r[pc] << 8; // get the higher byte of the chip8 instructino
	instr += r[pc+1] // get the lower byte of the chip8 instruction

	// instructions decode & execute

	// CLS: clear display instruction
	if (instr == 0x00e0)
	{
		for (int i=0;i<DISPLAY_WIDTH;i++)
			for (int j=0;j<DISPLAY_HEIGHT;j++)
				display[i][j] = 0;
		return;
	}

	// RET: return from subroutine
	if (instr == 0x00ee)
	{
		pc = stack[sp];
		if (sp > 0) --sp;	
	}

	uint16_t opcode = instr >> 12; // retrieve the opcode by shifting the last 12 bits away
	switch (opcode)
	{
		case 0x0: // SYS
			// Ignore this instruction, used only for older chip-8 implemenations
			break;
		case 0x1: // JP addr
			pc = instr & 0x0fff; // get the value of the last 12 bits by using bit mask
			break;
		case 0x2: // CALL addr
			++sp;
			stack[sp] = pc;
			pc = instr & 0x0fff;	
			break;
		case 0x3: // SE Vx, byte
			if (reg_file[(int) ((instr & 0x0f00) >> 8)] == ((int8_t) (instr & 0x00ff))) pc += 2;
			break;
		case 0x4: // SNE Vx, byte
			if (reg_file[(int) ((instr & 0x0f00) >> 8)] != ((int8_t) (instr & 0x00ff))) pc += 2;
			break;
		case 0x5: // SE Vx, Vy
			if (reg_file[(int) ((instr & 0x0f00) >> 8)] == reg_file[(int) ((instr & 0x00f0) >> 4)]) pc += 2;
			break;
		case 0x6: // LD Vx, byte
			reg_file[(int) ((instr & 0x0f00) >> 8)] = (int8_t) ((instr && 0x00ff));	
			break;
		case 0x7: // ADD Vx, byte
			reg_file[(int) ((instr & 0x0f00) >> 8)] += (int8_t) ((instr && 0x00ff));
			break;
		case 0x8:
			switch ((instr & 0x000f))
			{
				case 0x0: // LD Vx, Vy
					reg_file[(int) ((instr & 0x0f00) >> 8)] = reg_file[(int) ((instr & 0x00f0) >> 4)];
					break;	
				case 0x1: // OR Vx, Vy
					reg_file[(int) ((instr & 0x0f00) >> 8)] |= reg_file[(int) ((instr & 0x00f0) >> 4)];
					break;
				case 0x2: // AND Vx, Vy
					reg_file[(int) ((instr & 0x0f00) >> 8)] &= reg_file[(int) ((instr & 0x00f0) >> 4)];
					break;	
				case 0x3: // XOR Vx, Vy
					reg_file[(int) ((instr & 0x0f00) >> 8)] ^= reg_file[(int) ((instr & 0x00f0) >> 4)];
					break;
				case 0x4: // ADD Vx, Vy
					if (reg_file[(int) ((instr & 0x0f00) >> 8)] + reg_file[(int) ((instr & 0x00f0) >> 4)] > 255)
						reg_file[REGFILE_SIZE - 1] = 1;
					else
						reg_file[REGFILE_SIZE - 1] = 0;

					reg_file[(int) ((instr & 0x0f00) >> 8)] += reg_file[(int) ((instr & 0x00f0) >> 4)];
					break;
				case 0x5: // SUB Vx, Vy
					if (reg_file[(int) ((instr & 0x0f00) >> 8)] > reg_file[(int) ((instr & 0x00f0) >> 4)])
						reg_file[REGFILE_SIZE - 1] = 1;
					else
						reg_file[REGFILE_SIZE - 1] = 0;

					reg_file[(int) ((instr & 0x0f00) >> 8)] -= reg_file[(int) ((instr & 0x00f0) >> 4)];
					break;
				case 0x6: // SHR Vx {, Vy}
					reg_file[REGFILE_SIZE - 1] = (reg_file[(int) ((instr & 0x0f00) >> 8)] & 0x01);
					reg_file[(int) ((instr & 0x0f00) >> 8)] >>= 1;
					break;
				case 0x7: // SUBN Vx, Vy
					if (reg_file[(int) ((instr & 0x00f0) >> 4)] > reg_file[(int) ((instr & 0x0f00) >> 8)])
						reg_file[REGFILE_SIZE - 1] = 1;
					else
						reg_file[REGFILE_SIZE - 1] = 0;

					reg_file[(int) ((instr & 0x00f0) >> 4)] -= reg_file[(int) ((instr & 0x0f00) >> 8)];
					break;
				case 0xe: // SHL Vx, {, Vy}
					reg_file[REGFILE_SIZE - 1] = (reg_file[(int) ((instr & 0x0f00) >> 8)] & 0x80);
					reg_file[(int) ((instr & 0x0f00) >> 8)] <<= 1;
					break;
			}
			break;
		case 0x9: // SNE Vx, Vy
			if (reg_file[(int) ((instr & 0x0f00) >> 8)] != reg_file[(int) ((instr & 0x00f0) >> 4)]) pc += 2;
			break;
		case 0xa: // LD I, addr
			I = (instr & 0x0fff);	
			break;
		case 0xb: // JP V0, addr
			pc = ((uint16_t) reg_file[0]) + (instr & 0x0fff);		
			break;
		case 0xc: // RND Vx, byte
			uint8_t rand_num = ;
			reg_file[(int) ((instr & 0x0f00) >> 8)] = rand_num & ((uint8_t) instr & 0x00ff);
			break;
		case 0xd: // DRW Vx, Vy, nibble
				
			break;
		case 0xe:
			if ((instr & 0x00ff) == 0x9e)
			{

			}	

			if ((instr & 0x00ff) == 0xa1)
			{

			}
			break;
		case 0xf:
			switch ((uint8_t) instr & 0x00ff)
			{
				case 0x07:
					reg_file[(int) ((instr & 0x0f00) >> 8)]	= dt;
					break;
				case 0x0a:
		
					break;
				case 0x15:
					dt = reg_file[(int) ((instr & 0x0f00) >> 8)];
					break;
				case 0x18:
					st = reg_file[(int) ((instr & 0x0f00) >> 8)];
					break;
				case 0x1e:
					I += ((uint16_t) reg_file[(int) ((instr & 0x0f00) >> 8)]);
					break;
				case 0x29:
	
					break;
				case 0x33:
	
					break;
				case 0x55:

					break;
				case 0x65:

					break;
			}
	}

}

void chip8_cpu::load_sprites ()
{
	// set "0"
	ram[SPRITES_OFFSET] = 0xf0;
	ram[SPRITES_OFFSET+1] = 0x90;
	ram[SPRITES_OFFSET+2] = 0xf0;
	ram[SPRITES_OFFSET+3] = 0xf0;
	ram[SPRITES_OFFSET+4] = 0xf0;
	
}

void chip8_cpu::show_display ()
{
	// enumerate through all pixels and if the are encoded as lit, display "**" in pixel position
	for (int i=0;i<DISPLAY_HEIGHT;i++)
	{
		for (int j=0;i<DISPLAY_WIDTH;j++)
		{
			if (display[j][i]) std::cout << "*";
		}	
		std::cout << '\n';
	}
	std::cout << "\033c";
}
