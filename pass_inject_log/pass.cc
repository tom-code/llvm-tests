#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstIterator.h"

#include <cxxabi.h>

using namespace llvm;

struct p1_t : public ModulePass {
  static char ID;

  p1_t() : ModulePass(ID) {
    printf("pass init\n");
  }

  std::string demangle(std::string in) {
    std::string out = in;
    int status;
    const char *n = abi::__cxa_demangle(in.c_str(), 0, 0, &status);
    if (status != 0) return out;
    return n;
  }

  virtual bool runOnModule(Module &mod) {
    printf("running pass on module %s\n", mod.getModuleIdentifier().c_str());


    Constant *print_c  = mod.getOrInsertFunction("print", Type::getVoidTy(mod.getContext()), Type::getInt8PtrTy(mod.getContext()));
    Function *print_func = cast<Function>(print_c);


    IRBuilder<> builder(mod.getContext());

    for (auto &fun : mod) {
      if (fun.isIntrinsic()) continue;
      if (fun.isDeclaration()) continue;
      if (fun.getName() == "print") continue;

      printf("  processing function %s\n", fun.getName());

      //insert string with enter and leave message into IR
      char message[128];
      builder.SetInsertPoint(&*fun.begin());
      sprintf(message, "<enter> %s", demangle(fun.getName()).c_str());
      Value *msg = builder.CreateGlobalStringPtr(message);
      sprintf(message, "<leave> %s", demangle(fun.getName()).c_str());
      Value *msg_leave = builder.CreateGlobalStringPtr(message);

      { // insert print at begin of function
        std::vector<Value *> args;
        args.push_back(msg);
        ArrayRef<Value *> args_ref(args);
        Instruction *inst_print = CallInst::Create(print_func, args_ref);

        fun.begin()->getInstList().insert(fun.begin()->begin(), inst_print);
      }

      //find all "ret" instructions and insert print just before them
      for (inst_iterator inst = inst_begin(fun), E = inst_end(fun); inst != E; ++inst) {
        if (strcmp(inst->getOpcodeName(), "ret") == 0) {
          std::vector<Value *> args;
          args.push_back(msg_leave);
          ArrayRef<Value *> args_ref(args);
          Instruction *inst_print = CallInst::Create(print_func, args_ref);

          inst_print->insertBefore(&*inst);
        }
        //printf(">> %s\n", I->getOpcodeName());
        //errs() << *I << "\n";
      }
    }

    return false;
  }
  

};

char p1_t::ID = 0;
//static RegisterPass<p1_t> p1("p1", "p1",false);


static void regme(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new p1_t());
}
static RegisterStandardPasses reg(PassManagerBuilder::EP_ModuleOptimizerEarly, regme);

