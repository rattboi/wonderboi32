
	.ALIGN	2
	.GLOBAL horz_render_normal
	.TYPE	horz_render_normal,function

horz_render_normal:
	mov r12, #144
	add r1, r1, #480
horz_loop:
	sub r1, r1, #2
	mov r3, r1

@ #1
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #2
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #3
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #4
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #5
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #6
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #7
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #8
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #9
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #10
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #11
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #12
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #13
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #14
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #15
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #16
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #17
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #18
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #19
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #20
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #21
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #22
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #23
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #24
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #25
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #26
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #27
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480

@ #28
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	ldr r2,[r0],#4+(16*2)
	strh r2,[r3]
	add r3,r3,#480
	mov r2,r2,lsr #16
	strh r2,[r3]
	add r3,r3,#480
	
	subs r12,r12,#1
	bne horz_loop
	mov pc, lr
	