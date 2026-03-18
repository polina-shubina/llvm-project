// RUN: clang -fplugin=../RenameVarPlugin.so -Xclang -plugin -Xclang rename-var -fsyntax-only %s | FileCheck %s

typedef int my_int;        // тип

#define VALUE 42           // макрос

void function_name() {     // функция
    int local_var = VALUE;
}

// CHECK: typedef int my_int;
// CHECK: #define VALUE 42
// CHECK: void function_name() {
// CHECK-NEXT:     int local_var = VALUE;
// CHECK-NEXT: }