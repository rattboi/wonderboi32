	.ALIGN	2
	.GLOBAL horz_render_hvstretch
	.TYPE	horz_render_hvstretch,function

horz_render_hvstretch:
	mov r12, #215
	mov r4, #3
	add r1, r1, #480
horz_loop:
	sub r1, r1, #2
	mov r3, r1

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #1 (12 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480

@ #27 (8 pixels)
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	
	add r0,r0,#10*2+(16*2)

	subs  r4,r4,#1
	subeq r0,r0,#224+16
	subeq r0,r0,#224+16
	addeq r4,r4,#3
		
	subs r12,r12,#1
	bne horz_loop
	mov pc, lr
	
