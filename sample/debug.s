    ;; Testing debugging library

; function header
>HEAD
    .regs 10    ; number of registers needed
    .globals 10 ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    loadk 1 0       ; r[0] = 0
    getexternal 0 1 ; inspect
    call 2 0 1      ; inspect(R[0])

<CODE

; const secton
>CONST

    1#0          ; 0
    3#"inspect"  ; 1
    
<CONST
