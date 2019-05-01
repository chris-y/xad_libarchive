VERSION = 3
REVISION = 2

.macro DATE
.ascii "1.5.2019"
.endm

.macro VERS
.ascii "libarchive 3.2"
.endm

.macro VSTRING
.ascii "libarchive 3.2 (1.5.2019)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: libarchive 3.2 (1.5.2019)"
.byte 0
.endm
