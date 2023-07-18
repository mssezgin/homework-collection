#include "Includes.h"
#include "lcd.h"

#define TEM 1                       // text entry mode
#define CDM 2                       // character define mode
#define TSM 3                       // text scroll mode
#define TSM_INTERVAL_VALUE 0x8000

///// GLOBAL
uint8_t mode = TEM;                 // current mode

///// LCD DISPLAY
uint8_t lcd_update_req = 0x00;      // set when character at the cursor position changes
uint8_t cursor_lcd = 0x00;          // lcd cursor
// uint8_t n_pred = 37;             // number of predefined characters
uint16_t i_select = 0x0000;         // select character set, 0: predefined, 1: custom
uint8_t i_pred[16] = {              // index for predefined characters
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};
uint8_t i_cust[16] = {              // index for custom characters
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};
uint16_t tsm_counter = TSM_INTERVAL_VALUE;

///// SEVEN SEGMENT DISPLAY
uint8_t digit = 0;                  // current lighting digit of seven segment display
uint8_t seven_seg_mask[4] = {       // seven segment display digits
    0, 0, 0, 0                      // 0: number of custom characters, 1: -, 2: led cursor i, 3: led cursor j
};
uint8_t custom_char[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};
/* uint8_t CUSTOM[8][8] = {{
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }, {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000
    }
}; */

///// A/D CONVERTER

///// PORT
uint8_t porta_mask = 0x00;
uint8_t portb_mask = 0x00;
uint8_t portc_mask = 0x00;
uint8_t portd_mask = 0x00;
uint8_t porte_mask = 0x00;
uint8_t porth_mask = 0x00;
uint8_t portj_mask = 0x00;



void __interrupt(high_priority) highPriorityISR(void) {
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        cursor_lcd = (ADRESH & 0xE0) >> 5;
        LcdCmd(0x40 | cursor_lcd);
        ADCON0bits.GO_nDONE = 1;
    }
}

void __interrupt(low_priority) lowPriorityISR(void) { }

void init_lcd() {
    TRISB = 0x00;
    TRISD = 0x00;
}

void init_seven_seg() {
    TRISH = 0xF0;
    TRISJ = 0x00;
}

void init_button_ports() {
    TRISE = 0x3F;
}

void init_led_ports() {
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
}

void init_adc() {
    TRISH = 0xF0;
    ADCON1 = 0x00;          // all analog
    ADCON0 = 0x30;          // select channel 12
    ADCON2 = 0x2E;          // left justified, 12*TAD, FOSC/64
    ADCON0bits.ADON = 1;
}

void start_adc() {
    ADCON0bits.GO_nDONE = 1;
}

void init_interrupts() {
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.GIE = 1;
}

void lcd_task() {
    if (lcd_update_req) {
        lcd_update_req = 0;
        if ((i_select & (0x0001 << cursor_lcd)) == 0) {
            // display predefined character
            LcdCmd(0x80 | cursor_lcd);
            LcdData(PREDEFINED[i_pred[cursor_lcd]]);
        } else {
            // display custom character
            LcdCmd(0x80 | cursor_lcd);
            LcdData((i_cust[cursor_lcd] & 0x07) << 3);
        }
    }
}

void scan_custom_char() {
    for (uint8_t i = 0; i < 8; i++) {
        /* custom_char[i] =
                (((PORTA & (0x01 << i)) >> i) << 4) |
                (((PORTB & (0x01 << i)) >> i) << 3) |
                (((PORTC & (0x01 << i)) >> i) << 2) |
                (((PORTD & (0x01 << i)) >> i) << 1); */
        custom_char[i] = 0b00000;
        if (LATA & (0x01 << i)) {
            custom_char[i] |= 0b10000;
        }
        if (LATB & (0x01 << i)) {
            custom_char[i] |= 0b01000;
        }
        if (LATC & (0x01 << i)) {
            custom_char[i] |= 0b00100;
        }
        if (LATD & (0x01 << i)) {
            custom_char[i] |= 0b00010;
        }
    }
}

void seven_seg_task() {
    // TODO
}

void start_tsm() {
    uint8_t FINISHED[16] = {
        0, 0, 0, 0, 6, 9, 14, 9,
        19, 8, 5, 4, 0, 0, 0, 0
    };
    LcdClear();
    for (uint8_t i = 0; i < 16; i++) {
        LcdData(PREDEFINED[FINISHED[i]]);
    }
}

void tsm_task() {
    if (mode == TSM) {
        
        if (tsm_counter) {
            tsm_counter--;
            return;
        }
        tsm_counter = TSM_INTERVAL_VALUE;
        
        LcdCmd(0xC0);
        for (uint8_t i = 0; i < 16; i++) {
            if ((i_select & (0x0001 << i)) == 0) {
                // display predefined character
                LcdData(PREDEFINED[i_pred[i]]);
            } else {
                // display custom character
                LcdData((i_cust[i] & 0x07) << 3);
            }
        }
        
        if ((i_select & 0x0001) == 0) {
            i_select = i_select >> 1;
        } else {
            i_select = (i_select >> 1) | 0x8000;
        }
        
        uint8_t i_pred_0 = i_pred[0];
        uint8_t i_cust_0 = i_cust[0];
        for (uint8_t i = 0; i < 16; i++) {
            i_pred[i - 1] = i_pred[i];
            i_cust[i - 1] = i_cust[i];
        }
        i_pred[15] = i_pred_0;
        i_cust[15] = i_cust_0;
    }
}

void button_task() {
    
    // RE0 BUTTON
    if ((porte_mask & 0x01) == 0) {
        if (PORTE & 0x01) {
            porte_mask |= 0x01;
            if (mode == TEM) {
                lcd_update_req = 0x01;
                i_cust[cursor_lcd]++;
                if (i_cust[cursor_lcd] >= seven_seg_mask[0]) {
                    i_cust[cursor_lcd] = 0;
                }
                i_select |= (0x0001 << cursor_lcd);
            } else if (mode == CDM) {
                if (seven_seg_mask[2] < 3) {
                    seven_seg_mask[2]++;
                }
            }
        }
    } else {
        if ((PORTE & 0x01) == 0) {
            porte_mask &= ~0x01;
        }
    }
    
    // RE1 BUTTON
    if ((porte_mask & 0x02) == 0) {
        if (PORTE & 0x02) {
            porte_mask |= 0x02;
            if (mode == TEM) {
                lcd_update_req = 0x01;
                if (i_pred[cursor_lcd] == 0) {
                    i_pred[cursor_lcd] = N_PRED;
                } else {
                    i_pred[cursor_lcd]--;
                }
                i_select &= ~(0x0001 << cursor_lcd);
            } else if (mode == CDM) {
                if (seven_seg_mask[3] < 7) {
                    seven_seg_mask[3]++;
                }
            }
        }
    } else {
        if ((PORTE & 0x02) == 0) {
            porte_mask &= ~0x02;
        }
    }
    
    // RE2 BUTTON
    if ((porte_mask & 0x04) == 0) {
        if (PORTE & 0x04) {
            porte_mask |= 0x04;
            if (mode == TEM) {
                lcd_update_req = 0x01;
                i_pred[cursor_lcd]++;
                if (i_pred[cursor_lcd] >= N_PRED) {
                    i_pred[cursor_lcd] = 0;
                }
                i_select &= ~(0x0001 << cursor_lcd);
            } else if (mode == CDM) {
                if (seven_seg_mask[3] > 0) {
                    seven_seg_mask[3]--;
                }
            }
        }
    } else {
        if ((PORTE & 0x04) == 0) {
            porte_mask &= ~0x04;
        }
    }
    
    // RE3 BUTTON
    if ((porte_mask & 0x08) == 0) {
        if (PORTE & 0x08) {
            porte_mask |= 0x08;
            if (mode == TEM) {
                lcd_update_req = 0x01;
                if (i_cust[cursor_lcd] == 0) {
                    i_cust[cursor_lcd] = seven_seg_mask[0];
                } else {
                    i_cust[cursor_lcd]--;
                }
                i_select |= (0x0001 << cursor_lcd);
            } else if (mode == CDM) {
                if (seven_seg_mask[2] > 0) {
                    seven_seg_mask[2]--;
                }
            }
        }
    } else {
        if ((PORTE & 0x08) == 0) {
            porte_mask &= ~0x08;
        }
    }
    
    // RE4 BUTTON
    if ((porte_mask & 0x10) == 0) {
        if (PORTE & 0x10) {
            porte_mask |= 0x10;
            if (mode == TEM) {
                mode = CDM;
                // TODO
            } else if (mode == CDM) {
                switch (seven_seg_mask[2]) {
                    case 0:
                        LATA ^= (0x01 << seven_seg_mask[3]);
                        break;
                    case 1:
                        LATB ^= (0x01 << seven_seg_mask[3]);
                        break;
                    case 2:
                        LATC ^= (0x01 << seven_seg_mask[3]);
                        break;
                    case 3:
                        LATD ^= (0x01 << seven_seg_mask[3]);
                        break;
                    default:
                        break;
                }
            }
        }
    } else {
        if ((PORTE & 0x10) == 0) {
            porte_mask &= ~0x10;
        }
    }
    
    // RE5 BUTTON
    if ((porte_mask & 0x20) == 0) {
        if (PORTE & 0x20) {
            porte_mask |= 0x20;
            if (mode == TEM) {
                mode = TSM;
                start_tsm();
            } else if (mode == CDM) {
                mode = TEM;
                lcd_update_req = 0x01;
                seven_seg_mask[0]++;
                seven_seg_mask[2] = 0;
                seven_seg_mask[3] = 0;
                scan_custom_char();
                CreateChar(seven_seg_mask[0], custom_char);
                i_cust[cursor_lcd] = seven_seg_mask[0];
                i_select |= (0x0001 << cursor_lcd);
            }
        }
    } else {
        if ((PORTE & 0x20) == 0) {
            porte_mask &= ~0x20;
        }
    }
}

void led_task() {
    // TODO
}

void adc_task() {
    // TODO
}

int main(int argc, char** argv) {

    init_lcd();
    init_seven_seg();
    init_button_ports();
    init_led_ports();
    init_adc();
    init_interrupts();
    InitLCD();
    start_adc();
    
    while (1) {
        adc_task();
        button_task();
        tsm_task();
        led_task();
        lcd_task();
        seven_seg_task();
    }

    return (EXIT_SUCCESS);
}
