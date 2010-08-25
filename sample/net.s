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

    closure 0 0     ; sendmessage
    loadk   1 17    ; nick
    call 2 0 1      ; sendmessage(user)
    loadk   1 18    ; user
    call 2 0 1      ; sendmessage(nick)
    loadk   1 19    ; chan
    call 2 0 1      ; sendmessage(chan)

    getexternal 0 20 ; thread_create
    closure 1 2      ; main loop
    call 7 0 1       ; thread_create(loop)

    getexternal 0 20 ; thread create
    closure 1 1      ; ping
    call 6 0 1       ; thread_create(ping)

    getexternal 0 21 ; thread_join
    move 1 7         ; thread (loop)
    call 8 0 1       ; thread_join

    move 1 6         ; thread (ping)
    call 9 0 1       ; thread_join
    
    return 6 ;

<CODE

; const secton
>CONST

    3#"getaddrinfo" ; 0
    3#"socket"      ; 1
    3#"connect"     ; 2
    3#"send"        ; 3
    3#"recv"        ; 4

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
    1#100 ; 16

    3#"USER blah blah blah :blah blah\r\n" ; user - 17
    3#"NICK bijoubot\r\n "                 ; nick - 18    
    3#"JOIN #()\r\n"                       ; channel - 19

    3#"thread_create"                      ; 20
    3#"thread_join"                        ; 21
    
    4#
    >HEAD
    ; send a message
    ; params
    ;  message - string
    .regs 20      ; number of registers needed
    .name sendmsg ;
    .upvals 0     ; number of upvals
    .params 1     ; number of parameters
    <HEAD
    >CODE
        getglobal 0 0 ; send
        getglobal 1 1 ; socket fd
        getarg    2 0  ; message
        call    3 0 2  ; send(socket fd, message) 
    <CODE
    >CONST
        1#3   ; 0
        1#6   ; 1
    <CONST

    4#
    >HEAD
    ; ping a channel
    ; params
    ;  NONE (thread)
    .regs 20      ; number of registers needed
    .name ping    ;
    .upvals 0     ; number of upvals
    .params 0     ;
    <HEAD
    >CODE
        getexternal 0 3 ; sleep
        loadk 1 5     ;
        call 2 0 1    ; sleep(100)
        getglobal 0 0 ; send
        getglobal 1 1 ; socket fd
        loadk     2 2 ; "PING..."
        call 3 0 2    ; send(socket fd, "PING..")
        jmp -8 ;
    <CODE
    >CONST
        1#3   ; 0
        1#6   ; 1
        3#"PING #()\r\n" ; 2
        3#"sleep"        ; 3
        3#"thread_yield" ; 4
        1#100            ; 5
    <CONST

    4#
    >HEAD
    ; recv
    .regs 20      ; number of registers needed
    .name main_loop ;
    .upvals 0     ; number of upvals
    .params 0     ; number of parameters
    <HEAD
    >CODE
        ; main loop. recv from server
        getexternal 0 0 ; recv
        getglobal 1 1   ; socket fd
        loadk     2 2  ; max read
        call 3 0 2      ; recv(socket, max)
        closure 4 0K    ; println
        move 5 3        ;
        call 6 4 1      ; println
        jmp -7          ;
        
    <CODE
    >CONST
        3#"recv" ; 0
        1#6      ; 1
        1#100    ; 2
    <CONST


<CONST
