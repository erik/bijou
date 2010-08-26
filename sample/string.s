    ;; String library tests
        ;; requires string lib (obviously)

; function header
>HEAD
    .regs 15    ; number of registers needed
    .globals 0  ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    getexternal 0 0 ; string_cat
    loadk   1 1   ; "Hello, "
    loadk   2 2   ; "World"

    call 4  0 2   ; string_cat("Hello, ", " World")
    closure 3 1K  ; println

    call   5 3 1  ; println

    getexternal 0 3 ; char_at
    loadk    1 1    ; "Hello, "
    loadk    2 4    ; 0

    call   4 0 2    ; char_at("Hello", 0) 
    closure  3 1K   ; println

    call 5 3 1      ; println

    getexternal 0 5 ; index_of
    loadk       1 1 ; "Hello, "
    loadk       2 6 ; "ll"

    call    4 0 2   ; index_of("Hello", "ll")
    closure 3 1K    ; println

    call 5 3 1      ; println 

    getexternal 0 7 ; substr
    loadk       1 1 ; "Hello, "
    loadk       2 8 ; 1
    loadk       3 9 ; 3

    call      4 0 3 ; substr("Hello, ", 1, 3)
    closure   3 1K  ; println

    call      5 3 1 ; println

    getexternal 0 10 ; length
    loadk       1 2 ; "World"

    call      4 0 1 ; length("World")
    closure    3 1K ; println

    call      5 3 1 ; println

    return 5      ;

<CODE

; const secton
>CONST

    3#"string_cat" ; 0
    3#"Hello, "    ; 1
    3#"World"      ; 2

    3#"char_at"    ; 3
    1#0            ; 4

    3#"index_of"   ; 5
    3#"ll"         ; 6

    3#"substr"     ; 7
    1#1            ; 8
    1#3            ; 9

    3#"length"     ; 10
    
<CONST
