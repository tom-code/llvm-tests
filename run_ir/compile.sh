echo $CXX
set
$CXX run_ir.cc -std=c++11 -lLLVM-5.0 -g
#$CXX run_ir.cc -std=c++14 `llvm-config-5.0 --cxxflags --ldflags --system-libs --libs all` -g

