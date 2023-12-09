; ModuleID = 'cminus'
source_filename = "/home/hwc/Desktop/2023ustc-jianmu-compiler/tests/testcases_general/14-while_stmt.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  %op0 = alloca i32
  br label %label1
label1:                                                ; preds = %label_entry, %label4
  %op2 = phi i32 [ 10, %label_entry ], [ undef, %label4 ]
  %op3 = icmp ne i32* %op2, i32 0
  br i1 %op3, label %label4, label %label6
label4:                                                ; preds = %label1
  %op5 = sub i32* %op2, i32 1
  store i32 %op5, i32* %op0
  br label %label1
label6:                                                ; preds = %label1
  ret void
}
