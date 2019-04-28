VERSION = 3
REVISION = 1

.macro DATE
.ascii "28.4.2019"
.endm

.macro VERS
.ascii "libarchive 3.1"
.endm

.macro VSTRING
.ascii "libarchive 3.1 (28.4.2019)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: libarchive 3.1 (28.4.2019)"
.byte 0
.endm
