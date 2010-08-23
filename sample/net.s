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
    setglobal 15 7   ;
    
    getexternal 14 1 ; socket
    setglobal 14 8   ;
     
    getexternal 13 2 ; connect
    setglobal 13 9   ;
    
    getexternal 12 3 ; send
    setglobal 12 10  ;

    getexternal 11 4 ; recv
    setglobal 11 11  ; 

    getglobal 0 7   ; getaddrinfo
    loadk 1 5       ; "irc.freenode.net"
    loadk 2 6       ; "6667"
    call 3 0 2      ; getaddrinfo("irc...", "6667")
    setglobal 3 12  ;

    getglobal 0 8   ; socket
    getglobal 1 12  ; addrinfo
    call 3 0 1      ; socket(addrinfo)
    setglobal 3 13  ;

    getglobal 0 9   ; connect
    getglobal 1 13  ; socket fd
    getglobal 2 12  ; addrinfo
    call 3 0 2      ; connect(socket, info)

    getglobal 0 11  ; recv
    getglobal 1 13  ; socket fd
    loadk     2 16  ; max read
    call 3 0 2      ; recv(socket, max)
    closure 4 1K    ; println
    move 5 3        ;
    call 6 4 1      ; println

    return 6 ;

<CODE

; const secton
>CONST

    3#"getaddrinfo"; 0
    3#"socket"; 1
    3#"connect"; 2
    3#"send"; 3
    3#"recv"; 4

    3#"irc.freenode.net"; 5
    3#"6667"; 6

    1#0     ; 7
    1#1     ; 8
    1#2     ; 9
    1#3     ; 10
    1#4     ; 11
    1#5     ; 12
    1#6     ; 13
    1#7     ; 14
    1#8     ; 15
    1#10000 ; 16
    
<CONST