# Simple LLVM project

Simple program, created using hammer, duck tape and magic of Gandalf the grey. 

It initialize LLVM 'structures', create prototypes for printf and scanf and
call them.

On std::out it prints LLVM IR and creates file output.o with object code.

## But how can I use any of this?

You'll need:
* a dwarf, three hobbits and one elvish guy
* cmake (and probably make)
* compiler of some sort
* installed llvm libraries (I used version 4.0)
* luck

What now?

How about **compile it** first?   

    cmake CMakeLists.txt  
    make Frodo

Maybe **run** it?

    ./Frodo

How about **linking** the result?  

    { clang++ | g++ | gcc | ... } output.o -o run  
    ./run  

Was it that difficult? If it was, feel free to try GCC (no hate). 
