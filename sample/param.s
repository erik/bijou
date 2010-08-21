    ;; Testing parameters to nested functions

; function header
>HEAD
    .regs 3     ; number of registers needed
    .globals 0  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE
    closure 0 0 ;
    loadk 1 0   ; 42
    call 2 0 1  ;
    return 2    ;
<CODE

; const secton
>CONST

    1#42

    ; function -- inverts the param given to it
    ; shhh, unm doesn't exist!
    4#
    >HEAD
        .name invert
        .regs 3
        .upvals 0
        .params 1
    <HEAD
    
    >CODE
        loadk 0 0    ; R[0] = 0
        getarg 1 0   ; R[1] = param to invert
        sub 2 0 1    ; R[2] = 0 - param
        return 2     ; -param
    <CODE
    
    >CONST
        1#0
    <CONST
        
<CONST
