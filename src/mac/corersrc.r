#ifdef __UNIX__
  #include <Carbon.r>
#else
  #include <Types.r>
  #if UNIVERSAL_INTERFACES_VERSION > 0x320
    #include <ControlDefinitions.r>
  #endif
#endif

resource 'ldes' ( 128 )
{
	versionZero 
	{
		0 , 
		0 ,
		0 , 
		0 , 
		hasVertScroll , 
		noHorizScroll , 
		0 , 
		noGrowSpace , 
	}
} ;

resource 'ldes' ( 129 )
{
	versionZero 
	{
		0 , 
		0 ,
		0 , 
		0 , 
		hasVertScroll , 
		hasHorizScroll , 
		0 , 
		noGrowSpace , 
	}
} ;

data 'CURS' (10) {
	$"0000 03E0 0630 0808 1004 31C6 2362 2222"
	$"2362 31C6 1004 0808 0630 03E0 0000 0000"
	$"0000 03E0 07F0 0FF8 1FFC 3FFE 3FFE 3FFE"
	$"3FFE 3FFE 1FFC 0FF8 07F0 03E0 0000 0000"
	$"0007 0008"
};

data 'CURS' (11) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000"
};

data 'CURS' (12) {
	$"00F0 0088 0108 0190 0270 0220 0440 0440"
	$"0880 0880 1100 1E00 1C00 1800 1000 0000"
	$"00F0 00F8 01F8 01F0 03F0 03E0 07C0 07C0"
	$"0F80 0F80 1F00 1E00 1C00 1800 1000 0000"
	$"000E 0003"
};

data 'CURS' (13) {
	$"0000 1E00 2100 4080 4080 4080 4080 2180"
	$"1FC0 00E0 0070 0038 001C 000E 0006 0000"
	$"3F00 7F80 FFC0 FFC0 FFC0 FFC0 FFC0 7FC0"
	$"3FE0 1FF0 00F8 007C 003E 001F 000F 0007"
	$"0004 0004"
};

data 'CURS' (14) {
	$"0000 07E0 1FF0 3838 3C0C 6E0E 6706 6386"
	$"61C6 60E6 7076 303C 1C1C 0FF8 07E0 0000"
	$"0540 0FF0 3FF8 3C3C 7E0E FF0F 6F86 E7C7"
	$"63E6 E1F7 70FE 707E 3C3C 1FFC 0FF0 0540"
	$"0007 0007"
};

data 'CURS' (15) {
	$"0000 0380 0380 0380 0380 0380 0380 0FE0"
	$"1FF0 1FF0 0000 1FF0 1FF0 1550 1550 1550"
	$"07C0 07C0 07C0 07C0 07C0 07C0 0FE0 1FF0"
	$"3FF8 3FF8 3FF8 3FF8 3FF8 3FF8 3FF8 3FF8"
	$"000B 0007"
};

data 'CURS' (16) {
	$"00C0 0140 0640 08C0 3180 47FE 8001 8001"
	$"81FE 8040 01C0 0040 03C0 C080 3F80 0000"
	$"00C0 01C0 07C0 0FC0 3F80 7FFE FFFF FFFF"
	$"FFFE FFC0 FFC0 FFC0 FFC0 FF80 3F80 0000"
	$"0006 000F"
};

data 'CURS' (17) {
	$"0100 0280 0260 0310 018C 7FE3 8000 8000"
	$"7F80 0200 0380 0200 03C0 0107 01F8 0000"
	$"0100 0380 03E0 03F0 01FC 7FFF FFFF FFFF"
	$"FFFF 03FF 03FF 03FF 03FF 01FF 01F8 0000"
	$"0006 0000"
};

data 'CURS' (18) {
	$"0000 4078 60FC 71CE 7986 7C06 7E0E 7F1C"
	$"7FB8 7C30 6C30 4600 0630 0330 0300 0000"
	$"C078 E0FC F1FE FBFF FFCF FF8F FF1F FFBE"
	$"FFFC FE78 FF78 EFF8 CFF8 87F8 07F8 0300"
	$"0001 0001"
};

data 'CURS' (19) {
	$"0000 0002 0006 000E 001E 003E 007E 00FE"
	$"01FE 003E 0036 0062 0060 00C0 00C0 0000"
	$"0003 0007 000F 001F 003F 007F 00FF 01FF"
	$"03FF 07FF 007F 00F7 00F3 01E1 01E0 01C0"
	$"0001 000E"
};

data 'CURS' (20) {
	$"0000 0080 01C0 03E0 0080 0080 0080 1FFC"
	$"1FFC 0080 0080 0080 03E0 01C0 0080 0000"
	$"0080 01C0 03E0 07F0 0FF8 01C0 3FFE 3FFE"
	$"3FFE 3FFE 01C0 0FF8 07F0 03E0 01C0 0080"
	$"0007 0008"
};

data 'CURS' (21) {
	$"0000 0080 01C0 03E0 0080 0888 188C 3FFE"
	$"188C 0888 0080 03E0 01C0 0080 0000 0000"
	$"0080 01C0 03E0 07F0 0BE8 1DDC 3FFE 7FFF"
	$"3FFE 1DDC 0BE8 07F0 03E0 01C0 0080 0000"
	$"0007 0008"
};

data 'CURS' (22) {
	$"0000 001E 000E 060E 0712 03A0 01C0 00E0"
	$"0170 1238 1C18 1C00 1E00 0000 0000 0000"
	$"007F 003F 0E1F 0F0F 0F97 07E3 03E1 21F0"
	$"31F8 3A7C 3C3C 3E1C 3F00 3F80 0000 0000"
	$"0006 0009"
};

data 'CURS' (23) {
	$"0000 7800 7000 7060 48E0 05C0 0380 0700"
	$"0E80 1C48 1838 0038 0078 0000 0000 0000"
	$"FE00 FC00 F870 F0F0 E9F0 C7E0 87C0 0F84"
	$"1F8C 3E5C 3C3C 387C 00FC 01FC 0000 0000"
	$"0006 0006"
};

data 'CURS' (24) {
	$"0006 000E 001C 0018 0020 0040 00F8 0004"
	$"1FF4 200C 2AA8 1FF0 1F80 3800 6000 8000"
	$"000F 001F 003E 007C 0070 00E0 01FC 3FF6"
	$"7FF6 7FFE 7FFC 7FF8 3FF0 7FC0 F800 E000"
	$"000A 0006"
};

resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 0041 C000 00A2 F000 03A5 FC00"
		$"064B FF00 1CD7 FFC0 F1FF FFF0 E000 FFFC"
		$"80FF FFFF 80FF FFFF 803F FFFE 80FF FFFE"
		$"803F FFFA C0FF FFF6 7FFF FFEE 5FFF FFDE"
		$"FFFF FFBE CFFF FF7F F3FF FEFF E4FF FDFE"
		$"E13F FBFC E04F F7F8 E013 EFF0 7804 DFE0"
		$"1E01 3FC0 0780 3F80 01E0 3F00 0078 3E00"
		$"001E 3C00 0007 B800 0001 F000 0000 60",
		/* [2] */
		$"0000 0000 0041 C000 00E3 F000 03E7 FC00"
		$"07CF FF00 1FDF FFC0 FFFF FFF0 FFFF FFFC"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF 7FFF FFFF 7FFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFE"
		$"FFFF FFFC FFFF FFF8 FFFF FFF0 7FFF FFE0"
		$"1FFF FFC0 07FF FF80 01FF FF00 007F FE00"
		$"001F FC00 0007 F800 0001 F000 0000 60"
	}
};

resource 'ics#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"09C0 15F0 EBFC 8EFF AFFE 97FE DFFE EFFA"
		$"BFE7 8BE6 C5DC 30D8 1C30 0760 03C0",
		/* [2] */
		$"09C0 1DF0 FFFC FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFE FFFC 3FF8 0FF0 03E0 02C0"
	}
};

resource 'ics4' (128) {
	$"0000 EC0D 6FC0 0000 00DE BDD6 666F C000"
	$"DEBB EE65 6666 6FC0 E2CD BEBB 6666 666E"
	$"EC2C A566 6666 6660 E1C2 A666 6666 66D0"
	$"BDBD E666 6666 6DC0 FFF6 6666 6666 5CC0"
	$"ECDF 6666 666D 0DFF FC0C DF66 6650 DFFC"
	$"5DC0 0CDF 6D0D FFC0 0C75 C00C D0DF FC00"
	$"000D 6DC0 0DFF C000 0000 CE65 CFFC 0000"
	$"0000 00DD 6FC0"
};

resource 'ics8' (128) {
	$"0000 00F5 81F7 017F EBF1 2BF5 0000 0000"
	$"0000 F888 8256 7FD2 D2D2 ECF1 F800 0000"
	$"F9FB 5E5E FD82 EFB0 ECD2 D2D2 ECF1 F7F5"
	$"AD5D 3333 5E87 5E82 D3D2 D2D2 D2D2 EBFD"
	$"820F 3333 ACD5 D2EB D2D2 D2D2 D2D2 D2F5"
	$"8808 3333 5FEF D2D2 D2D2 D2D2 D2D2 8000"
	$"FB5E 5D5D D5D2 D2D2 D2D2 D2D2 D280 F600"
	$"FDFF D6EC D2D2 D2D2 D2D2 D2D2 802B 2BF5"
	$"FEF7 81F4 EFEB D2D2 D2D2 D280 F6F9 FFFD"
	$"FEF6 002B 57FE F0EB D2D2 80F6 56FF FFF7"
	$"7EAA F600 00F7 57F3 EC80 2B56 FFFF F700"
	$"0031 CD86 F600 00F7 FA2B 56FF FFF7 0000"
	$"0000 F57A EB80 F600 0056 FFFF F700 0000"
	$"0000 0000 2AAB EC80 2BFF FFF7 0000 0000"
	$"0000 0000 0000 5BAB F0FD F700 0000 0000"
	$"0000 0000 0000 0000 01"
};



