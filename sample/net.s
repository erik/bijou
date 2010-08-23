    ;; network test (link with net lib)

; function header
>HEAD
    .regs 20    ; number of registers needed
    .globals 20 ; number of globals
    .upvals 0   ; number of upvals
    .params 0   ; number of parameters
<HEAD

; code section
>CODE

    ;; setting up closures in globals
    
    getexternal 15 0 ; getaddrinfo
    setglobal 15 6   ;
    
    getexternal 14 1 ; socket
    setglobal 14 7   ;
     
    getexternal 13 2 ; connect
    setglobal 13 8   ;
    
    getexternal 12 3 ; send
    setglobal 12 9   ;

    getglobal 0 6   ; getaddrinfo
    loadk 1 4       ; "irc.freenode.net"
    loadk 2 5       ; "6667"
    call 3 0 2      ; getaddrinfo("irc...", "6667")
    setglobal 3 10  ;

    getglobal 0 7   ; socket
    getglobal 1 10  ; addrinfo
    call 3 0 1      ; socket(addrinfo)
    setglobal 3 11  ;
    
    return 0 ;

<CODE

; const secton
>CONST

    3#"getaddrinfo"; 0
    3#"socket"; 1
    3#"connect"; 2
    3#"send"; 3

    3#"irc.freenode.net"; 4
    3#"6667"; 5

    1#0     ; 6
    1#1     ; 7
    1#2     ; 8
    1#3     ; 9
    1#4     ; 10
    1#5     ; 11
    1#6     ; 12
    1#7     ; 13
    1#8     ; 14
    
<CONST
