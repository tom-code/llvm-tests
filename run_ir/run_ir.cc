
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Passes/PassBuilder.h"



//dirty me - copied from LambdaResolver.h and changed shared_ptr->unique_ptr
namespace llvm {
namespace orc {
template <typename DylibLookupFtorT,
          typename ExternalLookupFtorT>
std::unique_ptr<LambdaResolver<DylibLookupFtorT, ExternalLookupFtorT>>
  createLambdaResolverX(DylibLookupFtorT DylibLookupFtor, ExternalLookupFtorT ExternalLookupFtor) {
  using LR = LambdaResolver<DylibLookupFtorT, ExternalLookupFtorT>;
  return make_unique<LR>(std::move(DylibLookupFtor),
                         std::move(ExternalLookupFtor));
}
}
}


using namespace llvm;

//my test function
static void printer(char *data) {
  printf(">>>>>>>>>> %s\n", data);
}

/*  how to fix this ???
void optimize(std::unique_ptr<Module> &module) {
  PassBuilder pb;
  auto pm = pb.buildModuleOptimizationPipeline(PassBuilder::OptimizationLevel::O3, true);
  llvm::AnalysisManager<Module> mm(true);
  pm.run(*(module.get()), mm);
}*/

void optimize(std::unique_ptr<Module> &module) {
  PassBuilder pb;
  legacy::PassManager pm;
  PassManagerBuilder builder;
  builder.OptLevel = 3;
  builder.populateModulePassManager(pm);
  pm.run(*module);
}

int main() {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  LLVMContext Context;
  SMDiagnostic Err;

  std::unique_ptr<Module> module = parseIRFile("a.ll", Err, Context);

  if (!module) {
    Err.print("testor", errs());
    return 1;
  }

  optimize(module);
  //dump parsed and optimized module
  //errs() << *(module.get());

  if (verifyModule(*(module.get()))) {
    errs() <<  ": verify module failed\n";
    return 1;
  }

  auto Resolver = orc::createLambdaResolverX(
                      [&](const std::string &name) {
                        printf("symbol %s\n", name.c_str());
                        return nullptr;
                      },
                      [](const std::string &symbol) {
                        printf("resolving symbol: %s\n", symbol.c_str());
                        //if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(S))
                        //  return JITSymbol(SymAddr, JITSymbolFlags::Exported);
                        //return JITSymbol(nullptr);
                        if (symbol == "puts")
                          return JITSymbol((unsigned long)(void*)&printer, JITSymbolFlags::Exported);
                        return JITSymbol(nullptr);
                      });



  std::string errStr;
  ExecutionEngine *EE =
    EngineBuilder(std::move(module))
    .setEngineKind(EngineKind::JIT)
    .setErrorStr(&errStr)
    .setSymbolResolver(std::move(Resolver))
    .create();

  if (!EE) {
    errs() << ": Failed to construct ExecutionEngine: " << errStr << "\n";
    return 1;
  }

  //Function *func = module->getFunction("my_test_func");
  Function *func = EE->FindFunctionNamed("my_test_func");


  std::vector<GenericValue> Args(1);
  Args[0].IntVal = APInt(32, 10);
  GenericValue GV = EE->runFunction(func, Args);
  outs() << "Result: " << GV.IntVal << "\n";

  return 0;
}

