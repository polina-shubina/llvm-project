// RUN: clang -fplugin=../RenameVarPlugin.so -Xclang -plugin -Xclang rename-var -fsyntax-only %s | FileCheck %s

int a, b, c;

void func(int x, int y) {
    int p, q;
    static int s, t;
    p = a + b;
}

// CHECK: int global_a, global_b, global_c;
// CHECK: void func(int param_x, int param_y) {
// CHECK-NEXT:     int local_p, local_q;
// CHECK-NEXT:     static int static_s, static_t;
// CHECK-NEXT:     local_p = global_a + global_b;
// CHECK-NEXT: }