.ORG 0000h

    LJMP main

fn_wait_tx_complete:
    JNB TI, fn_wait_tx_complete
    CLR TI
    RET

main:
    ; Serial
    ORL TMOD, #20H ; Timer 1 Mode 2 (auto-reload)
    MOV TH1, #-6 ; 4800 baud

    ; Reload
    ; MOV direct direct ; Compiler bug?
    ; MOV TL1, TH1 ; Seems to be a 'MOV src dst'
    MOV A, TH1
    MOV TL1, A

    MOV SCON, #50H ; 8-bit 1-stop
    SETB TR1 ; Start Timer 1

    MOV SBUF, #72 ; H
    ACALL fn_wait_tx_complete
    MOV SBUF, #69 ; E
    ACALL fn_wait_tx_complete
    MOV SBUF, #76 ; L
    ACALL fn_wait_tx_complete
    MOV SBUF, #76 ; L
    ACALL fn_wait_tx_complete
    MOV SBUF, #79 ; O
    ACALL fn_wait_tx_complete
    MOV SBUF, #33 ; !
    ACALL fn_wait_tx_complete

.END
