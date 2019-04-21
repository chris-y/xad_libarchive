VERSION = 3
REVISION = 1

.macro DATE
.ascii "21.4.2019"
.endm

.macro VERS
.ascii "RAR 3.1"
.endm

.macro VSTRING
.ascii "RAR 3.1 (21.4.2019)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: RAR 3.1 (21.4.2019)"
.byte 0
.endm
