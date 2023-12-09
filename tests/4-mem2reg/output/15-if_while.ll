; ModuleID = 'cminus'
source_filename = "/home/hwc/Desktop/2023ustc-jianmu-compiler/tests/testcases_general/15-if_while.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  %op2 = alloca i32
  br label %label3
label3:                                                ; preds = %label_entry, %label17
  %op4 = phi i32 [ 0, %label_entry ], [ undef, %label17 ]
  %op5 = phi i32 [ 10, %label_entry ], [ undef, %label17 ]
  %op6 = phi i32 [ 0, %label_entry ], [ undef, %label17 ]
  %op7 = icmp ne i32* %op5, i32 0
  br i1 %op7, label %label8, label %label11
label8:                                                ; preds = %label3
  %op9 = sub i32* %op5, i32 1
  store i32 %op9, i32* %op0
  %op10 = icmp slt i32 %op9, 5
  br i1 %op10, label %label13, label %label15
label11:                                                ; preds = %label3
  %op12 = add i32* %op4, %op6
  ret i32 %op12
label13:                                                ; preds = %label8
  %op14 = add i32* %op4, i32 %op9
  store i32 %op14, i32* %op1
  br label %label17
label15:                                                ; preds = %label8
  %op16 = add i32* %op6, i32 %op9
  store i32 %op16, i32* %op2
  br label %label17
label17:                                                ; preds = %label13, %label15
  br label %label3
}
