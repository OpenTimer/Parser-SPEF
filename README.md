# Parser-SPEF

A Fast C++ Header-only Parser for Standard Parasitic Exchange Format (SPEF).

# SPEF format

A [Standard Parasitic Exchange Format] (SPEF) file records the parasitics of nets in a
circuit. Below is an example:

<img src="image/circuit.png" width="30%" align="right" style="margin:50px 0px"> 

```text
*D_NET Net1 C1+C2+C3+C4+C5
*CONN 
*I Z O
*I T1 I
*I T2 I
*CAP
1 1 C1
2 2 C2
3 3 C3
4 T1 C4
5 T2 C5 
*RES 
1 Z 3 RA
2 3 2 RB
3 1 3 RC
4 1 T1 RD
5 2 T2 RE
*END
```

# Get Started with Parser-SPEF

The following example shows how to use Parser-SPEF to read a SPEF file:

```cpp
#include <Parser-SPEF/spefparser.hpp>   // the only include you need

int main(){

  spef::Spef parser;

  if(parser.read("path_to_the_spef_file")){

    // Dump the SPEF file after successful read
    std::cout << parser.dump() << '\n';

  }
}
```
The parser is a `struct Spef` storing the parsed data. The `struct Spef` has following public members:

```cpp
struct Spef {
  
  std::string standard;
  std::string design_name;
  std::string date;
  std::string vendor;
  std::string program;
  std::string version;
  std::string design_flow;
  std::string divider;
  std::string delimiter;
  std::string bus_delimiter;
  std::string time_unit;
  std::string capacitance_unit;
  std::string resistance_unit;
  std::string inductance_unit;

  std::unordered_map<std::string, std::string> name_map;
  std::vector<Port> ports;
  std::vector<Net> nets;

  std::string dump() const;
  void clear();

  template <typename T>
  friend struct Action;

  friend void split_on_space(const char*, const char*, std::vector<std::string_view>&);

  bool read(const std::experimental::filesystem::path &);

  // TODO: what is the terminology?
  void name_expansion();              // Expand everything
  void name_expansion(Net&);
  void name_expansion(Port&);
};
```

# Compile Parser-SPEF

## System Requirements 
Parser-SPEF is very self-contained and only requires [PEGTL](https://github.com/taocpp/PEGTL) library (included). 
To compile Parser-SPEF, you only need a C++17 compiler:
+ GNU [C++ Compiler G++ v7.2](https://gcc.gnu.org/gcc-7/) (or higher) with C++17 support 
+ Clang 5.0 [C++ Compiler](https://clang.llvm.org/) (or higher) with C++17 support 

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

Parser-SPEF is licensed under the [MIT License](./LICENSE):

Copyright &copy; 2018 [Chun-Xun Lin][Chun-Xun Lin], [Tsung-Wei Huang][Tsung-Wei Huang] and [Martin Wong][Martin Wong]

The University of Illinois at Urbana-Champaign, IL, USA

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


* * *

[Tsung-Wei Huang]:       http://web.engr.illinois.edu/~thuang19/
[Chun-Xun Lin]:          https://github.com/clin99
[Martin Wong]:           https://ece.illinois.edu/directory/profile/mdfwong
[PEG]:                   https://en.wikipedia.org/wiki/Parsing_expression_grammar 
[Standard Parasitic Exchange Format]: https://en.wikipedia.org/wiki/Standard_Parasitic_Exchange_Format
