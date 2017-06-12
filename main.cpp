#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;
using namespace std;

LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
unique_ptr<Module> TheModule;
map<std::string, AllocaInst *> NamedValues;

// for local variables, why I don't even :)
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          const std::string &name) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getInt32Ty(TheContext), 0,
                           name.c_str());
}

void global_var() {
  TheModule->getOrInsertGlobal("test", Type::getInt32Ty(TheContext));

  llvm::GlobalVariable* gVar = TheModule->getNamedGlobal("test");
  gVar->setInitializer( ConstantInt::get(TheContext, APInt(32, 1)) );
  gVar->setConstant(false);
  gVar->setLinkage(GlobalVariable::PrivateLinkage);


  Type * returnType = Type::getInt32Ty(TheContext);
  std::vector<Type*> argTypes;
  FunctionType *functionType = FunctionType::get(returnType, argTypes, false);
  Function * function = Function::Create(functionType, Function::ExternalLinkage, "main", TheModule.get());

  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", function);
  Builder.SetInsertPoint(BB);

  // a number
  //Value * var = ConstantInt::get(IntegerType::getInt32Ty(TheContext), 42, true);

  Value * new_return = Builder.getInt32(0);
  if(!TheModule->getNamedGlobal("test")->isConstant())
    Builder.CreateStore(new_return, TheModule->getNamedGlobal("test"));

  // loads value from ptr of some sort
  Value * val = Builder.CreateLoad(TheModule->getNamedGlobal("test"), "loaded");
  Builder.CreateRet(val);
}

int main(int argc, char* argv[])
{
  // base

  TheModule = llvm::make_unique<Module>("Testing Front-End", TheContext);

  // initialization of IR emmiting

  InitializeNativeTarget();
  InitializeNativeTargetAsmParser();
  InitializeNativeTargetAsmPrinter();

  // * * * * * * * * * * * *

  // printf
  vector<Type*> printf_arg_types;
  printf_arg_types.push_back(llvm::Type::getInt8PtrTy(TheContext));

  llvm::FunctionType* printf_type = llvm::FunctionType::get( llvm::Type::getInt32Ty(TheContext), printf_arg_types, true);

  Function *func = Function::Create(
      printf_type, Function::ExternalLinkage,
      "printf",
      TheModule.get()
  );

  // scanf
  vector<Type*> scanf_arg_types;
  scanf_arg_types.push_back(llvm::Type::getInt8PtrTy(TheContext));

  llvm::FunctionType* scanf_type = llvm::FunctionType::get( llvm::Type::getInt32Ty(TheContext), scanf_arg_types, true);

  Function *scanf = Function::Create(
      scanf_type, Function::ExternalLinkage,
      "scanf",
      TheModule.get()
  );

  Type * returnType = Type::getInt32Ty(TheContext);
  std::vector<Type*> argTypes;
  FunctionType *functionType = FunctionType::get(returnType, argTypes, false);
  Function * function = Function::Create(functionType, Function::ExternalLinkage, "main", TheModule.get());

  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", function);
  Builder.SetInsertPoint(BB);


  AllocaInst *Alloca = CreateEntryBlockAlloca(function, "local_var");
  Builder.CreateStore(ConstantInt::get(Builder.getInt32Ty(), 1), Alloca);

  vector<Value *> args;
  args.push_back(Builder.CreateGlobalString("%d\n", ".str"));
  args.push_back(Builder.CreateLoad(Alloca, "loaded"));

  Builder.CreateCall(func, args, "printf_call");



  Builder.CreateRet(ConstantInt::get(Builder.getInt32Ty(), 0));

  /*
  // scanf call
  AllocaInst *Alloca = CreateEntryBlockAlloca(function, "local_var");

  vector<Value *> args2;
  args2.push_back(Builder.CreateGlobalString("%d\n", ".str"));
  args2.push_back(Alloca);

  Builder.CreateCall(scanf, args2, "scanf_call");

  // printf call
  vector<Value *> args;
  args.push_back(Builder.CreateGlobalString("%d\n", ".str"));
  args.push_back(Builder.CreateLoad(Alloca, "loaded"));

  Builder.CreateCall(func, args, "printf_call");


  Builder.CreateRet(Builder.getInt32(0));

   */

  // * * * * * * * * * * * *

  auto TargetTriple = sys::getDefaultTargetTriple();


  std::string Error;
  auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

  // Print an error and exit if we couldn't find the requested target.
  // This generally occurs if we've forgotten to initialise the
  // TargetRegistry or we have a bogus target triple.
  if (!Target) {
    errs() << Error;
    return 1;
  }

  auto CPU = "generic";
  auto Features = "";

  TargetOptions opt;
  auto RM = Optional<Reloc::Model>();
  auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  // something, that is recomended by someone smart
  TheModule->setDataLayout(TargetMachine->createDataLayout());
  TheModule->setTargetTriple(TargetTriple);

  auto Filename = "output.o";
  std::error_code EC;
  raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

  if (EC) {
    errs() << "Could not open file: " << EC.message();
    return 1;
  }

  // Print out all of the generated code.
  TheModule->dump();

  legacy::PassManager pass;
  auto FileType = TargetMachine::CGFT_ObjectFile;

  if (TargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
    errs() << "TargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*TheModule);
  dest.flush();

  return 0;
}

