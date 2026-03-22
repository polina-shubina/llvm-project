; RUN: opt -load-pass-plugin=%shlibdir/Mul2ShiftPass.so -passes=mul2shift -S %s | FileCheck %s

define i32 @test(i32 %x) {
  %y = mul i32 %x, 7
  ret i32 %y
}

; CHECK: %y = mul i32 %x, 7
