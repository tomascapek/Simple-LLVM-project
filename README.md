# Sample LLVM project

Simple program, created using hammer and duck tape.

It initialize LLVM 'structures', create prototypes for printf and scanf and
call them.

On std::out it prints LLVM IR and creates file output.o with object code.

## But how can I use any of this?

How about compile it first?   

   cmake CMakeLists.txt  
   make Mila

And run it?

    ./Mila

And link result?  

   { clang++ | g++ | gcc | ... } output.o -o run
   ./run

Was it too difficult? Go and try GCC :) 