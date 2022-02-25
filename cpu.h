#define MEM_SIZE 4096
#define REGFILE_SIZE 16
#define INIT_REG_VALUE 0x00
#define SPRITES_OFFSET 0x000
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define STACK_SIZE 16

class chip8_ram ()
{
	private:
		int8_t ram[MEM_SIZE]; // chip8's 4096 byte memory

	public:
		chip8_ram (); // initialize ram to 0
		int8_t fetch (uint16_t); // get the value at memory address
}

class chip8_cpu ()
{
	private:
		chip8_ram ram; // cpu memory
		int8_t reg_file[REGFILE_SIZE]; // cpu 16 by 8 bit register file	
		uint8_t dt; // delay timer register
		uint8_t st; // sound timer register
		uint16_t pc; // program counter: holds address of executing instruction
		uint8_t sp; // stack pointer: points to top-most level of the stack
		uint16_t I; // address immediate register	
		uint8_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT]; // each pixel on the display represented by 8-bit value
		uint16_t stack[STACK_SIZE]; // program stack;
		
	public:

		chip8_cpu (chip8_ram);

		void load_program (string);
	
		void step (); // operation of the cpu within one clock cycle
	
		void load_sprites ();
		void show_display ();
		
		void reset ();	
		~chip8_cpu (); // do nothing
}
