;errors that will make the program stop after the preAssembler:

;macro name not valid:
macro rts
macro .string
macro .struct
macro .data
macro 1


;macro name alreay exist:
macro m1
inc K
endmacro
macro m1
