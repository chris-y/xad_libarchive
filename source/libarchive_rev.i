VERSION		EQU	3
REVISION	EQU	2

DATE	MACRO
		dc.b '1.5.2019'
		ENDM

VERS	MACRO
		dc.b 'libarchive 3.2'
		ENDM

VSTRING	MACRO
		dc.b 'libarchive 3.2 (1.5.2019)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: libarchive 3.2 (1.5.2019)',0
		ENDM
