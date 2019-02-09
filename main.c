/* 
 * ICP
 * Interactive Control Program
 * for the Parallax P8X32A "Propeller" microcontroller
 * Revision 3.07
 * (c) 2018, 2019 Jacopo Maltagliati - all rights reserved
 */ 

#define K_STR "ICP"
#define K_REV "3.07"

#include "simpletools.h"
#define printf(...) print(__VA_ARGS__)

#define BUFLEN  255 // Buffers length. May limit to 128 later
#define OPSEP   32  // Operator separation char (ASCII 32 "space")
#define COLUMNS 80

#define RDY_LED_PIN 0 //

#define BEEP_TONE 750 
#define BEEP_LENGTH 250

/*
 * getOp
 * stdin to word+args
 * returns 0 if ok, 1 the operand is not a printable ASCII char or a space
 */
int getOp(char *word, char *args);

/*
 * interpretOp
 * word to fx
 * returns fx retval, or -1 if unrecognized word
 */
int interpretOp(char *word, char *args);

/*
 * Functions prototypes
 * Skeleton:
 *     -> int $FLAG_$NAME(char *args);
 * Description:
 *     -> $FLAG: p for public functions, 
 *               r for reserved functions,
 *               a for async functions. 
 *     -> $NAME: functor (4 chars).
 * Return value:   
 *     ->   p: int{0,1}
 *          r: int{0,1}
 *          a: void
 */
int p_word(char*);
int p_vers(char*);
int p_gpio(char*);
int r_beep(char*);
void a_beep(void);
int p_spkr(char*);

/* void main() */
void main() {
	char *oa, *op; // oa[], op[]
	op = (char *) malloc(BUFLEN * sizeof(char));
	oa = (char *) malloc(BUFLEN * sizeof(char));
	p_vers("");
	r_beep("");
    high(RDY_LED_PIN); // RDY LED ON
	while(1) {
		printf("\n? ");
		getOp(op, oa);
	}
	return;
}

/* int getOp() */
int getOp(char *word, char *args) {
	char c;
	short isop = 1;
	int i = 0, ret = 1;
	while(i < BUFLEN) {
		word[i] = args[i] = 0;
		i++;
	}
	i = 0;
	while(i < BUFLEN) {
		c = toupper(getchar());         // Input directly uppercased	
        if(c < OPSEP) break;   // Line termination! (non-printable character)
		if(isop) {             // Instruction Eval
            if(c == OPSEP) {    // Input is space, switch to Instruction Argument
			    isop = 0; 
			    i = 0;
			    continue;
		    } else {           // Buffer to "word" buffer
			    word[i] = c;  
            }
        } else { // Buffer to "args" buffer
			args[i] = c; 
		}
		i++;
	}
    fflush(stdin); // Flush stdin
	if(*word <= 32) return 1; // Garbage character? Leave quietly
	ret = interpretOp(word, args);
	return ret;
}

int interpretOp(char *op, char *args) {
	if(!strcmp(op, "HELP")) {
		return p_word(args);
	} else 
	if(!strcmp(op, "VER")) {
		return p_vers(args);
	} else
	if(!strcmp(op, "PIN")) {
		return p_gpio(args);
	} else 	
	if(!strcmp(op, "BEEP")) { 
		return r_beep(args);
    } else 	
	if(!strcmp(op, "SOUND")) {
	  return p_spkr(args);	
	} else {
		printf("\tUNKNOWN COMMAND %s\n", op);
        r_beep("");
		return -1;
	}
}

/*
 * Functions
 */

int p_word(char *args) {
	printf("HELP\nVER\nPIN\nBEEP\nSOUND\n");
	return 0;
}

int p_vers(char *args) {
	printf("%s %s \n",K_STR,K_REV);
	return 0;
}

int p_gpio(char *args) {
	int pin = atoi(args);
    char status[] = "HIGH";
	if(pin <= 1 || pin > 3) {
		printf("\t\t%d\n", args);
		return 1;
	}
	if(get_state(pin)) {
		low(pin);
		strcpy(status, "LOW");
	} else {
		high(pin);
	}
    printf("\tPIN %d SET TO %s\n", pin, status);
    return 0;
}

int p_spkr(char *args) {
		int freq = atoi(args);
		if (freq < 100 || freq > 1000) {
		 printf("\tFREQUENCY ERROR %d\n", freq);
		 return 1;    
		}
		printf("\tBEEPING AT %d HZ\n", freq);
		freqout(1,1000,freq);
	return 0;
}

int r_beep(char *args) {
    int *cog = cog_run(&a_beep, 128);
    return 0;
}

void a_beep(void) {
    freqout(1,BEEP_LENGTH,BEEP_TONE);
}