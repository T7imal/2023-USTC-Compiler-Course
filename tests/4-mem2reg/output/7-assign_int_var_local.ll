; ModuleID = 'cminus'
source_filename = "/home/hwc/Desktop/2023ustc-jianmu-compiler/tests/testcases_general/7-assign_int_var_local.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  ret i32 1234
}
