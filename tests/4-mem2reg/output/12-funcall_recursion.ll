; ModuleID = 'cminus'
source_filename = "/home/hwc/Desktop/2023ustc-jianmu-compiler/tests/testcases_general/12-funcall_recursion.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @factorial(i32 %arg0) {
label_entry:
  %op1 = icmp eq i32 %arg0, 0
  br i1 %op1, label %label2, label %label3
label2:                                                ; preds = %label_entry
  ret i32 1
label3:                                                ; preds = %label_entry
  %op4 = sub i32 %arg0, 1
  %op5 = call i32 @factorial(i32 %op4)
  %op6 = mul i32 %arg0, %op5
  ret i32 %op6
label7:
  ret i32 0
}
define i32 @main() {
label_entry:
  %op0 = call i32 @factorial(i32 10)
  ret i32 %op0
}
