VERSION = 2
REVISION = 6

.macro DATE
.ascii "23.4.2019"
.endm

.macro VERS
.ascii "RAR 2.6"
.endm

.macro VSTRING
.ascii "RAR 2.6 (23.4.2019)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: RAR 2.6 (23.4.2019)"
.byte 0
.endm
