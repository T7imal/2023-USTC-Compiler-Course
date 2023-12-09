; ModuleID = 'cminus'
source_filename = "/home/hwc/Desktop/2023ustc-jianmu-compiler/tests/testcases_general/11-funcall_chain.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @addone(i32 %arg0) {
label_entry:
  %op1 = add i32 %arg0, 1
  ret i32 %op1
}
define i32 @main() {
label_entry:
  %op0 = call i32 @addone(i32 1230)
  %op1 = call i32 @addone(i32 %op0)
  %op2 = call i32 @addone(i32 %op1)
  %op3 = call i32 @addone(i32 %op2)
  ret i32 %op3
}
