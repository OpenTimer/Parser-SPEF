# Parser-SPEF

A Fast C++ Header-only Parser for Standard Parasitic Exchange Format (SPEF).

# Get Started with Parser-SPEF

A [Standard Parasitic Exchange Format] (SPEF) file records the parasitics of nets in a
circuit.

<img src="image/circuit.png" width="37%" align="right"> 

```text
// A RC network of 5 capacitors and 5 resistors
*D_NET Net1 total_cap  // total_cap = C1+C2+C3+C4+C5
*CONN          // Begin of CONN section
*I Z O         // Output Pin Z 
*I T1 I        // Input Pin T1
*I T2 I        // Input Pin T2
*CAP           // Begin of CAP section
1 P1 C1        // Node P1 with cap C1
2 P2 C2        // Node P2 with cap C2
3 P3 C3        // Node P3 with cap C3
4 T1 C4        // Node T1 with cap C4
5 T2 C5        // Node T2 with cap C5
*RES           // Begin of RES section
1 Z P3 R1      // Res R1 with nodes Z and P3
2 P2 P3 R2     // Res R2 with nodes P2 and P3
3 P1 P3 R3     // Res R3 with nodes P1 and P3
4 P1 T1 R4     // Res R4 with nodes P1 and T1
5 P2 T2 R5     // Res R5 with nodes P2 and T2
*END
```

The following example shows how to use Parser-SPEF to read a SPEF file:

```cpp
#include "Parser-SPEF/parser-spef.hpp"   // the only include you need

int main(){
  spef::Spef parser;                     // create a parser object
  if(parser.read("simple.spef")){        // parse a .spef
    std::cout << parser.dump() << '\n';  // dump the spef
  }
}
``` 
To compile this example, put the code in `example.cpp` and type the command:

```cpp
g++ -std=c++17 -I Parser-SPEF/ example.cpp -lstc++fs -o example
```

After the parse succeeds, you can retrieve the parsed data stored in the parser by accessing
the member data in `struct Spef`.

## Struct Spef 
The parser is a `struct Spef` that has following member data:

| Name | Type | Description |
| ------------- |:-------------| :--------------|
| standard               | `std::string` | the string after the header `*SPEF`            |
| design\_name           | `std::string` | the string after the header `*DESIGN`          |
| date                   | `std::string` | the string after the header `*DATE`            |
| vendor                 | `std::string` | the string after the header `*VENDOR`          |
| program                | `std::string` | the string after the header `*PROGRAM`         |
| version                | `std::string` | the string after the header `*VERSION`         |
| design\_flow           | `std::string` | the string after the header `*DESIGN_FLOW`     |
| divider                | `std::string` | the string after the header `*DIVIDER`         |
| delimiter              | `std::string` | the string after the header `*DELIMITER`       |
| bus\_delimiter         | `std::string` | the string after the header `*BUS_DELIMITER`   | 
| time\_unit             | `std::string` | the string after the header `*T_UNIT`          |
| capacitance\_unit      | `std::string` | the string after the header `*C_UNIT`          |
| resistance\_unit       | `std::string` | the string after the header `*R_UNIT`          |
| inductance\_unit       | `std::string` | the string after the header `*L_UNIT`          |
| name\_map              | `std::unordered_map<std::string, std::string>` |  the name mapping (a number with an asterisk prefix and the mapped name) in ***NAME\_MAP** section  |
| ports                  | `std::vector<Port>` | the set of ports in ***PORTS** section. `Port` is a `struct` |
| nets                   | `std::vector<Net> nets`| the set of nets in ***D\_NET** section. `Net` is a `struct`  |

The `struct Spef` has following member APIs:

| Name | Argument | Return | Description |
| ------ |:------------------| :-------------- | :-------------- |
| read  | path | bool | parse the file pointed by the path and return `true` if the parser succeeds |
| dump  | n/a  | `std::string` |  dump the member data to a `std::string` of SPEF format |
| clear | n/a  | n/a | clear the content of all member data |
| name\_expansion | n/a | n/a | expand the mapped names in all ports and nets |
| name\_expansion | `Net` | n/a | expand the mapped names in a given net |
| name\_expansion | `Port` | n/a | expand the mapped names in a given port |


## Struct Port
The `struct Port` stores the information regarding a port in ***PORTS** section. `struct Port` has following member
data:

| Name | Type | Description |
| ------------- |:-------------| :--------------|
| name   | `std::string` | the name of the port   |
| ConnectionDirection   | `enum class` | the direction of the port. The value could be either INPUT, OUTPUT or INOUT.   |


## Struct Net
The `struct Net` stores the information regarding a net in ***D\_NET** section. `struct Net` has following member
data:

| Name | Type | Description |
| ------------- |:-------------| :--------------|
| name   | `std::string` | the name of the net  |
| connections   | `std::vector<Connection>` | the connections in the ***CONN** section  |
| caps   | `std::vector<std::tuple<std::string, std::string, float>>` | the capacitances in the ***CAP** section  |
| ress   | `std::vector<std::tuple<std::string, std::string, float>>` | the resistances in the ***RES** section  |



# Compile Parser-SPEF

## System Requirements 
Parser-SPEF is very self-contained and only requires [PEGTL](https://github.com/taocpp/PEGTL) 
library. 
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

## Examples
The folder [example] contains a [simple.cpp] that parses a given SPEF file and dumps to screen. The folder [benchmark] contains SPEF files from [TAU 2015 contest] and can be supplied as inputs to the [simple.cpp].

| Example | Description | How to Run ? |
| ------------- |:-------------| :--------------|
| [simple.cpp] | Read a SPEF file and dump the parsed data to screen | ./simple [path to the SPEF file]  |


## Performance Profiling

| Benchmark | number of nets (**D\_NET**) | Real | User | Sys |
| --------- |:--------------: |:-----| :----| :--- |
 ac97\_ctrl.spef     | 14407	   |	0:00.16|	0.15|	0.01 
 aes\_core.spef      | 23199	   |	0:00.33|	0.32|	0.01 
 c1355.spef          | 221       | 0:00.00|	0.00|	0.00 
 c17\_slack.spef     | 11	      |	0:00.00|	0.00|	0.00 
 c17.spef            | 11	      | 0:00.00|	0.00|	0.00 
 c1908.spef          | 255	     | 0:00.00|	0.00|	0.00 
 c2670.spef          | 501	     | 0:00.00|	0.00|	0.00 
 c3540.spef          | 741	     | 0:00.01|	0.00|	0.00 
 c3\_slack.spef      | 7	       |	0:00.00|	0.00|	0.00 
 c432.spef           | 170	     | 0:00.00|	0.00|	0.00 
 c499.spef           | 217	     | 0:00.00|	0.00|	0.00 
 c5315.spef          | 1096	    | 0:00.01|	0.01|	0.00 
 c6288.spef          | 1699	    | 0:00.01|	0.01|	0.00 
 c7552\_slack.spef   | 1353	    |	0:00.01|	0.01|	0.00 
 c7552.spef          | 1353	    | 0:00.01|	0.01|	0.00 
 c880.spef           | 281	     | 0:00.00|	0.00|	0.00 
 des\_perf.spef      | 106532	  |	0:01.30|	1.23|	0.07 
 s1196.spef          | 657	     | 0:00.00|	0.00|	0.00 
 s1494.spef          | 814	     | 0:00.01|	0.00|	0.00 
 s27.spef            | 34	      | 0:00.00|	0.00|	0.00 
 s344.spef           | 193	     | 0:00.00|	0.00|	0.00 
 s349.spef           | 205	     | 0:00.00|	0.00|	0.00 
 s386.spef           | 186	     | 0:00.00|	0.00|	0.00 
 s400.spef           | 226	     | 0:00.00|	0.00|	0.00 
 s510.spef           | 312	     | 0:00.00|	0.00|	0.00 
 s526.spef           | 309	     | 0:00.00|	0.00|	0.00 
 simple.spef         | 6	       | 0:00.00|	0.00|	0.00 
 vga\_lcd.spef       | 139632	  |	0:01.56|	1.48|	0.08 






























	



























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
[example]:               https://github.com/OpenTimer/Parser-SPEF/tree/master/example
[benchmark]:             https://github.com/OpenTimer/Parser-SPEF/tree/master/benchmark  
[unittest]:              https://github.com/OpenTimer/Parser-SPEF/tree/master/unittest
[TAU 2015 contest]:      http://www.tauworkshop.com/2015/contest.html
[simple.cpp]:            https://github.com/OpenTimer/Parser-SPEF/tree/master/example/simple.cpp

