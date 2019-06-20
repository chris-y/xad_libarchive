VERSION = 3
REVISION = 3

.macro DATE
.ascii "20.6.2019"
.endm

.macro VERS
.ascii "libarchive 3.3"
.endm

.macro VSTRING
.ascii "libarchive 3.3 (20.6.2019)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: libarchive 3.3 (20.6.2019)"
.byte 0
.endm
