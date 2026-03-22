#ifndef LLVM_TRANSFORMS_MUL2SHIFT_MUL2SHIFT_H
#define LLVM_TRANSFORMS_MUL2SHIFT_MUL2SHIFT_H

#include "llvm/IR/PassManager.h"

namespace llvm {
class Mul2ShiftPass : public PassInfoMixin<Mul2ShiftPass> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} // namespace llvm

#endif

