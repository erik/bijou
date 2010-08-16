    ;; Example Program

; function header
>HEAD
    .regs 20    ; number of registers needed
    .globals 5  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    loadk 0 2     ; r[0] = 42
    add 1 0 1K    ; r[1] = 42 + 5
    setglobal 1 0 ; GBL[K[0]] = R[1]
    getglobal 0 0 ; R[0] = GBL[K[0]]
    return 0      ; 47

<CODE

; const secton
>CONST

    1#0
    1#42
    1#5
    3#"this is a string"
<CONST
