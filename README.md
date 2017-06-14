# Simple LLVM project

Simple program, created using hammer and duck tape. 

It initialize LLVM 'structures', create prototypes for printf and scanf and
call them.

On std::out it prints LLVM IR and creates file output.o with object code.

## But how can I use any of this?

You'll need:
* cmake (and build tool of your choice)
* compiler of some sort
* installed llvm dev libraries (I used version 4.0)
What now?

**compile it** first?   

    cmake CMakeLists.txt  
    make Frodo

**run** it?

    ./Frodo

and **link** the result

    { clang++ | g++ | gcc | ... } output.o -o run  
    ./run
    
Have fun! 
