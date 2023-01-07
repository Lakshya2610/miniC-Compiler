cd /home/guptalak/Desktop/code/build
make
echo "Compiler Compiled"
cd ..
/home/guptalak/Desktop/code/build/src/minicc /home/guptalak/Desktop/code/easy.c -o out.bc
echo "BitCode Generated"
llvm-dis out.bc -o out.ll
echo "Assembly Generated"
clang out.bc build/minicio/libminicio.a -o out
echo "Executable Generated"
