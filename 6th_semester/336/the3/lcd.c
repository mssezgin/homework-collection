#include "Includes.h"
#include "lcd.h"

void Pulse(void){
    LCD_EN = 1;
    __delay_us(LCD_PULSE);
    LCD_EN = 0;
    __delay_us(LCD_PULSE);
}

void InitLCD(void) {
    __delay_ms(20);
    PORTD = 0x30;
    Pulse();
    
    __delay_ms(6);
    PORTD = 0x30;
    Pulse();
    
    __delay_us(300);
    PORTD = 0x30;
    Pulse();
    
    __delay_ms(2);
    PORTD = 0x20;
    Pulse();
        
    LcdCmd(0x2C);
    LcdCmd(0x0C);
    LcdClear();
}

void LcdCmd(uint8_t cmd){
    // Send 'cmd' as a command to the LCD.
    LCD_RS = 0;
    LCD_D7 = !!(cmd & (0x01 << 7));
    LCD_D6 = !!(cmd & (0x01 << 6));
    LCD_D5 = !!(cmd & (0x01 << 5));
    LCD_D4 = !!(cmd & (0x01 << 4));
    Pulse();
    LCD_D7 = !!(cmd & (0x01 << 3));
    LCD_D6 = !!(cmd & (0x01 << 2));
    LCD_D5 = !!(cmd & (0x01 << 1));
    LCD_D4 = !!(cmd & (0x01 << 0));
    Pulse();
}

void LcdData(uint8_t data){
    // Send 'data' as data to the LCD.
    LCD_RS = 1;
    LCD_D7 = !!(data & (0x01 << 7));
    LCD_D6 = !!(data & (0x01 << 6));
    LCD_D5 = !!(data & (0x01 << 5));
    LCD_D4 = !!(data & (0x01 << 4));
    Pulse();
    LCD_D7 = !!(data & (0x01 << 3));
    LCD_D6 = !!(data & (0x01 << 2));
    LCD_D5 = !!(data & (0x01 << 1));
    LCD_D4 = !!(data & (0x01 << 0));
    Pulse();
}

void CreateChar(uint8_t addr, uint8_t charmap[]) {
    addr &= 0x7; // we only have 8 locations 0-7
    LcdCmd(0x40 | (addr << 3)); // Set the address for custom char.
    for (int i=0; i<8; i++) {
        LcdData(charmap[i]); // Write char data.
    }
}

void LcdClear() {
    LcdCmd(0x01);
}
