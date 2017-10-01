# llvm pass - inject log

custom llvm pass to inject (runtime) logging when any function is entered or leaved

tested with llvm 5.0

**compile pass:**
```
cmake .
make
```

**compile example:**
```sh ./test.sh```

**run example:**
```./a.out```

This pass adds calling of function **print** to start of any function and just before ret instruction.
Function **print** must be linked together with target application - in example app it is implemented in pr.c
