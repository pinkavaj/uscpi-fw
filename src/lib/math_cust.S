#define	acc0 R18
#define	acc1 R19
#define	acc2 R20
#define	acc3 R21
#define	acc4 R22
#define	acc5 R23
#define	acc6 R24
#define	acc7 R25

#define	reg0 R14
#define	reg1 R15
#define	reg2 R16
#define	reg3 R17

#define	temp0 R26
#define	temp1 R27
#define	temp2 R30
#define	temp3 R31

#define	counter R30
#define zero R1
#define dummy R2
/*****************************************************************************/

.text

/*****************************************************************************/
.global math_div_64_32_r32
math_div_64_32_r32:
/*
; dividend [div7:div0] = [acc7:acc0]
; divisor  [reg3:reg0]
; temp     [temp3:temp0]
; result   [div3:div0] = [acc3:acc0]
;
*/

/*
IN: R25 -> R8	 ; start on even register
R25:R18
R17:R14

OUT:
R25:R22
*/

/*****************************************************************************/

	clr	temp0
	clr	temp1
	ldi	counter, 40

udiv1:
	lsl	acc0
	rol	acc1
	rol	acc2
	rol	acc3
	rol	acc4
	rol	acc5
	rol	acc6
	rol	acc7
	rol	temp0
	brcs	udiv_sub

	cp	acc5, reg0
	cpc	acc6, reg1
	cpc	acc7, reg2
	cpc	temp0, reg3

	brcs udiv2
udiv_sub:
	sub	acc5, reg0
	sbc	acc6, reg1
	sbc	acc7, reg2
	sbc	temp0, reg3

	inc	acc0

udiv2:
	dec	counter
	brne udiv1

	movw	r22,	acc0
	movw	r24,	acc2
/*****************************************************************************/
	ret

/*****************************************************************************/

.global math_mul_32_32_r64
math_mul_32_32_r64:

	push 	reg0
	push 	reg1
	push 	reg2
	push 	reg3

	push dummy

	movw	temp0, R22
	movw	temp2, R24

	movw	reg0, R18
	movw	reg2, R20

/*****************************************************************************/

	clr	acc0
	clr	acc1
	movw	acc2, acc0
	movw	acc4, acc0
	movw	acc6, acc0

	clr	dummy

// 0x0
	mul	reg0, temp0
	movw	acc0, R0
// 0x1
	mul	reg0, temp1
	add	acc1, R0
	adc	acc2, R1
// 1x0
	mul	reg1, temp0
	add	acc1, R0
	adc	acc2, R1
	adc	acc3, dummy
// 0x2
	mul	reg0, temp2
	add	acc2, R0
	adc	acc3, R1
// 1x1
	mul	reg1, temp1
	add	acc2, R0
	adc	acc3, R1
	adc	acc4, dummy
// 2x0
	mul	reg2, temp0
	add	acc2, R0
	adc	acc3, R1
	adc	acc4, dummy
// 0x3
	mul	reg0, temp3
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
// 1x2
	mul	reg1, temp2
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
// 2x1
	mul	reg2, temp1
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
// 3x0
	mul	reg3, temp0
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
// 1x3
	mul	reg1, temp3
	add	acc4, R0
	adc	acc5, R1
	adc	acc6, dummy
// 2x2
	mul	reg2, temp2
	add	acc4, R0
	adc	acc5, R1
	adc	acc6, dummy
// 3x1
	mul	reg3, temp1
	add	acc4, R0
	adc	acc5, R1
	adc	acc6, dummy
// 2x3
	mul	reg2, temp3
	add	acc5, R0
	adc	acc6, R1
	adc	acc7, dummy
// 3x2
	mul	reg3, temp2
	add	acc5, R0
	adc	acc6, R1
	adc	acc7, dummy
// 3x3
	mul	reg3, temp3
	add	acc6, R0
	adc	acc7, R1


/*****************************************************************************/
	pop	dummy

	pop 	reg3
	pop 	reg2
	pop 	reg1
	pop 	reg0

	clr	r1

	ret

