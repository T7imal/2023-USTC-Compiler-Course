	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $fp, -24
# %op1 = icmp ne i32 2, 0
	addi.w $t0, $zero, 2
	addi.w $t1, $zero, 0
	sub.w $t2, $t0, $t1
	sltu $t2, $zero, $t2
	st.b $t2, $fp, -29
# br i1 %op1, label %label2, label %label3
	ld.b $t0, $fp, -29
	bnez $t0, .main_label2
	b .main_label3
.main_label2:
# store i32 3, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 3
	st.w $t1, $t0, 0
# br label %label4
	b .main_label4
.main_label3:
# br label %label4
	b .main_label4
.main_label4:
# store i32 4, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 4
	st.w $t1, $t0, 0
# ret void
	addi.d $a0, $zero, 0
	b main_epilogue
main_epilogue:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
