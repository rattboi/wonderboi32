
	.ALIGN	2
	.GLOBAL horz_render_widescreen
	.TYPE	horz_render_widescreen,function

horz_render_widescreen:
	mov r12, #320
	sub r12,r12,#1
	mov r11, #3
horz_loop:
	mov r3, r1

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4
	str r2,[r3],#4

	ldr r2,[r0],#4
	str r2,[r3],#4
	ldr r2,[r0],#4+(96)
	str r2,[r3],#4

	add r1, r1, #240

	subs  r11,r11,#1
	subeq r0,r0,#240
	addeq r11,r11,#3

	subs r12,r12,#1
	bne horz_loop
	mov pc, lr
	
