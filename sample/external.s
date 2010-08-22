    ;; Calling a function from a dynamically loaded library

; function header
>HEAD
    .regs 5     ; number of registers needed
    .globals 0  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    loadk 0 0 ;
    getexternal 0 0 ; 
    call 1 0 0 ;
    return 1 ;

<CODE

; const secton
>CONST

    3#"test"

<CONST
