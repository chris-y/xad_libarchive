VERSION		EQU	3
REVISION	EQU	1

DATE	MACRO
		dc.b '28.4.2019'
		ENDM

VERS	MACRO
		dc.b 'libarchive 3.1'
		ENDM

VSTRING	MACRO
		dc.b 'libarchive 3.1 (28.4.2019)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: libarchive 3.1 (28.4.2019)',0
		ENDM
