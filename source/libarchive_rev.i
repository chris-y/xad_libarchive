VERSION		EQU	3
REVISION	EQU	3

DATE	MACRO
		dc.b '20.6.2019'
		ENDM

VERS	MACRO
		dc.b 'libarchive 3.3'
		ENDM

VSTRING	MACRO
		dc.b 'libarchive 3.3 (20.6.2019)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: libarchive 3.3 (20.6.2019)',0
		ENDM
