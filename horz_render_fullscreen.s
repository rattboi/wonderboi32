
	.ALIGN	2
	.GLOBAL horz_render_fullscreen
	.TYPE	horz_render_fullscreen,function

horz_render_fullscreen:
	mov r12, #320
	sub r12,r12,#1
	mov r11, #3
horz_loop:
	mov r3, r1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	ldrb r2,[r0],#1
	strb r2,[r3],#1
	strb r2,[r3],#1

	add r0,	r0,	#96
	add r1, r1, #240

	subs  r11,r11,#1
	subeq r0,r0,#240
	addeq r11,r11,#3

	subs r12,r12,#1
	bne horz_loop
	mov pc, lr
	
