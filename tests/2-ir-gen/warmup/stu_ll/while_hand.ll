; ModuleID = '../c_cases/while.c'

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 10, i32* %a, align 4
  store i32 0, i32* %i, align 4
  br label %while.cond

while.cond: 
  %i1 = load i32, i32* %i, align 4
  %2 = icmp slt i32 %i1, 10
  br i1 %2, label %while.body, label %while.end

while.body:
  %i2 = load i32, i32* %i, align 4
  %i3 = add i32 %i2, 1
  store i32 %i3, i32* %i, align 4
  %a1 = load i32, i32* %a, align 4
  %a2 = add i32 %a1, %i3
  store i32 %a2, i32* %a, align 4
  br label %while.cond

while.end:
  %a3 = load i32, i32* %a, align 4
  ret i32 %a3
}