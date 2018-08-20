# Parser-SPEF
[![Build Status](https://travis-ci.org/OpenTimer/Parser-SPEF.svg?branch=master)](https://travis-ci.org/OpenTimer/Parser-SPEF)

A Fast C++ Header-only Parser for Standard Parasitic Exchange Format (SPEF).

# Get Started with Parser-SPEF

A [Standard Parasitic Exchange Format][SPEF] file records the parasitics of nets in a
circuit. For more details about the SPEF, please refer to the [SPEF format].

<img src="image/circuit.png" width="37%" align="right"> 

```text
// A RC network of 5 capacitors and 5 resistors
*D_NET Net1 total_cap  // total_cap = C1+C2+C3+C4+C5
*CONN                  // Begin of CONN section
*I Z O                 // Output Pin Z 
*I T1 I                // Input Pin T1
*I T2 I                // Input Pin T2
*CAP                   // Begin of CAP section
1 P1 C1                // Node P1 with cap C1
2 P2 C2                // Node P2 with cap C2
3 P3 C3                // Node P3 with cap C3
4 T1 C4                // Node T1 with cap C4
5 T2 C5                // Node T2 with cap C5
*RES                   // Begin of RES section
1 Z P3 R1              // Res R1 with nodes Z and P3
2 P2 P3 R2             // Res R2 with nodes P2 and P3
3 P1 P3 R3             // Res R3 with nodes P1 and P3
4 P1 T1 R4             // Res R4 with nodes P1 and T1
5 P2 T2 R5             // Res R5 with nodes P2 and T2
*END                   // End of NET section
```

The following [example.cpp](doc/example.cpp) shows how to read and parse a SPEF:

```cpp
#include <parser-spef.hpp>   // Parser-SPEF is header-only

int main(){
  spef::Spef parser;                     // create a parser object
  if(parser.read("myspef.spef")){        // parse a .spef
    std::cout << parser.dump() << '\n';  // dump the parsed spef
  }
  else {
    std::cerr << *parser.error;          // show the error message
  }
  return 0;
}
``` 

You only need a C++ compiler with C++17 support to compile Parser-SPEF.

```bash 
~$ cd doc
~$ g++ -std=c++17 -I ../ example.cpp -lstdc++fs -o example
```

# Use Parser-SPEF

Parser-SPEF is extremely easy to use and understand. Once the parser succeeds, you can retrieve
pretty much all required data in the structs `Spef`, `Port`, and `Net`, which are defined
in [parser-spef.hpp](./parser-spef.hpp).

## Struct Spef 

The parser object is a struct of type `Spef` which stores SPEF data 
when the parsing process succeeds.

| Name | Type | Description |
| ------------- |:-------------| :--------------|
| standard               | std::string | the string after the header `*SPEF`            |
| design_name           | std::string | the string after the header `*DESIGN`          |
| date                   | std::string | the string after the header `*DATE`            |
| vendor                 | std::string | the string after the header `*VENDOR`          |
| program                | std::string | the string after the header `*PROGRAM`         |
| version                | std::string | the string after the header `*VERSION`         |
| design_flow           | std::string | the string after the header `*DESIGN_FLOW`     |
| divider                | std::string | the string after the header `*DIVIDER`         |
| delimiter              | std::string | the string after the header `*DELIMITER`       |
| bus_delimiter         | std::string | the string after the header `*BUS_DELIMITER`   | 
| time_unit             | std::string | the string after the header `*T_UNIT`          |
| capacitance_unit      | std::string | the string after the header `*C_UNIT`          |
| resistance_unit       | std::string | the string after the header `*R_UNIT`          |
| inductance_unit       | std::string | the string after the header `*L_UNIT`          |
| name_map              | std::unordered_map<size_t, std::string> |  the name mapping (a number with an asterisk prefix and the mapped name) in *NAME_MAP* section  |
| ports                  | std::vector of Port | the ports in *PORTS* section. |
| nets                   | std::vector of Net  | the set of nets in *D_NET* section. |

The parser struct has the following member functions:

| Name | Argument | Return | Description |
| ----- |:------------------| :-------------- | :-------------- |
| read  | path | bool | parse a given file and return true upon success or false on failure |
| dump  | n/a  | std::string |  dump the content to a SPEF |
| clear | n/a  | n/a | clear the content of all member data |
| expand_name | n/a | n/a | expand the mapped names in all ports and nets |
| expand_name | Net | n/a | expand the mapped names in a given net |
| expand_name | Port | n/a | expand the mapped names in a given port |


## Struct Port

The struct `Port` stores the information of a port in **PORTS** section. 

| Name | Type | Description |
| ------------- |:-------------| :--------------|
| name   | std::string | the name of the port   |
| ConnectionDirection   | `enum class` | the direction of the port. The value could be either INPUT, OUTPUT or INOUT.   |


## Struct Net

The struct `Net` stores the information regarding a net in **D_NET** section. 

| Name | Type | Description |
| ------------- |:-------------| :--------------|
| name   | std::string | the name of the net  |
| connections   | std::vector<Connection> | the connections in the **CONN** section  |
| caps   | std::vector<std::tuple<std::string, std::string, float>> | the capacitances in the **CAP** section  |
| ress   | std::vector<std::tuple<std::string, std::string, float>> | the resistances in the **RES** section  |



# Compile Tests

Parser-SPEF is written in modern C++17 on top of a header-only Parsing Expression Grammar (PEG)
library [PEGTL][PEGTL]. 

## System Requirements 

To compile the unit tests, you only need a C++17 compiler:
+ GNU [C++ Compiler G++ v7.2](https://gcc.gnu.org/gcc-7/) (or higher) with C++17 support 
+ Clang 5.0 [C++ Compiler](https://clang.llvm.org/) (or higher) with C++17 support 

Currently Parser-SPEF has been tested to run well on Linux distributions and MAC OSX.

## Build through CMake

We use [CMake](https://cmake.org/) to manage the source and tests. 
We recommend using out-of-source build.

```bash
~$ git clone https://github.com/OpenTimer/Parser-SPEF.git
~$ cd Parser-SPEF
~$ mkdir build
~$ cd build
~$ cmake ../
~$ make 
```

After successful build, example and unittest binaries are placed in
the folder [example](./example) and [unittest](./unittest), respectively.

## Run Tests

Parser-SPEF uses [Doctest](https://github.com/onqtam/doctest) for unit tests. We have added all
unittests to the CMake and you can use the target `test` to run all tests.

```bash
~$ make test
```

## Examples

The folder [example](./example) contains several tutorial examples to demonstrate the usage of Parser-SPEF.

| Example | Description | How to Run ? |
| ------------- |:-------------| :--------------|
| [simple.cpp] | Read a SPEF file and dump the parsed data to screen | ./simple [file]  |


# Performance

Parser-SPEF is as fast as a handcrafted (highly optimized) SPEF parser but far more general and 
adaptive in new changes.
We have evaluated Parser-SPEF over a handcrafted implementation on large 
[benchmarks](https://sites.google.com/site/taucontest2015/resources)
on a 3.2 GHz 24 GB Ubuntu Machine using one thread.

| Benchmark | Number of Nets (D_NET) | File Size | Parser-SPEF | Handcrafted |
| --------- |:---------------------: | :-------: | :---------: | :---------: |
| netcard_iccad | 1498555 | 1.2G | 19.11 s | 21.48 |
| leon2_iccad | 1616984 | 1.4G | 21.51 s | 24.23 |
| leon3mp_iccad | 1247979 | 1.1G | 16.90 | 14.71 |
| vga_lcd | 139632 | 95M | 1.50 | 1.37 |


# License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

Parser-SPEF is licensed under the [MIT License](./LICENSE):

Copyright &copy; 2018 [Chun-Xun Lin][Chun-Xun Lin], [Tsung-Wei Huang][Tsung-Wei Huang] and [Martin Wong][Martin Wong]

The University of Illinois at Urbana-Champaign, IL, USA

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


* * *

[Tsung-Wei Huang]: http://web.engr.illinois.edu/~thuang19/
[Chun-Xun Lin]:    https://github.com/clin99
[Martin Wong]:     https://ece.illinois.edu/directory/profile/mdfwong
[PEG]:             https://en.wikipedia.org/wiki/Parsing_expression_grammar 
[PEGTL]:           https://github.com/taocpp/PEGTL
[SPEF]:            https://en.wikipedia.org/wiki/Standard_Parasitic_Exchange_Format 
[example]:         https://github.com/OpenTimer/Parser-SPEF/tree/master/example
[benchmark]:       https://github.com/OpenTimer/Parser-SPEF/tree/master/benchmark  
[unittest]:        https://github.com/OpenTimer/Parser-SPEF/tree/master/unittest
[TAU 2014]:        https://sites.google.com/site/taucontest2014/home
[TAU 2015]:        http://www.tauworkshop.com/2015/contest.html
[TAU 2016]:        https://sites.google.com/site/taucontest2016/home
[simple.cpp]:      https://github.com/OpenTimer/Parser-SPEF/tree/master/example/simple.cpp
[SPEF format]:     https://github.com/OpenTimer/OpenTimer/wiki/SPEF 

