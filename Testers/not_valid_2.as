;errors that will make the program stop after first pass:

;line length errors
K2: .string "This is a very long line that - more than the maximum length that valid - 80 chars"

;comment error - not the first char in the line
    ;

;label errors
r1: mov r3,S1.1
clr: mov r3,S1.1
.data: mov r3,S1.1
1Label: .string "abc"
longLabel1234567891012345678910: .data 12, 4, 56, 78, 10
label with spaces: mov L1, L1
.extern label1 label2

;comma errors
label1: .data 12,6, -9, 10,
label2: .data 12,6,-9,,10
mov , r3, K


;data errors
.data
.data 433, --653, 30
.data 763, 599, ++6, 5
.data  4, 8, 15, x, 16, 23, 42
.data 3, 4, 6.5, 9


;string errors
.string
.string missing quotation marks
.string missing quotation marks in the bigining"
.string "missing quotation marks in the end


;struct errors
.struct "missing number"
.struct 9
.struct a3 "not valid number"


;too many arguments
.extern label1, label2
.entry label1 ,label2
mov A, r1, r2
cmp A, r1, r2
add A, r0, r1
sub #3, r1, r5
lea HAHAHA, r3, r4
clr r2, r6
not r2, r6
inc r1, r4
dec Count, Monte
jmp B, A
bne Bob ,Sponge
get B, A
jsr BAT, MAN
prn r1, r2
hlt  A
rts B


;operand addressing methods errors
mov S1.1, #1
add S1.2, #2
sub S1.2, #3
not #4
clr #5
lea #-1, r1
lea r4, r1
sub S1.2, #3
inc #-2
dec #-3
jmp #-4
bne #-5
get #+6
jsr #100