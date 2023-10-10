; ModuleID = '../c_cases/if.c'

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca float, align 4
  store i32 0, i32* %1, align 4
  store float 0x40163851E0000000, float* %2, align 4
  %a1 = load float, float* %2, align 4
  %3 = fcmp ogt float %a1, 1.000000e+00
  br i1 %3, label %4, label %6

4:                                                ; preds = %0
  store i32 233, i32* %1, align 4
  %5 = load i32, i32* %1, align 4
  ret i32 %5

6:                                                ; preds = %0
  store i32 0, i32* %1, align 4
  %7 = load i32, i32* %1, align 4
  ret i32 %7
}