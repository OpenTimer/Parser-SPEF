#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <spefparser.hpp>
#include <vector>
#include <utility>
#include <limits.h>
#include <cstdlib> 
#include <algorithm>
#include <cassert>


// Utility ----------------------------------------------------------------------------------------
char valid_char(){
  // 33 - 126 are valid chars except 34(") and 39 (') 42(*)
  int c = rand()%94+33;
  while(c == 34 or c == 39 or c == 42){
    c = rand()%94+33;
  }
  assert(not std::isspace(c) and c < 127 and c >= 33);
  return c;
}

std::string rand_str(size_t sz){ 
  if(sz == 0) return std::string();
  std::string s(sz, ' ');
  std::generate(s.begin(), s.end(), valid_char);
  return s;
}


const std::vector<char> space = {' ', '\f', '\n', '\r', '\t', '\v'};
std::string rand_space(size_t sz){
  if(sz == 0){ return std::string(); }
  std::string s(sz, ' ');
  std::generate(s.begin(), s.end(), [&](){return space[rand()%space.size()];});
  return s;
}

std::string rand_port(spef::ConnectionDirection c, size_t len){
  std::string s {"*"};
  s.append(rand_str(rand()%len+1)).append(rand_space(rand()%space.size()+1));
  switch(c){
    case spef::ConnectionDirection::INPUT:  s.append("I"); break;
    case spef::ConnectionDirection::OUTPUT: s.append("O"); break;
    case spef::ConnectionDirection::INOUT:  s.append("B"); break;
    default: break;
  }
  s.append(rand_space(rand()%space.size()+1));

  if(rand()%2 == 0){
    return s;
  }
  
  if(auto num=rand()%3; num == 0 or num == 1){
    if(num == 0) s.append("*C");
    else         s.append("*S");
    for(size_t i=0; i<2; i++){
      s.append(rand_space(rand()%space.size()+1));
      s.append(std::to_string((double)rand() / RAND_MAX));
    }
  }
  else{
    s.append("*L").append(rand_space(rand()%space.size()+1))
     .append(std::to_string((double)rand() / RAND_MAX));
  }
  s.append(rand_space(rand()%space.size()));
  return s;
}



void parse(spef::Spef &data, std::string& buffer){
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
    exit(1);
  }
}

void compare_header(spef::Spef& data, std::unordered_map<std::string, std::string> gold){
  REQUIRE(data.standard == gold.at("*SPEF"));
  REQUIRE(data.design_name == gold.at("*DESIGN"));
  REQUIRE(data.date == gold.at("*DATE"));
  REQUIRE(data.vendor == gold.at("*VENDOR"));
  REQUIRE(data.program == gold.at("*PROGRAM"));
  REQUIRE(data.version == gold.at("*VERSION"));
  REQUIRE(data.design_flow == gold.at("*DESIGN_FLOW"));
  REQUIRE(data.divider == gold.at("*DIVIDER"));
  REQUIRE(data.delimiter == gold.at("*DELIMITER"));
  gold.at("*BUS_DELIMITER").erase(
    remove_if(gold.at("*BUS_DELIMITER").begin(), gold.at("*BUS_DELIMITER").end(), 
      [](auto c){ return std::isspace(c); }), gold.at("*BUS_DELIMITER").end()
  );
  REQUIRE(data.bus_delimiter == gold.at("*BUS_DELIMITER"));

  REQUIRE(data.time_unit        == gold.at("*T_UNIT"));
  REQUIRE(data.capacitance_unit == gold.at("*C_UNIT"));
  REQUIRE(data.resistance_unit  == gold.at("*R_UNIT"));
  REQUIRE(data.inductance_unit  == gold.at("*L_UNIT"));
} 
// End of Utility --------------------------------------------------------------------------------- 




TEST_CASE("Header.Fix"){ return ;
  std::srand(123); 

  std::unordered_map<std::string, std::string> 
  headers = {
    {"*SPEF",          "\"1481-2009 IEEE Standar\""},
    {"*DESIGN",        "\"Design1\""},
    {"*DATE",          "\"Mon Aug 9 03:10 2018\""},
    {"*VENDOR",        "\"An EDA company\""},
    {"*PROGRAM",       "\"SIMPLE\""},
    {"*VERSION",       "\"1.2.3\""},
    {"*DIVIDER",       "/"},
    {"*DELIMITER",     ":"},
    {"*BUS_DELIMITER", "[  ]"}
  };
  
  std::unordered_map<std::string, std::string> 
  units = {
    {"*T_UNIT",    "1 S"},
    {"*R_UNIT",    "1 MOHM"},
    {"*L_UNIT",    "1 FHENRY"},
    {"*C_UNIT",    "1 UC"}
  };

  std::unordered_map<std::string, std::string> 
  kvp = {
    {"*SPEF", ""}, {"*DESIGN", ""}, {"*DATE", ""}, {"*VENDOR", ""},
    {"*PROGRAM", ""}, {"*VERSION", ""}, {"*DESIGN_FLOW", ""}, {"*DIVIDER", ""},
    {"*DELIMITER", ""}, {"*BUS_DELIMITER", ""}, {"*T_UNIT", ""}, {"*C_UNIT", ""}, 
    {"*R_UNIT", ""}, {"*L_UNIT", ""}
  };

  spef::Spef data;
  std::string buffer;
  std::string header_buf;
  std::string unit_buf;

  auto add_space = [&](std::string& buf){
    if(auto num = rand()%3; num == 0){
      buf.append(1, space[rand()%space.size()]);
    }
    else if(num == 1){
      buf.insert(0, 1, space[rand()%space.size()]);
    }
  };
  auto add_data = [&](std::string& buf, 
    const std::unordered_map<std::string, std::string>& values){
      for(const auto& [k, v]: values){
        // Empty or Fix value 
        if(k.find("UNIT") != std::string::npos or 
           k.find("DELIMITER") != std::string::npos or
           k.find("DIVIDER") != std::string::npos){
          kvp.at(k) = values.at(k);
        }
        else{
          kvp.at(k) = rand()%2 == 0 ? "" : values.at(k);
        }
        if(rand()%2 == 0){ 
          // Add to back
          buf.append(k).append(1, space[rand()%space.size()]).append(kvp.at(k));
        }
        else{
          // Add to front
          buf.insert(0, kvp.at(k)).insert(0, 1, space[rand()%space.size()]).insert(0, k);
        }
        add_space(buf);
      }
  };

  size_t run {10000};
  for(size_t i=0; i<run; ++i){
    buffer.clear();
    header_buf.clear();
    unit_buf.clear();

    add_data(header_buf, headers);
    add_data(unit_buf, units);
    buffer.append(header_buf).append(unit_buf);

    parse(data, buffer);
    compare_header(data, kvp); 
  }
}

TEST_CASE("Header.Random"){ return;
  std::srand(123); 

  std::vector<std::string> 
  headers = {
    "*SPEF", "*DESIGN", "*DATE", "*VENDOR", "*PROGRAM", "*VERSION",
    "*DESIGN_FLOW", "*DIVIDER", "*DELIMITER", "*BUS_DELIMITER"
  };
  std::vector<std::string> 
  units = {
    "*T_UNIT", "*C_UNIT", "*R_UNIT", "*L_UNIT"
  };
  std::unordered_map<std::string, std::string> 
  kvp = {
    {"*SPEF", ""}, {"*DESIGN", ""}, {"*DATE", ""}, {"*VENDOR", ""},
    {"*PROGRAM", ""}, {"*VERSION", ""}, {"*DESIGN_FLOW", ""}, {"*DIVIDER", ""},
    {"*DELIMITER", ""}, {"*BUS_DELIMITER", ""}, {"*T_UNIT", ""}, {"*C_UNIT", ""}, 
    {"*R_UNIT", ""}, {"*L_UNIT", ""}
  };

  const int str_length {20};



  auto rand_unit = [&](){
    return std::to_string((double)rand() / RAND_MAX) + rand_space(rand()%space.size()+1) + 
      rand_str(rand()%str_length+1);
  };
 

  spef::Spef data;
  std::string buffer;

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
    parse(data, buffer);

    compare_header(data, kvp);
  } 
}


TEST_CASE("NameMap"){ return ;

  srand(123);
  spef::Spef data;
  std::string buffer("*NAME_MAP");

  // Empty NAME MAP section
  parse(data, buffer); 
  REQUIRE(data.name_map.empty());

  size_t len {30};
  size_t name_map_num {100};
  std::unordered_map<std::string, std::string> name_map;

  size_t run {10000};
  for(size_t j=0; j<run; j++){
    name_map.clear();

    buffer = "*NAME_MAP";
    buffer.append(rand_space(rand()%space.size()));

    for(size_t i=0; i<name_map_num; ++i){
      name_map.insert({rand_str(rand()%len + 1).insert(0, 1, '*'), rand_str(rand()%len+1)});
    }

    for(const auto& [k, v]: name_map){
      buffer.append(k).append(rand_space(rand()%space.size()+1))
        .append(v).append(rand_space(rand()%space.size()));
    }

    parse(data, buffer);
    REQUIRE(data.name_map == name_map);
  }
}

TEST_CASE("Port"){
  srand(123);
  spef::Spef data;
  std::string buffer("*PORTS");
  // Empty NAME MAP section
  parse(data, buffer); 
  REQUIRE(data.ports.empty());

  std::vector<std::string> port_names;
  std::vector<spef::ConnectionDirection> port_directions;
  size_t len {30};
  size_t port_num {100};
  size_t run {10000};
  for(size_t j=0; j<run; j++){
    port_names.clear();
    port_directions.clear();

    buffer = "*PORTS";
    buffer.append(rand_space(rand()%space.size()));
    for(size_t i=0; i<port_num; ++i){
      size_t beg = buffer.size();
      auto d = rand()%3;
      switch(d){
        case 0: buffer.append(rand_port(spef::ConnectionDirection::INPUT,  len)); 
          port_directions.emplace_back(spef::ConnectionDirection::INPUT);
          break;
        case 1: buffer.append(rand_port(spef::ConnectionDirection::OUTPUT, len)); 
          port_directions.emplace_back(spef::ConnectionDirection::OUTPUT);
          break;
        case 2: buffer.append(rand_port(spef::ConnectionDirection::INOUT,  len)); 
          port_directions.emplace_back(spef::ConnectionDirection::INOUT);
          break;
        default: break;
      }
      size_t end {beg};
      while(not std::isspace(buffer[end])){
        end ++;
      }
      port_names.emplace_back(buffer.substr(beg, end-beg));
      buffer.append(rand_space(rand()%space.size()));
    }

    //std::cout << "Buffer=" << buffer << "=\n";

    parse(data, buffer);
    REQUIRE(data.ports.size() == port_num);
    for(size_t k=0; k<data.ports.size(); ++k){
      REQUIRE(data.ports[k].direction == port_directions[k]);
      REQUIRE(data.ports[k].name == port_names[k]);
    }
  }
}


