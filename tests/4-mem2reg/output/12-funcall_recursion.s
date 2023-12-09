	.text
	.globl factorial
	.type factorial, @function
factorial:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -48
	st.w $a0, $fp, -20
.factorial_label_entry:
# %op1 = icmp eq i32 %arg0, 0
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 0
	sub.w $t2, $t0, $t1
	sltui $t2, $t2, 1
	st.b $t2, $fp, -21
# br i1 %op1, label %label2, label %label3
	ld.b $t0, $fp, -21
	bnez $t0, .factorial_label2
	b .factorial_label3
.factorial_label2:
# ret i32 1
	addi.w $a0, $zero, 1
	b factorial_epilogue
.factorial_label3:
# %op4 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -28
# %op5 = call i32 @factorial(i32 %op4)
	ld.w $a0, $fp, -28
	bl factorial
	st.w $a0, $fp, -32
# %op6 = mul i32 %arg0, %op5
	ld.w $t0, $fp, -20
	ld.w $t1, $fp, -32
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -36
# ret i32 %op6
	ld.w $a0, $fp, -36
	b factorial_epilogue
.factorial_label7:
# ret i32 0
	addi.w $a0, $zero, 0
	b factorial_epilogue
factorial_epilogue:
	addi.d $sp, $sp, 48
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# %op0 = call i32 @factorial(i32 10)
	addi.w $a0, $zero, 10
	bl factorial
	st.w $a0, $fp, -20
# ret i32 %op0
	ld.w $a0, $fp, -20
	b main_epilogue
main_epilogue:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra