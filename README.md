# Parser-SPEF
A Modern C++17 SPEF parser based on the [Parsing Expression Grammar](https://en.wikipedia.org/wiki/Parsing_expression_grammar). 


# Get Started with Parser-SPEF
The following example shows how to use Parser-SPEF to read a SPEF file:
```cpp
#include "Parser-SPEF/spefparser.hpp"   // the only include you need

int main(){

  spef::Spef parser;

  if(parser.read("path_to_the_spef_file")){

    // Dump the SPEF file after successful read
    std::cout << parser.dump() << '\n';

  }
}
```



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


# License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

OpenTimer is licensed under the [MIT License](./LICENSE):

Copyright &copy; 2018 [Dr. Tsung-Wei Huang][Tsung-Wei Huang] and [Dr. Martin Wong][Martin Wong]

The University of Illinois at Urbana-Champaign, IL, USA

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

