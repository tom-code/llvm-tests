
@str = private constant [14 x i8] c"Hello, World!\00"
declare i32 @puts(i8* nocapture) nounwind

define i32 @my_test_func(i32 %x) {
entry:



  %cast1 =  getelementptr [14 x i8], [14 x i8]* @str, i32 0, i32 0
  %0 = call i32 @puts(i8* %cast1)
  %tmp = add i32 10, %x
  %tmp2 = add i32 10, %tmp
  ret i32 %tmp2
}
