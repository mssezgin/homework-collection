#include <p18cxxx.h>
#include <xc.h>
#include <stdint.h>

#define THE2_TIMER0_START_VALUE         0x00
#define THE2_TIMER0_END_VALUE_LEVEL1    0x4C
#define THE2_TIMER0_END_VALUE_LEVEL2    0x3D
#define THE2_TIMER0_END_VALUE_LEVEL3    0x2E
#define THE2_TIMER1H_START_VALUE        0xF0
#define THE2_TIMER1L_START_VALUE        0x00
#define THE2_ROTATE_AMOUNT_LEVEL1       1
#define THE2_ROTATE_AMOUNT_LEVEL2       3
#define THE2_ROTATE_AMOUNT_LEVEL3       5

#define ONE     0b00000110;
#define TWO     0b00111011;
#define THREE   0b01001111;
#define FOUR    0b01100110;
#define FIVE    0b01101101;
#define SIX     0b01011111;
#define SEVEN   0b00000111;
#define EIGHT   0b01111111;
#define NINE    0b01101111;
#define ZERO    0b00111111;

typedef enum {
    IDLE,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    END,
    LOSE
} game_state_t;


// timer interrupts
uint8_t timer0_overflowed = 0x00;
uint8_t timer0_counter = 0x00;
uint8_t timer0_end_value = THE2_TIMER0_END_VALUE_LEVEL1;
uint8_t timer1_overflowed = 0x00;


// game
uint8_t start_game_req = 0x00;
game_state_t game_state = IDLE;
int8_t health = 9;
uint8_t note_count = 0;
uint8_t max_note_count = 5;
uint16_t seed = 0x0000;
uint8_t rotate_amount = THE2_ROTATE_AMOUNT_LEVEL1;


// ports
uint8_t update_output_req = 0x00;
uint8_t porta_var = 0x00;
uint8_t portb_var = 0x00;
uint8_t portc_var = 0x00;
uint8_t portd_var = 0x00;
uint8_t porte_var = 0x00;
uint8_t portf_var = 0x00;
uint8_t portg_var = 0x00;


// seven segment display
uint8_t update_display_req = 0x00;
uint8_t end = 0;
uint8_t lose = 0;
uint8_t seg_display_rank = 0;





////////////////////////////////////////////////
////////////////// Interrupts //////////////////
////////////////////////////////////////////////

void init_interrupts() {
    INTCONbits.TMR0IE = 1;
    PIE1bits.TMR1IE = 1;
    RCONbits.IPEN = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

/**
 * Called every 6.55 ms (256 * 256 = 65536 cycles).
 * Timer 0 runs in 8-bit mode with 1:256 prescaler.
 * timer0_overflowed flag is set when timer0_counter reaches
 *   - 76 at level 1 (256 * 256 * 76 = 4980736 cycles = 498.01 ms)
 *   - 61 at level 2 (256 * 256 * 61 = 3997696 cycles = 399.77 ms)
 *   - 46 at level 3 (256 * 256 * 46 = 3014656 cycles = 301.46 ms)
 */
void tmr0_isr() {
    INTCONbits.TMR0IF = 0;
    TMR0 = THE2_TIMER0_START_VALUE; // 0x00
    ++timer0_counter;
    if (timer0_counter >= timer0_end_value) {
        timer0_overflowed = 0x01;
        timer0_counter = 0x00;
    }
    return;
}

/**
 * Timer 1 runs in 16-bit mode with 1:1 prescaler.
 */
void tmr1_isr() {
    PIR1bits.TMR1IF = 0;
    TMR1H = THE2_TIMER1H_START_VALUE;   // 0xF0
    TMR1L = THE2_TIMER1L_START_VALUE;   // 0x00
    timer1_overflowed = 0x01;
    return;
}

void __interrupt(high_priority) highPriorityISR(void) {
    if (INTCONbits.TMR0IF)
        tmr0_isr();
    else if (PIR1bits.TMR1IF)
        tmr1_isr();
}

void __interrupt(low_priority) lowPriorityISR(void) {}



////////////////////////////////////////////////
//////////////////// Timers ////////////////////
////////////////////////////////////////////////

/**
 * Initializes timer 0 in 8-bit mode with 1:256 prescaler.
 * Initializes timer 1 in 16-bit mode with 1:1 prescaler.
 * Both timers are off until start functions is called.
 */
void init_timers() {
    T0CON = 0x47;   // 8-bit with 1:256 prescaler
    T1CON = 0x80;   // 16-bit with 1:1 prescaler
}

void start_timer0() {
    TMR0 = THE2_TIMER0_START_VALUE;
    T0CONbits.TMR0ON = 1;
}

void stop_timer0() {
    T0CONbits.TMR0ON = 0;
}

void start_timer1() {
    TMR1H = THE2_TIMER1H_START_VALUE;
    TMR1L = THE2_TIMER1L_START_VALUE;
    T1CONbits.TMR1ON = 1;
}

void stop_timer1() {
    T1CONbits.TMR1ON = 0;
}



////////////////////////////////////////////////
//////////////////// Ports /////////////////////
////////////////////////////////////////////////

void init_ports() {
    
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    LATF = 0x00;
    LATG = 0x00;
    LATH = 0x00;
    LATJ = 0x00;
    
    ADCON1 = 0x0F;

    TRISA = 0xE0;
    TRISB = 0xE0;
    TRISC = 0xE1;   // RC0 is initially input
    TRISD = 0xE0;
    TRISE = 0xE0;
    TRISF = 0xE0;
    TRISG = 0xFF;
    TRISH = 0x00;
    TRISJ = 0x00;
}

void init_portc_as_input() {
    TRISCbits.RC0 = 1;
}

void init_portc_as_output() {
    TRISCbits.RC0 = 0;
    // LATCbits.LATC0 = 0;
}

/**
 * - Checks whether RC0 is pressed while the game is in
 * the idle state.
 * - Checks whether any of RGX is pressed while the game
 * is running.
 */
void port_input_task() {
    switch (game_state) {
        case IDLE:
            if (PORTCbits.RC0 != 0) {
                if ((portc_var & 0x01) == 0x00) {
                    portc_var = portc_var | 0x01;
                }
            } else {
                if ((portc_var & 0x01) != 0x00) {   
                    portc_var = portc_var & 0xFE;
                    init_portc_as_output();
                    start_game_req = 0x01;
                }
            }
            break;
            
        case LEVEL1:
        case LEVEL2:
        case LEVEL3:
            for (uint8_t i = 0, m = 0x01; i < 5; ++i, m = m << 1) {
                if ((PORTG & m) != 0x00) {
                    if ((portg_var & m) == 0x00) {
                        portg_var = portg_var | m;
                        if ((portf_var & m) != 0x00) {
                            portf_var = portf_var & ~m;
                            update_output_req = 0x01;
                        } else {
                            --health;
                            update_display_req = 0x01;
                        }
                    }
                } else {
                    if ((portg_var & m) != 0x00) {
                        portg_var = portg_var & ~m;
                    }
                }
            }
            break;
            
        default:
            break;
    }
}

/**
 * If there is any change on the portx_var variables,
 * update_output_req flag is set, and this function
 * reflects those changes to PORTs.
 */
void port_output_task() {
    if (update_output_req != 0x00) {
        update_output_req = 0x00;
        LATF = (LATF & 0xE0) | (portf_var & 0x1F);
        LATE = (LATE & 0xE0) | (porte_var & 0x1F);
        LATD = (LATD & 0xE0) | (portd_var & 0x1F);
        LATC = (LATC & 0xE0) | (portc_var & 0x1F);
        LATB = (LATB & 0xE0) | (portb_var & 0x1F);
        LATA = (LATA & 0xE0) | (porta_var & 0x1F);
    }
}



////////////////////////////////////////////////
//////////// Seven Segment Display /////////////
////////////////////////////////////////////////

/**
 * Updates the seven segment display depending on
 * the current state of the game and health.
 */
void seven_segment_display_task() {
    
    if (timer1_overflowed != 0x00) {
        timer1_overflowed = 0x00;
        
        if (lose)
            //display lose
        {
            switch (seg_display_rank)
            {
                case 0: 
                    PORTHbits.RH3=1;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=0;
                    PORTJ=0b00111000;
                    break;
                case 1: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=1;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=0;
                    PORTJ=0b00111111;
                    break;
                case 2: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=1;
                    PORTHbits.RH0=0;
                    PORTJ=0b01101101;
                    break;
                case 3: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=1;
                    PORTJ=0b01111001;
                    break;
            }
        }
        else if (end)    
        {
        //display end
            switch (seg_display_rank)
            {
                case 0: 
                    PORTHbits.RH3=1;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=0;
                    PORTJ=0b01111001;
                    break;
                case 1: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=1;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=0;
                    PORTJ=0b01010100;
                    break;
                case 2: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=1;
                    PORTHbits.RH0=0;
                    PORTJ=0b01011110;
                    break;
                case 3: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=1;
                    PORTJ=0;
                    break;
            }

        }
        else
        {//otherwise we will display health and level using timer1 interrupt 
            switch (seg_display_rank)
            {
                case 0: 
                    PORTHbits.RH3=1;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=0;
                    switch(health)
                    {
                        case 9:
                            PORTJ=NINE;
                            break;
                        case 8:
                            PORTJ=EIGHT;
                            break;
                        case 7:
                            PORTJ=SEVEN;
                            break;
                        case 6:
                            PORTJ=SIX;
                            break;
                        case 5:
                            PORTJ=FIVE;
                            break;
                        case 4:
                            PORTJ=FOUR;
                            break;
                        case 3:
                            PORTJ=THREE;
                            break;
                        case 2:
                            PORTJ=TWO;
                            break;
                        case 1:
                            PORTJ=ONE;
                            break;
                        case 0:
                            PORTJ=ZERO;
                            break;
                    }            
                    break;
                case 1: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=1;
                    switch(game_state)
                    {
                        case LEVEL3:
                            PORTJ=THREE;
                            break;
                        case LEVEL2:
                            PORTJ=TWO;
                            break;
                        case LEVEL1:
                            PORTJ=ONE;
                            break;
                        case IDLE:
                            PORTJ=ZERO;
                            break;
                    }
                    break;
                case 2: 
                    PORTHbits.RH3=1;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=0;
                    switch(health)
                    {
                        case 9:
                            PORTJ=NINE;
                            break;
                        case 8:
                            PORTJ=EIGHT;
                            break;
                        case 7:
                            PORTJ=SEVEN;
                            break;
                        case 6:
                            PORTJ=SIX;
                            break;
                        case 5:
                            PORTJ=FIVE;
                            break;
                        case 4:
                            PORTJ=FOUR;
                            break;
                        case 3:
                            PORTJ=THREE;
                            break;
                        case 2:
                            PORTJ=TWO;
                            break;
                        case 1:
                            PORTJ=ONE;
                            break;
                        case 0:
                            PORTJ=ZERO;
                            break;
                    }
                     break;
                case 3: 
                    PORTHbits.RH3=0;
                    PORTHbits.RH2=0;
                    PORTHbits.RH1=0;
                    PORTHbits.RH0=1;
                    switch(game_state)
                    {
                        case LEVEL3:
                            PORTJ=THREE;
                            break;
                        case LEVEL2:
                            PORTJ=TWO;
                            break;
                        case LEVEL1:
                            PORTJ=ONE;
                            break;
                        case IDLE:
                            PORTJ=ZERO;
                            break;
                    }
                    break;
            }
        }
        seg_display_rank=(seg_display_rank+1)%4;
    }
}



////////////////////////////////////////////////
///////////////////// Game /////////////////////
////////////////////////////////////////////////

void reset_game_data() {
    game_state = IDLE;
    health = 9;
    note_count = 0;
    max_note_count = 5;
    rotate_amount = THE2_ROTATE_AMOUNT_LEVEL1;
    timer0_end_value = THE2_TIMER0_END_VALUE_LEVEL1;
    porta_var = 0x00;
    portb_var = 0x00;
    portc_var = 0x00;
    portd_var = 0x00;
    porte_var = 0x00;
    portf_var = 0x00;
    portg_var = 0x00;
    start_game_req = 0x00;
    update_output_req = 0x01;
    update_display_req = 0x01;
}

/**
 * - Returns a random note if the count of generated
 * notes has not reached its maximum value.
 * - Returns zero otherwise.
 */
uint8_t next_note() {
    uint8_t r = 0x00;
    if (note_count < max_note_count) {
        r = 0x01 << (seed & 0x0007) % 5;
        for (uint8_t i = 0; i < rotate_amount; ++i) {
            if ((seed & 0x8000) != 0x0000) {
                seed = (seed << 1) | 0x0001;
            } else {
                seed = seed << 1;
            }
        }
    }
    return r;
}

/**
 * - If the game is in idle state, and start_game_req
 * flag is set, this function saves the timer 1 value
 * as seed, starts the timer 0, and moves on to level 1.
 * - If the game is in level 1, 2, or 3, and the required
 * time interval has passed, this function decreases
 * health if PORTF was not cleared, updates the places
 * of notes, and generates a note for PORTA.
 * - If the game ends or health reaches 0, this function
 * moves on to end or lose state. Immediately, it moves
 * back to idle state while keeping the values of end
 * and lose variables used by seven segment display
 * functions until a restart.
 */
void game_task() {
    
    switch (game_state) {
        
        case IDLE:
            if (start_game_req != 0x00) {
                start_game_req = 0x00;
                seed = TMR1L;
                seed = seed | (TMR1H << 8);
                reset_game_data();
                game_state = LEVEL1;
                lose = 0;
                end = 0;
                start_timer0();
            }
            break;
            
        case LEVEL1:
        case LEVEL2:
        case LEVEL3:
            
            if (timer0_overflowed) {
                
                timer0_overflowed = 0x00;
                update_output_req = 0x01;
                
                if ((portf_var & 0x1F) != 0x00) {
                    --health;
                    update_display_req = 0x01;
                }
                
                portf_var = porte_var;
                porte_var = portd_var;
                portd_var = portc_var;
                portc_var = portb_var;
                portb_var = porta_var;
                porta_var = next_note();
                ++note_count;
                
                if (health <= 0) {
                    game_state = LOSE;
                    lose = 1;
                    break;
                }
                
                if (note_count > (max_note_count + 5)) {
                    note_count = 0;
                    update_display_req = 0x01;
                    
                    switch (game_state) {
                        
                        case LEVEL1:
                            game_state = LEVEL2;
                            max_note_count = 10;
                            rotate_amount = THE2_ROTATE_AMOUNT_LEVEL2;
                            timer0_end_value = THE2_TIMER0_END_VALUE_LEVEL2;
                            break;
                            
                        case LEVEL2:
                            game_state = LEVEL3;
                            max_note_count = 15;
                            rotate_amount = THE2_ROTATE_AMOUNT_LEVEL3;
                            timer0_end_value = THE2_TIMER0_END_VALUE_LEVEL3;
                            break;
                            
                        case LEVEL3:
                            game_state = END;
                            end = 1;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
            
            break;
            
        case END:
        case LOSE:
            stop_timer0();
            init_portc_as_input();
            reset_game_data();
            game_state = IDLE;
            break;
            
        default:
            break;
    }
}


////////////////////////////////////////////////
///////////////////// Main /////////////////////
////////////////////////////////////////////////

int main(int argc, char** argv) {
    
    init_ports();
    init_portc_as_input();
    init_timers();
    reset_game_data();
    init_interrupts();
    start_timer1();
    
    while (1) {
        port_input_task();
        game_task();
        port_output_task();
        seven_segment_display_task();
    }
}
