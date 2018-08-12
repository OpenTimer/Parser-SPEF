# Parser-SPEF
A Modern C++17 SPEF parser 


# Compile Parser-SPEF

## System Requirements 
Parser-SPEF is very self-contained and only requires [PEGTL](https://github.com/taocpp/PEGTL) library (included). 
To compile Parser-SPEF, you only need:
+ A GNU [C++ Compiler G++ v7.2](https://gcc.gnu.org/gcc-7/) (or higher) with C++17 support 
Currently Parser-SPEF has been tested to run well on Linux distributions. 

## Compile Whole Project
We use [CMake](https://cmake.org/) to manage the source and tests. 
We recommend using out-of-source build.

```bash
~$ git clone https://github.com/OpenTimer/Parser-SPEF.git
~$ cd Parser-SPEF
~$ mkdir build
~$ cd build
~$ cmake ../ -DCMAKE_CXX_COMPILER=g++
~$ make 
```

After successful build, you can find the binaries for both examples and unit tests in the 
folder `example` and `unittest`, respectively.

## Run Tests
Parser-SPEF uses [Doctest](https://github.com/onqtam/doctest) for unit tests. After successful build, 
to run the unit tests:
```bash
~$ make test
```

# Integrate Parser-SPEF to your Project 

