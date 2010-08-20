    ;; Testing a nested function

; function header
>HEAD
    .regs 1     ; number of registers needed
    .globals 0  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE
    closure 0 0 ;
    call 1 0 0 ;
    return 1  ;
<CODE

; const secton
>CONST

    1#0
    ; a function!
    4#
    >HEAD
        .regs 1
        .upvals 0
        .params 0
    <HEAD
    
    >CODE
        loadk 0 0 ;
        return 0 ;
    <CODE
    
    >CONST
        3#"apple\tabc\nsauce"
    <CONST
        
<CONST
