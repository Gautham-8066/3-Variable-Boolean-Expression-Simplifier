#include <REGX52.H>
#include <string.h>

// --- Function Prototypes ---
void delay(unsigned int ms);
void LCD_Command(unsigned char cmd);
void LCD_Char(unsigned char ch);
void LCD_String(char *str);
void LCD_Clear();
void LCD_Init();
char Keypad_Read();
unsigned char get_minterm_mask(char arr[], char cnt);
void LCD_PrintTerm(char *term, char *sep, char *first);
void showMinimized(char minterms[], char count);

// --- LCD & Keypad Pin Mapping (matches your hardware) ---
sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;
#define LCD_DATA P1

sbit ROW1 = P3^0;
sbit ROW2 = P3^1;
sbit ROW3 = P3^2;
sbit ROW4 = P3^3;
sbit COL1 = P3^4;
sbit COL2 = P3^5;
sbit COL3 = P3^6;

// --- Utility Functions ---
void delay(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 100; j++);
}

// --- LCD Functions ---
void LCD_Command(unsigned char cmd) {
    LCD_DATA = cmd;
    RS = 0; RW = 0; EN = 1; delay(1);
    EN = 0; delay(1);
}
void LCD_Char(unsigned char ch) {
    LCD_DATA = ch;
    RS = 1; RW = 0; EN = 1; delay(1);
    EN = 0; delay(1);
}
void LCD_String(char *str) {
    while (*str) LCD_Char(*str++);
}
void LCD_Clear() {
    LCD_Command(0x01); delay(2);
}
void LCD_Init() {
    LCD_Command(0x38);
    delay(2);
    LCD_Command(0x0C);
    delay(2);
    LCD_Command(0x06);
    delay(2);
    LCD_Command(0x01);
    delay(2);
    LCD_Command(0x80);
    delay(2);
}

// --- Keypad Logic ---
char Keypad_Read() {
    char key = 0;
    COL1 = COL2 = COL3 = 1;
    // Row 1
    ROW1 = 0; ROW2 = ROW3 = ROW4 = 1;
    if (COL1 == 0) { delay(10); while(COL1==0); return '1'; }
    if (COL2 == 0) { delay(10); while(COL2==0); return '2'; }
    if (COL3 == 0) { delay(10); while(COL3==0); return '3'; }
    // Row 2
    ROW2 = 0; ROW1 = ROW3 = ROW4 = 1;
    if (COL1 == 0) { delay(10); while(COL1==0); return '4'; }
    if (COL2 == 0) { delay(10); while(COL2==0); return '5'; }
    if (COL3 == 0) { delay(10); while(COL3==0); return '6'; }
    // Row 3
    ROW3 = 0; ROW1 = ROW2 = ROW4 = 1;
    if (COL1 == 0) { delay(10); while(COL1==0); return '7'; }
    if (COL2 == 0) { delay(10); while(COL2==0); return '8'; }
    if (COL3 == 0) { delay(10); while(COL3==0); return '9'; }
    // Row 4
    ROW4 = 0; ROW1 = ROW2 = ROW3 = 1;
    if (COL1 == 0) { delay(10); while(COL1==0); return '*'; }
    if (COL2 == 0) { delay(10); while(COL2==0); return '0'; }
    if (COL3 == 0) { delay(10); while(COL3==0); return '#'; }
    return 0;
}

// Minterms bitmask
unsigned char get_minterm_mask(char arr[], char cnt) {
    unsigned char m = 0;
    char i;
    for (i = 0; i < cnt; i++) {
        if (arr[i] >= 0 && arr[i] <= 7)
            m |= (1 << arr[i]);
    }
    return m;
}

void LCD_PrintTerm(char *term, char *sep, char *first) {
    if (!(*first)) LCD_String(sep); // Print "+" only after first term
    LCD_String(term);
    *first = 0;
}

// --- Optimized K-map-based minimizer: prints minimized result directly to LCD ---
void showMinimized(char minterms[], char count) {
    unsigned char mask = get_minterm_mask(minterms, count);
    unsigned char used = 0;
    char first = 1;
    char i;

    LCD_Clear();
    LCD_Command(0x80); LCD_String("Simplified:");
    LCD_Command(0xC0);

    // Special cases
    if(mask == 0xFF) { LCD_String("1"); return; }
    if(mask == 0x00) { LCD_String("0"); return; }

    // Quads
    if ((mask & 0x0F) == 0x0F) { LCD_PrintTerm("A'", "", &first); used |= 0x0F; }
    if ((mask & 0xF0) == 0xF0) { LCD_PrintTerm("A", "", &first); used |= 0xF0; }
    if ((mask & 0x33) == 0x33) { LCD_PrintTerm("B'", "+", &first); used |= 0x33; }
    if ((mask & 0xCC) == 0xCC) { LCD_PrintTerm("B", "+", &first); used |= 0xCC; }
    if ((mask & 0x55) == 0x55) { LCD_PrintTerm("C'", "+", &first); used |= 0x55; }
    if ((mask & 0xAA) == 0xAA) { LCD_PrintTerm("C", "+", &first); used |= 0xAA; }

    // Pairs (no returns here; mark covered minterms)
    if ((mask & 0x03) == 0x03 && (used & 0x03) == 0) { LCD_PrintTerm("A'B'", "+", &first); used |= 0x03; }
    if ((mask & 0x0C) == 0x0C && (used & 0x0C) == 0) { LCD_PrintTerm("A'B", "+", &first); used |= 0x0C; }
    if ((mask & 0x30) == 0x30 && (used & 0x30) == 0) { LCD_PrintTerm("AB'", "+", &first); used |= 0x30; }
    if ((mask & 0xC0) == 0xC0 && (used & 0xC0) == 0) { LCD_PrintTerm("AB", "+", &first); used |= 0xC0; }
    if ((mask & 0x05) == 0x05 && (used & 0x05) == 0) { LCD_PrintTerm("A'C'", "+", &first); used |= 0x05; }
    if ((mask & 0x0A) == 0x0A && (used & 0x0A) == 0) { LCD_PrintTerm("A'C", "+", &first); used |= 0x0A; }
    if ((mask & 0x50) == 0x50 && (used & 0x50) == 0) { LCD_PrintTerm("AC'", "+", &first); used |= 0x50; }
    if ((mask & 0xA0) == 0xA0 && (used & 0xA0) == 0) { LCD_PrintTerm("AC", "+", &first); used |= 0xA0; }
    if ((mask & 0x11) == 0x11 && (used & 0x11) == 0) { LCD_PrintTerm("B'C'", "+", &first); used |= 0x11; }
    if ((mask & 0x22) == 0x22 && (used & 0x22) == 0) { LCD_PrintTerm("B'C", "+", &first); used |= 0x22; }
    if ((mask & 0x44) == 0x44 && (used & 0x44) == 0) { LCD_PrintTerm("BC'", "+", &first); used |= 0x44; }
    if ((mask & 0x88) == 0x88 && (used & 0x88) == 0) { LCD_PrintTerm("BC", "+", &first); used |= 0x88; }

    // Singles (if not covered above)
    for(i=0; i<8; i++) {
        if((mask & (1<<i)) && !(used & (1<<i))) {
            if(!first) LCD_String("+");
            switch(i) {
                case 0: LCD_String("A'B'C'"); break;
                case 1: LCD_String("A'B'C"); break;
                case 2: LCD_String("A'BC'"); break;
                case 3: LCD_String("A'BC"); break;
                case 4: LCD_String("AB'C'"); break;
                case 5: LCD_String("AB'C"); break;
                case 6: LCD_String("ABC'"); break;
                case 7: LCD_String("ABC"); break;
            }
            first = 0;
        }
    }
}

// --- MAIN FUNCTION ---
void main() {
    char minterms[8], count = 0, pressed_key;
    LCD_DATA = 0x00;
    LCD_Init();
    LCD_Command(0x80);
    LCD_String("Enter Minterms:");
    LCD_Command(0xC0);
    LCD_String("M: ");
    while (1) {
        pressed_key = Keypad_Read();
        if (pressed_key >= '0' && pressed_key <= '7' && count < 8) {
            minterms[count++] = pressed_key - '0';
            LCD_Char(pressed_key);
        }
        if (pressed_key == '#') break;
        delay(50);
    }
    showMinimized(minterms, count);
    while(1);
}
