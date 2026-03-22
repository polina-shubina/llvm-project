; RUN: opt -load-pass-plugin=%shlibdir/Mul2ShiftPass.so -passes=mul2shift -S %s | FileCheck %s

define i64 @test(i64 %x) {
  %y = mul i64 %x, 32
  ret i64 %y
}

; CHECK: %y = shl i64 %x, 5
