;errors that will make the program stop after second pass:

;using label as entry without define it first:
.entry ABC

;using label as entry and as extern:
.extern FEH
.entry FEH

;operand not valid:
add S1.3, r2
sub ##3, r4
not r12
clr T.0
lea ., r1
