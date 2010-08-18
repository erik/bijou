    ;; Call a built in function

; function header
>HEAD
    .regs 10    ; number of registers needed
    .globals 0  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    closure 0 1K ; println 
    loadk 1 0    ; 42
    call 9 0 1   ; R[9] = println(42)
    
    closure 2 2K ; sin
    loadk 3 0    ; 42
    call 3 2 1   ; R[3] = sin(42)

    closure 2 3K ; cos
    loadk 3 0    ; 42
    call 3 2 1   ; R[3] = cos(42)

    closure 2 4K ; tan
    loadk 3 0    ; 42
    call 3 2 1   ; R[3] = tan(42)
    
    return 9     ; tan(42)

<CODE

; const secton
>CONST

    1#42
    
<CONST
