; ModuleID = 'out'
source_filename = "out"

define i32 @add(i32 %0, i32 %1) {
entry:
  %y = alloca i32, align 4
  store i32 9, ptr %y, align 4
  %l_y = load i32, ptr %y, align 4
  %eqtemp = icmp eq i32 %l_y, 9
  br i1 %eqtemp, label %if1then, label %if1else

if1then:                                          ; preds = %entry
  store i32 0, ptr %y, align 4
  br label %merge
  br label %merge

if1else:                                          ; preds = %entry

merge:                                            ; preds = %if1then, %if1then
}

define i32 @main(i32 %0) {
entry:
  %x = alloca i32, align 4
  store i32 10, ptr %x, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %loop.cond

loop.cond:                                        ; preds = %loop.body, %entry
  %i_phi = phi i32 [ 0, %entry ], [ %addtemp2, %loop.body ]
  %l_i = load i32, ptr %i, align 4
  %lttemp = icmp ult i32 %l_i, 10
  br i1 %lttemp, label %loop.body, label %loop.end

loop.body:                                        ; preds = %loop.cond
  %l_x = load i32, ptr %x, align 4
  %addtemp = add i32 %l_x, 1
  store i32 %addtemp, ptr %x, align 4
  %l_i1 = load i32, ptr %i, align 4
  %addtemp2 = add i32 %l_i1, 1
  store i32 %addtemp2, ptr %i, align 4
  br label %loop.cond

loop.end:                                         ; preds = %loop.cond
  ret i32 0
}
