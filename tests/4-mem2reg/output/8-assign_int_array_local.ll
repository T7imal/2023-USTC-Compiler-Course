; ModuleID = 'cminus'
source_filename = "/home/hwc/Desktop/2023ustc-jianmu-compiler/tests/testcases_general/8-assign_int_array_local.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  %op0 = icmp sle i32 3, 0
  br i1 %op0, label %label1, label %label2
label1:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label2
label2:                                                ; preds = %label_entry, %label1
  %op3 = icmp sle i32 3, 0
  br i1 %op3, label %label4, label %label5
label4:                                                ; preds = %label2
  call void @neg_idx_except()
  br label %label5
label5:                                                ; preds = %label2, %label4
  ret i32 1234
}
