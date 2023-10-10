; ModuleID = '../c_cases/assign.c'

define dso_local i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 0
  store i32 10, i32* %2, align 16
  %3 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 1
  %4 = load i32, i32* %2, align 16
  %5 = mul i32 %4, 2
  store i32 %5, i32* %3, align 16
  %6 = load i32, i32* %3, align 16
  ret i32 %6
}