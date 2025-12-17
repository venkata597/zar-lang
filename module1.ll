; ModuleID = 'module1'
source_filename = "module1"

define i32 @main() {
entry:
  %num = alloca i32, align 4
  store i32 5, ptr %num, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  %l_num = load i32, ptr %num, align 4
  %eqtemp = icmp eq i32 %l_num, 5
  br i1 %eqtemp, label %if1then, label %if1else

if1then:                                          ; preds = %entry
  %l_num1 = load i32, ptr %num, align 4
  store i32 %l_num1, ptr %a, align 4
  br label %merge
  br label %merge

if1else:                                          ; preds = %entry

merge:                                            ; preds = %if1then, %if1then
  ret i32 0
}
