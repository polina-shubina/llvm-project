// RUN: clang -fplugin=../clang/examples/RenameVarPlugin/RenameVarPlugin.so -Xclang -plugin -Xclang rename-var -fsyntax-only %s | FileCheck %s

int arr[10];
int *ptr;

void test(int param) {
    int local_arr[5];
    static int static_arr[3];
    local_arr[0] = ptr[1];
}

// CHECK: int global_arr[10];
// CHECK: int *global_ptr;
// CHECK: void test(int param_param) {
// CHECK-NEXT:     int local_arr[5];
// CHECK-NEXT:     static int static_arr[3];
// CHECK-NEXT:     local_arr[0] = global_ptr[1];
// CHECK-NEXT: }