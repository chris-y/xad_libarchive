VERSION = 2
REVISION = 1

.macro DATE
.ascii "25.4.2019"
.endm

.macro VERS
.ascii "Zip 2.1"
.endm

.macro VSTRING
.ascii "Zip 2.1 (25.4.2019)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: Zip 2.1 (25.4.2019)"
.byte 0
.endm
