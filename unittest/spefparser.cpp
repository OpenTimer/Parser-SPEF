#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <spefparser.hpp>
#include <vector>
#include <utility>
#include <limits.h>
#include <cstdlib> 
#include <algorithm>
#include <cassert>

TEST_CASE("Header.Random"){
  std::srand(123); 

  std::vector<char> space = {' ', '\f', '\n', '\r', '\t', '\v'};
  std::vector<std::string> headers = {
    "*SPEF", "*DESIGN", "*DATE", "*VENDOR", "*PROGRAM", "*VERSION",
    "*DESIGN_FLOW", "*DIVIDER", "*DELIMITER", "*BUS_DELIMITER"
  };
  std::vector<std::string> units = {
    "*T_UNIT", "*C_UNIT", "*R_UNIT", "*L_UNIT"
  };
  std::unordered_map<std::string_view, std::string> kvp = {
    {"*SPEF", ""}, {"*DESIGN", ""}, {"*DATE", ""}, {"*VENDOR", ""},
    {"*PROGRAM", ""}, {"*VERSION", ""}, {"*DESIGN_FLOW", ""}, {"*DIVIDER", ""},
    {"*DELIMITER", ""}, {"*BUS_DELIMITER", ""}, {"*T_UNIT", ""}, {"*C_UNIT", ""}, 
    {"*R_UNIT", ""}, {"*L_UNIT", ""}
  };

  const int str_length {20};

  auto valid_char = [](){
    // 33 - 126 are valid chars except 34(") and 39 (') 42(*)
    int c = rand()%94+33;
    while(c == 34 or c == 39 or c == 42){
      c = rand()%94+33;
    }
    assert(not std::isspace(c) and c < 127 and c >= 33);
    return c;
  };

  auto rand_str = [&](size_t sz){ 
    std::string s(sz, ' ');
    std::generate(s.begin(), s.end(), valid_char);
    return s;
  };

  auto rand_space = [&](size_t sz){
    if(sz == 0){ return std::string(); }
    std::string s(sz, ' ');
    std::generate(s.begin(), s.end(), [&](){return space[rand()%space.size()];});
    return s;
  };

  auto rand_unit = [&](){
    return std::to_string((double)rand() / RAND_MAX) + rand_space(rand()%space.size()+1) + 
      rand_str(rand()%str_length+1);
  };
 

  spef::Spef data;
  std::string buffer;
  auto parse = [&data, &buffer](){ 
    data.clear();
    tao::pegtl::memory_input<> in(buffer, "");
    try{
      tao::pegtl::parse<spef::RuleSpef, spef::Action, spef::Control>(in, data);
    }
    catch(const tao::pegtl::parse_error& e){ 

      std::cout << e.what() << std::endl;                                                                                                
      const auto p = e.positions.front();
      std::cout << "Fail at line " << p.line << ":\n";
      std::cout << "  " << in.line_as_string(p) << '\n';
      std::cout << "  " << std::string(p.byte_in_line, ' ') << "\033[31m^\033[0m" << '\n';

      //{
      //  std::ofstream ofs("FAIL");
      //  ofs << buffer;
      //  ofs.close();
      //}
      //{
      //  std::ofstream ofs("FAIL_INT");
      //  for(size_t i=0; i<buffer.size(); i++){
      //    ofs << (int)(buffer[i]) << "=" << buffer[i] << '\n';
      //  }
      //  ofs.close();
      //}
      exit(1);
    }

  };


  for(int i=0; i<10000; i++){
    std::random_shuffle(headers.begin(), headers.end());
    std::random_shuffle(units.begin(), units.end());

    buffer.clear();  

    for(size_t i=0; i<headers.size(); ++i){
      if(headers[i] == "*DELIMITER" or headers[i] == "*DIVIDER"){
        kvp.at(headers[i]) = rand_str(1);
      }
      else if(headers[i] == "*BUS_DELIMITER"){
        kvp.at(headers[i]) = rand_str(1) + rand_space(rand()%space.size()) + rand_str(1);
      }
      else{
        kvp.at(headers[i]) = '"' + rand_str(rand()%str_length+1) + '"';
      }
      buffer.append(headers[i]).append(rand_space(rand()%space.size()+1))
        .append(kvp.at(headers[i])).append(rand_space(rand()%space.size()));
    }

    for(size_t i=0; i<units.size(); ++i){
      kvp.at(units[i]) = rand_unit();
      buffer.append(units[i]).append(rand_space(rand()%space.size()+1))
        .append(kvp.at(units[i])).append(rand_space(rand()%space.size()));
    }

    //std::cout << i << "  buffer = " << buffer << '\n';
    parse();

    REQUIRE(data.standard == kvp.at("*SPEF"));
    REQUIRE(data.design_name == kvp.at("*DESIGN"));
    REQUIRE(data.date == kvp.at("*DATE"));
    REQUIRE(data.vendor == kvp.at("*VENDOR"));
    REQUIRE(data.program == kvp.at("*PROGRAM"));
    REQUIRE(data.version == kvp.at("*VERSION"));
    REQUIRE(data.design_flow == kvp.at("*DESIGN_FLOW"));
    REQUIRE(data.divider == kvp.at("*DIVIDER"));
    REQUIRE(data.delimiter == kvp.at("*DELIMITER"));
    kvp.at("*BUS_DELIMITER").erase(
      remove_if(kvp.at("*BUS_DELIMITER").begin(), kvp.at("*BUS_DELIMITER").end(), 
        [](auto c){ return std::isspace(c); }), kvp.at("*BUS_DELIMITER").end()
    );
    REQUIRE(data.bus_delimiter == kvp.at("*BUS_DELIMITER"));

    REQUIRE(data.time_unit        == kvp.at("*T_UNIT"));
    REQUIRE(data.capacitance_unit == kvp.at("*C_UNIT"));
    REQUIRE(data.resistance_unit  == kvp.at("*R_UNIT"));
    REQUIRE(data.inductance_unit  == kvp.at("*L_UNIT"));
  } 
}
