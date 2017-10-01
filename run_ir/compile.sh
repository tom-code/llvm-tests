#export CXX=clang++

set -x
CXX=${CXX:=clang++}
LCFG=${LCFG:=llvm-config}

$LCFG --cxxflags
$CXX run_ir.cc `$LCFG --cxxflags` -std=c++11  -g `$LCFG --libs`

#$CXX run_ir.cc -std=c++14 `llvm-config-5.0 --cxxflags --ldflags --system-libs --libs all` -g

