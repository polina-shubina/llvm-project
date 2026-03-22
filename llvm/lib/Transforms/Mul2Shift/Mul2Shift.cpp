#include "llvm/Transforms/Mul2Shift/Mul2Shift.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static bool isPowerOfTwo(uint64_t n) {
    return n && !(n & (n - 1));
}

PreservedAnalyses Mul2ShiftPass::run(Function &F, FunctionAnalysisManager &AM) {
    bool Changed = false;

    for (auto &BB : F) {
        for (auto &I : BB) {
            if (auto *BO = dyn_cast<BinaryOperator>(&I)) {
                if (BO->getOpcode() == Instruction::Mul) {
                    Value *LHS = BO->getOperand(0);
                    Value *RHS = BO->getOperand(1);

                    if (auto *CI = dyn_cast<ConstantInt>(RHS)) {
                        uint64_t Val = CI->getZExtValue();
                        if (isPowerOfTwo(Val)) {
                            unsigned ShiftAmount = Log2_64(Val);
                            IRBuilder<> Builder(BO);
                            Value *Shift = Builder.CreateShl(LHS, ShiftAmount);
                            BO->replaceAllUsesWith(Shift);
                            BO->eraseFromParent();
                            Changed = true;
                        }
                    }
                }
            }
        }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
