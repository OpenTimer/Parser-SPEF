#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <spefparser.hpp>
#include <vector>
#include <utility>
#include <limits.h>
#include <cstdlib> 
#include <algorithm>

TEST_CASE("Header"){
  std::srand(0); 

  std::vector<char> space = {' ', '\f', '\n', '\r', '\t', '\v'};
  std::vector<std::string> headers = {
    "*SPEF", "*DESIGN", "*DATE", "*VENDOR", "*PROGRAM", "*VERSION",
    "*DESIGN_FLOW", "*DIVIDER", "*DELIMITER", "*BUS_DELIMITER"
  };
  std::vector<std::string> units = {
    "T_UNIT", "C_UNIT", "R_UNIT", "L_UNIT"
  };


  auto valid_char = [](){ 
    // 33 - 126 are valid chars except 34(") and 39 (')
    int c {rand()%94+32};
    while(c == 34 or c == 39){
      c = rand()%94+32;
    }
    return c;
  };
 

  spef::Spef data;
  std::string buffer;
  auto parse = [&data, &buffer](){
    tao::pegtl::memory_input<> in(buffer, "");
    tao::pegtl::parse<spef::RuleSpef, spef::Action, spef::Control>(in, data);
  };
  {
    std::random_shuffle(headers.begin(), headers.end());
    std::random_shuffle(units.begin(), units.end());

    int header_num {rand()%headers.size()};
    int unit_num   {rand()%units.size()};

    buffer.clear();  

    std::unordered_map<std::string_view, std::string> kvp;
    for(size_t i=0; i<header_num; ++i){
  
    }
     
    //REQUIRE(data.standard == "");
    //REQUIRE(data.standard == "\"IEEE 1481-1998\"");
  }
  
}
