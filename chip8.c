#include<stdio.h>

#include "SDL.h"

typedef struct{
	int running;
	int refreshed;
}GameState;

typedef struct{
	SDL_Window* window;
	SDL_Renderer* renderer;
}SDL_t;

typedef struct{
	uint8_t ram[4096];
	uint16_t romAddress;
	uint16_t displayAddress;
	uint16_t stackPointer;
	uint8_t registers[16];
	uint16_t registerI;
	uint16_t delayTimer;
	uint16_t soundTimer;
	uint16_t PC;
	uint8_t pressedKey;
	uint8_t keyPad[16];
}chip8_t;

void refreshScreen(SDL_t SDL_data, const chip8_t chip8){
	SDL_SetRenderDrawColor(SDL_data.renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(SDL_data.renderer);
	SDL_SetRenderDrawColor(SDL_data.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for(uint16_t displayBytes=0; displayBytes<256; displayBytes++){
		for(uint8_t bit = 0; bit<8; bit++){
			if((chip8.ram[chip8.displayAddress + displayBytes] >> (7-bit)) & 1){
				SDL_RenderDrawPoint(SDL_data.renderer, (displayBytes%8)*8 + bit, displayBytes/8);
			}
		}
	}
	SDL_RenderPresent(SDL_data.renderer);
}

void handleInput(GameState* gameState, SDL_t SDL_data, chip8_t* chip8){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			case(SDL_QUIT):
				gameState->running = 0;
			case(SDL_KEYDOWN):
				if(event.key.keysym.sym == SDLK_ESCAPE){gameState->running=0;}
				else if(event.key.keysym.sym == SDLK_1){chip8->pressedKey = 0x1;}
				else if(event.key.keysym.sym == SDLK_2){chip8->pressedKey = 0x2;}
				else if(event.key.keysym.sym == SDLK_3){chip8->pressedKey = 0x3;}
				else if(event.key.keysym.sym == SDLK_4){chip8->pressedKey = 0xc;}
				else if(event.key.keysym.sym == SDLK_q){chip8->pressedKey = 0x4;}
				else if(event.key.keysym.sym == SDLK_w){chip8->pressedKey = 0x5;}
				else if(event.key.keysym.sym == SDLK_e){chip8->pressedKey = 0x6;}
				else if(event.key.keysym.sym == SDLK_r){chip8->pressedKey = 0xd;}
				else if(event.key.keysym.sym == SDLK_a){chip8->pressedKey = 0x7;}
				else if(event.key.keysym.sym == SDLK_s){chip8->pressedKey = 0x8;}
				else if(event.key.keysym.sym == SDLK_d){chip8->pressedKey = 0x9;}
				else if(event.key.keysym.sym == SDLK_f){chip8->pressedKey = 0xe;}
				else if(event.key.keysym.sym == SDLK_z){chip8->pressedKey = 0xa;}
				else if(event.key.keysym.sym == SDLK_x){chip8->pressedKey = 0x0;}
				else if(event.key.keysym.sym == SDLK_c){chip8->pressedKey = 0xb;}
				else if(event.key.keysym.sym == SDLK_v){chip8->pressedKey = 0xf;}
				chip8->keyPad[chip8->pressedKey] = 1;
				break;
			case(SDL_KEYUP):
				if(event.key.keysym.sym == SDLK_1){chip8->pressedKey = 0x1;}
				else if(event.key.keysym.sym == SDLK_2){chip8->pressedKey = 0x2;}
				else if(event.key.keysym.sym == SDLK_3){chip8->pressedKey = 0x3;}
				else if(event.key.keysym.sym == SDLK_4){chip8->pressedKey = 0xc;}
				else if(event.key.keysym.sym == SDLK_q){chip8->pressedKey = 0x4;}
				else if(event.key.keysym.sym == SDLK_w){chip8->pressedKey = 0x5;}
				else if(event.key.keysym.sym == SDLK_e){chip8->pressedKey = 0x6;}
				else if(event.key.keysym.sym == SDLK_r){chip8->pressedKey = 0xd;}
				else if(event.key.keysym.sym == SDLK_a){chip8->pressedKey = 0x7;}
				else if(event.key.keysym.sym == SDLK_s){chip8->pressedKey = 0x8;}
				else if(event.key.keysym.sym == SDLK_d){chip8->pressedKey = 0x9;}
				else if(event.key.keysym.sym == SDLK_f){chip8->pressedKey = 0xe;}
				else if(event.key.keysym.sym == SDLK_z){chip8->pressedKey = 0xa;}
				else if(event.key.keysym.sym == SDLK_x){chip8->pressedKey = 0x0;}
				else if(event.key.keysym.sym == SDLK_c){chip8->pressedKey = 0xb;}
				chip8->keyPad[chip8->pressedKey] = 0;
				chip8->pressedKey = 0xFF;
				break;
		}
	}
}

uint8_t getKey(){
	SDL_Event event;
	uint8_t currentlyPressed = 0xFF;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			case(SDL_KEYDOWN):
				if(event.key.keysym.sym == SDLK_1){currentlyPressed = 0x1;}
				else if(event.key.keysym.sym == SDLK_2){currentlyPressed = 0x2;}
				else if(event.key.keysym.sym == SDLK_3){currentlyPressed = 0x3;}
				else if(event.key.keysym.sym == SDLK_4){currentlyPressed = 0xc;}
				else if(event.key.keysym.sym == SDLK_q){currentlyPressed = 0x4;}
				else if(event.key.keysym.sym == SDLK_w){currentlyPressed = 0x5;}
				else if(event.key.keysym.sym == SDLK_e){currentlyPressed = 0x6;}
				else if(event.key.keysym.sym == SDLK_r){currentlyPressed = 0xd;}
				else if(event.key.keysym.sym == SDLK_a){currentlyPressed = 0x7;}
				else if(event.key.keysym.sym == SDLK_s){currentlyPressed = 0x8;}
				else if(event.key.keysym.sym == SDLK_d){currentlyPressed = 0x9;}
				else if(event.key.keysym.sym == SDLK_f){currentlyPressed = 0xe;}
				else if(event.key.keysym.sym == SDLK_z){currentlyPressed = 0xa;}
				else if(event.key.keysym.sym == SDLK_x){currentlyPressed = 0x0;}
				else if(event.key.keysym.sym == SDLK_c){currentlyPressed = 0xb;}
				else if(event.key.keysym.sym == SDLK_v){currentlyPressed = 0xf;}
				printf("%x\n", currentlyPressed);
		
		}
	}
	return currentlyPressed; 
}

void initializeSDL(SDL_t* SDL_data){
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_data->window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 128, SDL_WINDOW_RESIZABLE);
	SDL_data->renderer = SDL_CreateRenderer(SDL_data->window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(SDL_data->renderer, 4, 4);
}

void destroySDL(SDL_t* SDL_data){
	printf("Safely closed Chip-8\n");
	SDL_DestroyRenderer(SDL_data->renderer);
	SDL_DestroyWindow(SDL_data->window);
	SDL_Quit();
}

void initializeChip8(chip8_t* chip8){
	unsigned char font[] = {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
				0x20, 0x60, 0x20, 0x20, 0x70, // 1
				0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
				0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
				0x90, 0x90, 0xF0, 0x10, 0x10, // 4
				0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
				0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
				0xF0, 0x10, 0x20, 0x40, 0x40, // 7
				0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
				0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
				0xF0, 0x90, 0xF0, 0x90, 0x90, // A
				0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
				0xF0, 0x80, 0x80, 0x80, 0xF0, // C
				0xE0, 0x90, 0x90, 0x90, 0xE0, // D
				0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
				0xF0, 0x80, 0xF0, 0x80, 0x80}; // F
	memcpy(chip8->ram, &font, 80);
	chip8->romAddress = 512;
	chip8->displayAddress = 0xF00;
	chip8->stackPointer = 0xEA0;
	chip8->PC = 512;
	chip8->pressedKey = 0xff;
	memset(chip8->keyPad, 0, sizeof(chip8->keyPad));
}

void loadRom(chip8_t* chip8, char* rom){
	FILE* file = fopen(rom, "r");
	fseek(file, 0, SEEK_END);
	const size_t romSize = ftell(file);
	const size_t maxSize = sizeof chip8->ram - 0x200;
	rewind(file);
	if(romSize > maxSize){
		printf("Rom is too large. Compatibility issue.\n");
	}else{
		fread(&chip8->ram[chip8->romAddress], 1, romSize, file);
	}
	for(int i =0; i<romSize; i++){
		printf("%x\n", chip8->ram[chip8->romAddress + i]);
	}
	fclose(file);
}

int willUnset(unsigned char target, unsigned char source){
	for(unsigned char i=0; i<8; i++){
		if((target&1) && ((source&1) == 0)){
			return 1;
		}
		target = target >> 1;
		source = source >> 1;
	}
	return 0;
}

void drawPixel(chip8_t* chip8, unsigned char X, unsigned char Y, unsigned char height){
	uint8_t xByte = (chip8->registers[X]%64)/8;
	uint8_t xBit = chip8->registers[X]%8;
	uint8_t yByte = (chip8->registers[Y]%32);
	uint8_t sprite;
	uint8_t leftSprite;
	uint8_t rightSprite;
	uint8_t currentPixel;
       	uint8_t spritePixel;
	uint16_t memoryOffset = chip8->displayAddress+yByte*8;	
	chip8->registers[15] = 0;
	
	for(uint8_t h=0; h<height; h++){
		if(h+yByte >= 32){return;}
		sprite = chip8->ram[chip8->registerI+h];

		//check left byte for collision
		leftSprite = sprite >> xBit;
		for(uint8_t i=0; i<=7; i++){
			currentPixel = (chip8->ram[memoryOffset+h*8+xByte] >> i) & 0x1;
			spritePixel = (leftSprite >> i) & 0x1;
			if(currentPixel && spritePixel){chip8->registers[15] = 1;}
		}
		//Xor left byte with the appropriate sprite bits
		chip8->ram[memoryOffset+h*8+xByte] ^= leftSprite;
		
		//check right byte for collision if any
		if(xByte < 7 && xBit){
			rightSprite = sprite << (8-xBit);
			for(uint8_t i=8-xBit; i<8; i++){
				currentPixel = ((chip8->ram[memoryOffset+yByte*8+h*8+xByte+1]) >> i) & 0x1;
				spritePixel = (rightSprite >> i) & 0x1;
				if(currentPixel && spritePixel){chip8->registers[15] = 1;}
			}
			//Xor right byte
			chip8->ram[memoryOffset+h*8+xByte+1] ^= rightSprite;
		}
	}
}

void executeInstruction(chip8_t* chip8, GameState* gameState){
	//Opcode is 4 byte in big endian
	//N0,N1,N2,N3 will be used to represent 8 bit chunks of the opcodes
	//(first byte)(N0N1) (second byte)(N2N3)
	uint8_t N0 = (chip8->ram[chip8->PC] >> 4) & 0xF;
	uint8_t N1 = (chip8->ram[chip8->PC] >> 0) & 0xF;
	uint8_t N2 = (chip8->ram[chip8->PC + 1] >> 4) & 0xF;
	uint8_t N3 = (chip8->ram[chip8->PC + 1] >> 0) & 0xF;
	printf("Current PC value : %x\n", chip8->PC);
	printf("Current opcode: %x%x%x%x\n", N0,N1,N2,N3);
	chip8->PC += 2;
	switch(N0){
		case(0):
			if(N3 == 0x0){
				memset(&chip8->ram[chip8->displayAddress], 0, 256);
				gameState->refreshed = 0;
				//printf("Clearing Screen\n");
			}
			else if(N3 == 0xE){
				if(chip8->stackPointer <= 0XEA0){
					printf("Stackpointer attempting to move to region outside of designated stack\n");
					gameState->running = 0;
				}else{
					//printf("Returning from subroutine\n");
					chip8->stackPointer-=2;
					chip8->PC = (chip8->ram[chip8->stackPointer] << 8)|chip8->ram[chip8->stackPointer+1];
				}
			}
			break;
		case(1):
			printf("Jumping to address %x%x%x\n", N1,N2,N3);
			chip8->PC = (((N1 << 4) | N2) << 4) | N3;
			printf("Current PC: %x\n", chip8->PC);
			break;
		case(2):
			chip8->ram[chip8->stackPointer+1] = chip8->PC & 0xff;
			chip8->ram[chip8->stackPointer] = (chip8->PC >> 8) & 0xf;
			chip8->stackPointer+=2;
			chip8->PC = (((N1 << 4) | N2) << 4) | N3;
			break;
		case(3):
			//printf("Value in register %d: %x, comp with %x%x\n", N1, chip8->registers[N1], N2, N3);
			if(chip8->registers[N1]==((N2 << 4) | N3)){chip8->PC+=2;}
			break;
		case(4):
			if(chip8->registers[N1]!=((N2 << 4) | N3)){chip8->PC+=2;}
			break;
		case(5):
			if(chip8->registers[N1] == chip8->registers[N2]){chip8->PC+=2;}
			break;
		case(6):
			//printf("Loading register %d with value %x%x\n", N1, N2, N3);
			chip8->registers[N1] = ((N2 << 4) | N3);
			//printf("Register has value: %d\n", chip8->registers[N1]);
			break;
		case(7):
			//printf("Adding register %d with %d %d\n", N1, N2, N3);
			//printf("Before addition: %d\n", chip8->registers[N1]);
			chip8->registers[N1] += ((N2 << 4) | N3);
			//printf("After Addition: %d\n", chip8->registers[N1]);
			break;
		case(8):
			if(N3 == 0){chip8->registers[N1] = chip8->registers[N2];}
			else if(N3 == 1){
				chip8->registers[N1] = chip8->registers[N1] | chip8->registers[N2];
				chip8->registers[15] = 0;
			}
			else if(N3 == 2){
				chip8->registers[N1] = chip8->registers[N1] & chip8->registers[N2];
				chip8->registers[15] = 0;
			}
			else if(N3 == 3){
				chip8->registers[N1] = chip8->registers[N1] ^ chip8->registers[N2];
				chip8->registers[15] = 0;
			}
			else if(N3 == 4){
				chip8->registers[N1] += chip8->registers[N2];
				chip8->registers[15] = chip8->registers[N1] <= chip8->registers[N2];
			}else if(N3 == 5){
				uint8_t before = chip8->registers[N1];
				//printf("Subtracting %d from %d\n", chip8->registers[N2], chip8->registers[N1]);
				chip8->registers[N1] -= chip8->registers[N2];
				//printf("Result %d\n", chip8->registers[N1]);
				chip8->registers[15] = (chip8->registers[N1] <= before );
				//printf("flag: %x\n", chip8->registers[15]);
			}else if(N3 == 6){
				uint8_t lsb = chip8->registers[N2] & 1;
				//printf("Before shift:%d ", chip8->registers[N1]);
				chip8->registers[N1] = chip8->registers[N2] >> 1;
				chip8->registers[15] = lsb;
				//printf("After shift:%d \n", chip8->registers[N1]);
			}else if(N3 == 7){
				chip8->registers[N1] = chip8->registers[N2] - chip8->registers[N1];
				chip8->registers[15] = (chip8->registers[N2] >= chip8->registers[N1]);
			}else if(N3 == 0xE){
				uint8_t msb = (chip8->registers[N2] >> 7) & 1;
				chip8->registers[N1] = chip8->registers[N2] << 1;
				chip8->registers[15] = msb;
			}
			break;
		case(9):
			if(chip8->registers[N1] != chip8->registers[N2]){chip8->PC+=2;}
			break;
		case(0xA):
			printf("Setting register I with value %x%x%x\n", N1, N2, N3);
			chip8->registerI = (((N1<<4) | N2)<<4)|N3;
			break;
		case(0xB):
			chip8->PC = chip8->registers[0] + ((((N1<<4)|N2)<<4)|N3);
			break;
		case(0xC):
			chip8->registers[N1] = rand() & ((N2<<4) | N3);
			break;
		case(0xD):
			drawPixel(chip8, N1, N2, N3);
			gameState->refreshed = 0;
			break;
		case(0xE):
			printf("Key stored in X: %x\n", chip8->registers[N1]);
			if(N2 == 9){
				if(chip8->keyPad[chip8->registers[N1]]){chip8->PC += 2;}
			}else if(N2 == 0xA){
				if(!chip8->keyPad[chip8->registers[N1]]){chip8->PC += 2;}
			}
			break;
		case(0xF):
			if(N3 == 7){chip8->registers[N1] = chip8->delayTimer;}
			else if(N3 == 0xA){
				printf("awaiting key\n");
				if(chip8->pressedKey <= 0xf){chip8->registers[N1] = chip8->pressedKey;}
				else{chip8->PC -= 2;}
			}else if(N2==1 && N3==5){
				printf("================================================\n");
				chip8->delayTimer=chip8->registers[N1];}
			else if(N2==1 && N3==8){chip8->soundTimer=chip8->registers[N1];}
			else if(N3==0xE){chip8->registerI += chip8->registers[N1];}
			else if(N3==9){chip8->registerI = chip8->registers[N1]*5;}
			else if(N3==3){
				chip8->ram[chip8->registerI] = chip8->registers[N1]/100;
				chip8->ram[chip8->registerI + 1] = (chip8->registers[N1]/10)%10;
				chip8->ram[chip8->registerI + 2] = chip8->registers[N1]%10;
			}else if(N2==5){
				for(int i=0; i<=N1; i++){
					chip8->ram[chip8->registerI + i] = chip8->registers[i];
				}
				chip8->registerI += N1 + 1;
			}else if(N2==6){
				for(int i=0; i<=N1; i++){
					chip8->registers[i] = chip8->ram[chip8->registerI+i];
				}
				chip8->registerI += N1 + 1;
			}
			break;

	}

}

void updateTimers(chip8_t* chip8){
	if(chip8->delayTimer || chip8->soundTimer){
		SDL_Delay(16);
		if(chip8->delayTimer){	
			chip8->delayTimer--;
		}
		if(chip8->soundTimer){
			chip8->soundTimer--;
		}
	}
}

int main(int argc, char** argv){
	SDL_t SDL_data;
	chip8_t chip8 = {0};
	GameState gameState = {1, 0};
	initializeChip8(&chip8);
	loadRom(&chip8, argv[1]);
	initializeSDL(&SDL_data);
	while(gameState.running){
		handleInput(&gameState, SDL_data, &chip8);
		if(chip8.delayTimer==0){executeInstruction(&chip8, &gameState);}
		if(gameState.refreshed == 0){
			refreshScreen(SDL_data, chip8);
			gameState.refreshed = 1;
			if(!chip8.delayTimer){SDL_Delay(16);}

		}
		updateTimers(&chip8);
	}
	destroySDL(&SDL_data);
	return 0;
}
