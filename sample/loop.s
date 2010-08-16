    ;; Infinite loop program

; function header
>HEAD
    .regs 1    ; number of registers needed
    .globals 0  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    loadk 0 0     ; r[0] = 0
    jmp -1        ; jump back 1 (muaha)
    return 0      ; never reached

<CODE

; const secton
>CONST

    1#0
    
<CONST
