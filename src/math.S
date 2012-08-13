
/*****************************************************************************/
#define	R0acc  R0
#define	R1acc  R1

#define	acc0 R2
#define	acc1 R3
#define	acc2 R4
#define	acc3 R5
#define	acc4 R6
#define	acc5 R7
#define	acc6 R8
#define	acc7 R9

#define	reg0 R10
#define	reg1 R11
#define	reg2 R12
#define	reg3 R13

#define	temp0 R14
#define	temp1 R15
#define	temp2 R16
#define	temp3 R17

#define	temp R18
#define	dummy R19
#define	glbcounter R20
#define	indxtmp R21
#define	glbpointer R22

#define	XL R26
#define	XH R27
#define	YL R28
#define	YH R29
#define	ZL R30
#define	ZH R31

/*****************************************************************************/
.text

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
R22:R25
*/


/*****************************************************************************/
.global math_div_64_32_r32
math_div_64_32_r32:

	push	R0acc
	push 	R1acc

	push 	acc0
	push 	acc1
	push 	acc2
	push 	acc3
	push 	acc4
	push 	acc5
	push 	acc6
	push 	acc7

	push 	reg0
	push 	reg1
	push 	reg2
	push 	reg3

	push 	temp0
	push 	temp1
	push 	temp2
	push 	temp3

	push	YL
	push	YL

	mov	acc0, R18
	mov	acc2, R20
	mov	acc1, R19
	mov	acc3, R21
	mov	acc4, R22
	mov	acc5, R23
	mov	acc6, R24
	mov	acc7, R25

	mov	reg0, R14
	mov	reg1, R15
	mov	reg2, R16
	mov	reg3, R17

/*****************************************************************************/
	clr	temp0
	clr	temp1
	clr	temp2
	clr	temp3
	ldi	glbcounter, 64

udi1:
	lsl	acc0
	rol	acc1
	rol	acc2
	rol	acc3
	rol	acc4
	rol	acc5
	rol	acc6
	rol	acc7

	rol	temp0
	rol	temp1
	rol	temp2
	rol	temp3

	cp	temp0, reg0
	cpc	temp1, reg1
	cpc	temp2, reg2
	cpc	temp3, reg3

	brcs udi2

	sub	temp0, reg0
	sbc	temp1, reg1
	sbc	temp2, reg2
	sbc	temp3, reg3

	inc	acc0

udi2:
	dec	glbcounter
	brne udi1

/*****************************************************************************/
	mov	R22, acc0
	mov	R23, acc1
	mov	R24, acc2
	mov	R25, acc3

	pop	YL
	pop	YL
	pop 	temp3
	pop 	temp2			
	pop 	temp1
	pop 	temp0

	pop 	reg3
	pop 	reg2
	pop 	reg1
	pop 	reg0

	pop 	acc7
	pop 	acc6
	pop 	acc5
	pop 	acc4
	pop 	acc3
	pop 	acc2
	pop 	acc1
	pop 	acc0

	pop 	R1acc
	pop	R0acc

	ret

/*****************************************************************************/
.global math_mul_32_32_r64
math_mul_32_32_r64:

	push	R0acc
	push 	R1acc

	push 	acc0
	push 	acc1
	push 	acc2
	push 	acc3
	push 	acc4
	push 	acc5
	push 	acc6
	push 	acc7

	push 	reg0
	push 	reg1
	push 	reg2
	push 	reg3

	push 	temp0
	push 	temp1
	push 	temp2
	push 	temp3

	push	YL
	push	YL

	mov	R25, temp0
	mov	R24, temp1
	mov	R23, temp2
	mov	R22, temp3

	mov	R21, reg0
	mov	R20, reg1
	mov	R19, reg2
	mov	R18, reg3

/*****************************************************************************/

	clr	acc7
	clr	acc6
	clr	acc5
	clr	acc4
	clr	acc3
	clr	acc2
	clr	acc1
	clr	acc0

	clr	dummy

//0. round
	mul	reg0, temp0
	add	acc0, R0
	adc	acc1, R1
	adc	acc2, dummy
	adc	acc3, dummy
	adc	acc4, dummy
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg1, temp0
	add	acc1, R0
	adc	acc2, R1
	adc	acc3, dummy
	adc	acc4, dummy
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg2, temp0
	add	acc2, R0
	adc	acc3, R1
	adc	acc4, dummy
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy		

	mul	reg3, temp0
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

//1. round
	mul	reg0, temp1
	add	acc1, R0
	adc	acc2, R1
	adc	acc3, dummy
	adc	acc4, dummy
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg1, temp1
	add	acc2, R0
	adc	acc3, R1
	adc	acc4, dummy
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg2, temp1
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg3, temp1
	add	acc4, R0
	adc	acc5, R1
	adc	acc6, dummy
	adc	acc7, dummy

// 2. round
	mul	reg0, temp2
	add	acc2, R0
	adc	acc3, R1
	adc	acc4, dummy
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg1, temp2
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg2, temp2
	add	acc4, R0
	adc	acc5, R1
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg3, temp2
	add	acc5, R0
	adc	acc6, R1
	adc	acc7, dummy

// 3. round
	mul	reg0, temp3
	add	acc3, R0
	adc	acc4, R1
	adc	acc5, dummy
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg1, temp3
	add	acc4, R0
	adc	acc5, R1
	adc	acc6, dummy
	adc	acc7, dummy

	mul	reg2, temp3
	add	acc5, R0
	adc	acc6, R1
	adc	acc7, dummy

	mul	reg3, temp3
	add	acc6, R0
	adc	acc7, R1

/*****************************************************************************/

	mov	R18, acc0
	mov	R19, acc1
	mov	R20, acc2
	mov	R21, acc3
	mov	R22, acc4
	mov	R23, acc5
	mov	R24, acc6
	mov	R25, acc7

	pop	YL
	pop	YL
	pop 	temp3
	pop 	temp2			
	pop 	temp1
	pop 	temp0

	pop 	reg3
	pop 	reg2
	pop 	reg1
	pop 	reg0

	pop 	acc7
	pop 	acc6
	pop 	acc5
	pop 	acc4
	pop 	acc3
	pop 	acc2
	pop 	acc1
	pop 	acc0

	pop 	R1acc
	pop	R0acc

	ret

