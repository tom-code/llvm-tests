
set -x
CXX=${CXX:=clang++}
LCFG=${LCFG:=llvm-config}

$LCFG --cxxflags
$CXX -c run_ir.cc -o run_ir.o `$LCFG --cxxflags` -std=c++11  -g
$CXX run_ir.o `$LCFG --ldflags --libs`

#$CXX run_ir.cc -std=c++14 `llvm-config-5.0 --cxxflags --ldflags --system-libs --libs all` -g

