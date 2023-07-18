PROCESSOR 18F8722
    
#include <xc.inc>

; configurations
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

; global variable declarations
GLOBAL  t1, t2, t3                              ; variables for time delay
GLOBAL  state                                   ; 0: default, 1: level selection, 2: action selection, 3: countdown
GLOBAL  blink_state                             ; 0: leds are off, 1: leds are on
GLOBAL  ms500_passed_flag                       ; set whenever 500ms passes
GLOBAL  level                                   ; [1-4]
GLOBAL  action                                  ; [1-2]
GLOBAL  playtime                                ; level * action
GLOBAL  leds_need_update                        ; 1 when leds need update
GLOBAL  portb_mask, portc_mask, portd_mask      ; led port masks
GLOBAL  ra4_state, re4_state                    ; button states

; allocating memory for variables
PSECT udata_acs
    t1:
        DS  1
    t2:
        DS  1
    t3:
        DS  1
    state:
        DS  1
    blink_state:
        DS  1
    ms500_passed_flag:
        DS  1
    level:
        DS  1
    action:
        DS  1
    playtime:
        DS  1
    leds_need_update:
        DS  1
    portb_mask:
        DS  1
    portc_mask:
        DS  1
    portd_mask:
        DS  1
    ra4_state:
        DS  1
    re4_state:
        DS  1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto    main

; DO NOT DELETE OR MODIFY
; 500ms pass check for test scripts
ms500_passed:
    nop
    return

; DO NOT DELETE OR MODIFY
; 1sec pass check for test scripts
ms1000_passed:
    nop
    return


init:
    ; clear ra4_state and re4_state
    clrf    ra4_state
    clrf    re4_state
    
    ; clear variables
    clrf    ms500_passed_flag
    clrf    leds_need_update
    
    
    ; set RA4 and RE4 as inputs
    clrf    LATA
    clrf    LATE
    movlw   0xFF
    movwf   TRISA
    movwf   TRISE
    
    ; set RB[0-3] as output
    movlw   0x0F
    movwf   LATB
    movlw   0xF0
    movwf   TRISB
    
    ; set RC[0-1] as output
    movlw   0x03
    movwf   LATC
    movlw   0xFC
    movwf   TRISC
    
    ; set RD[0-8] as output
    movlw   0xFF
    movwf   LATD
    movlw   0x00
    movwf   TRISD
    
    return


busy_wait_ms1000:
    movlw   0x84
    movwf   t3
    _loop3:
        movlw   0xAF
        movwf   t2
        _loop2:
            movlw   0x8F
            movwf   t1
            _loop1:
                decfsz  t1, 1
                goto    _loop1
                decfsz  t2, 1
                goto    _loop2
                decfsz  t3, 1
                goto    _loop3
    return


reset_to_default_state:
    ; set state to 0
    clrf    state
    
    ; set port masks to default
    clrf    portd_mask
    movlw   0x01
    movwf   portb_mask
    movwf   portc_mask
    
    ; set led update flag
    movwf   leds_need_update
    
    ; set level, action and blink_state to 1
    movwf   level
    movwf   action
    movwf   blink_state
    
    ; set playtime to 0
    clrf    playtime
    
    ; reset time variables
    movlw   0x33
    movwf   t1
    movwf   t2
    movwf   t3
    
    return


ra4_task:
    movlw   0x00
    cpfseq  ra4_state
    goto    ra4_was_pressed
    
    ; ra4_state = 0
        btfsc   PORTA, 4
        goto    ra4_pressed
        ; RA4 = 0
            return
        ; RA4 = 1
        ra4_pressed:
            movlw   0x01
            movwf   ra4_state
            return
    
    ; ra4_state = 1
    ra4_was_pressed:
        btfss   PORTA, 4
        goto    ra4_released
        ; RA4 = 1
            return
        ; RA4 = 0
        ra4_released:
            clrf    ra4_state
            
            movlw   0x01
            cpfseq  state
            goto    state_is_not_1
            goto    level_selection
            
            state_is_not_1:
            movlw   0x02
            cpfseq  state
            return
            goto    action_type_selection
            
            level_selection:
                ; set led update flag
                bsf     leds_need_update, 0
                
                movlw   0x04
                cpfslt  level
                goto    level_is_4
                ; level < 4
                incf    level           ; level++
                rlncf   portb_mask      ; portb_mask = 00000011 -> 00000110
                bsf     portb_mask, 0   ; portb_mask =          -> 00000111
                return
                ; level = 4
                level_is_4:
                movlw   0x01
                movwf   level
                movwf   portb_mask
                return
                
            action_type_selection:
                ; set led update flag
                bsf     leds_need_update, 0
                
                movlw   0x03
                xorwf   action
                xorwf   portc_mask
                return


re4_task:
    movlw   0x00
    cpfseq  re4_state
    goto    re4_was_pressed
    
    ; re4_state = 0
        btfsc   PORTE, 4
        goto    re4_pressed
        ; RE4 = 0
            return
        ; RE4 = 1
        re4_pressed:
            movlw   0x01
            movwf   re4_state
            return
    
    ; re4_state = 1
    re4_was_pressed:
        btfss   PORTE, 4
        goto    re4_released
        ; RE4 = 1
            return
        ; RE4 = 0
        re4_released:
            clrf    re4_state
            ; state change
            movlw   0x02
            cpfslt  state
            goto    countdown_state
            ; case: state = [0-1]
            incf    state                       ; move on to the next state
            return
            
            countdown_state:
                ; movlw 0x02
                cpfseq  state
                ; case: state = 3, countdown has already started
                return
                ; case: state = 2
                incf    state                   ; move on to the countdown state
                call    calculate_playtime
                bsf     leds_need_update, 0
                ; reset chronometer
                movlw   0x33
                movwf   t1
                movwf   t2
                movwf   t3
                return


calculate_playtime:
    movf    level, w
    mulwf   action
    movff   PRODL, playtime             ; playtime = level * action
    
    clrf    portd_mask
    movf    playtime, w
    calculate_loop:                     ; while (wreg != 0) {
        rlncf   portd_mask              ;     mask = mask << 1;
        bsf     portd_mask, 0           ;     mask = mask | 1;
        decfsz  WREG                    ;     wreg--;
        goto    calculate_loop          ; }
    
    return


chronometer_task:
    decfsz  t1                          ; if (--t1 != 0)
    return                              ;     return;
    movlw   0x32                        ; t1 = 0x32
    movwf   t1
    
    decfsz  t2                          ; if (--t2 != 0)
    return                              ;     return;
    movlw   0x32                        ; t2 = 032
    movwf   t2
    
    decfsz  t3                          ; if (--t3 != 0)
    return                              ;     return;
    movlw   0x33                        ; t3 = 0x33
    movwf   t3
    
    bsf     ms500_passed_flag, 0
    return


ms500_task:
    btfss   ms500_passed_flag, 0
    ; ms500_passed_flag = 0
    return
    ; ms500_passed_flag = 1
    bcf     ms500_passed_flag, 0
    btg     blink_state, 0
    bsf     leds_need_update, 0
    
    movlw   0x03
    cpfseq  state
    goto    return_ms500_task
    
    ; countdown state
    btfss   portd_mask, 0
    goto    countdown_ended
    
    decf    playtime                    ; playtime--;
    rrncf   portd_mask                  ; portd_mask >> 1
    bcf     portd_mask, 7
    goto    return_ms500_task
    
    countdown_ended:
    call    reset_to_default_state
    
    return_ms500_task:
    call    ms500_passed
    
    return


led_task:
    btfss   leds_need_update, 0
    return
    
    ; update leds
    ; clear the flag
    bcf     leds_need_update, 0
    
    update_portb:
    movlw   0x01
    cpfseq  state
    goto    turn_on_portb
    btfsc   blink_state, 0
    goto    turn_on_portb
    ; state = 1 && blink_state = 0
    turn_off_portb:
        movlw   0xF0                    ; PORTB = PORTB & 11110000                           ; turn off all leds
        andwf   PORTB                   ; e.g. xxxx0011 = xxxx0011 & 11110000 = xxxx0000
        goto    update_portc
    ; state != 1 || blink_state = 1
    turn_on_portb:
        movf    portb_mask, w           ; PORTB = PORTB | mask                              ; turn on the required leds
        iorwf   PORTB                   ; e.g. xxxx0011 = xxxx0011 | 00000111 = xxxx0111
        iorlw   0xF0                    ; PORTB = PORTB & (mask | 11110000)                 ; turn off other leds
        andwf   PORTB                   ; e.g. xxxx0111 = xxxx0111 & 11110111 = xxxx0111
    
    update_portc:
    movlw   0x02
    cpfseq  state
    goto    turn_on_portc
    btfsc   blink_state, 0
    goto    turn_on_portc
    ; state = 2 && blink_state = 0
    turn_off_portc:
        movlw   0xFC                    ; PORTC = PORTC & 11111100                           ; turn off all leds
        andwf   PORTC                   ; e.g. xxxxxx10 = xxxxxx10 & 11111100 = xxxxxx00
        goto    update_portd
    ; state != 2 || blink_state = 1
    turn_on_portc:
        movf    portc_mask, w           ; PORTC = PORTC | mask                              ; turn on the required leds
        iorwf   PORTC                   ; e.g. xxxxxx10 = xxxxxx10 | 00000001 = xxxxxx11
        iorlw   0xFC                    ; PORTC = PORTC & (mask | 11111100)                 ; turn off other leds
        andwf   PORTC                   ; e.g. xxxxxx11 = xxxxxx11 & 11111101 = xxxxxx01
    
    update_portd:
    movf    portd_mask, w
    iorwf   PORTD                       ; PORTD = PORTD | mask      ; turn on the requierd leds
    andwf   PORTD                       ; PORTD = PORTD & mask      ; turn off other leds
    
    return


PSECT CODE
main:
    call    init
    call    busy_wait_ms1000
    call    ms1000_passed
    call    reset_to_default_state
    call    led_task
    
    loop:
        call    ra4_task
        call    re4_task
        call    chronometer_task
        call    ms500_task
        call    led_task
        goto    loop

end resetVec