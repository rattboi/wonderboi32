
	.ALIGN 2
	.ARM
	
	.GLOBAL		WSCBmp_horz
	.GLOBAL		WSCBmp_vert
	.GLOBAL		WBTitle
	
WSCBmp_horz:
	.INCBIN "wsc_horz.raw"
	
WSCBmp_vert:
	.INCBIN "wsc_vert.raw"

WBTitle:
	.INCBIN "title.raw"

