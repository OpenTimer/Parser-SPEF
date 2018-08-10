#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <spefparser.hpp>
#include <vector>
#include <utility>
#include <limits.h>
#include <cstdlib> 
#include <algorithm>
#include <cassert>
#include <random>


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
  // Remove comment symbol "//"
  for(size_t i=0; i<s.size()-1; ++i){
    if(s[i] == '/' and s[i+1] == '/'){
      s[i+1] = '\\';
    }
  }
  return s;
}


//const std::vector<char> SPACE = {' ', '\f', '\n', '\r', '\t', '\v'};
const std::vector<char> SPACE = {' ', '\n'};

std::string space(){
  std::string s(rand()%SPACE.size()+1, ' ');
  std::generate(s.begin(), s.end(), [&](){return SPACE[rand()%SPACE.size()];});
  return s;
}

std::string dontcare(){
  size_t sz {rand()%SPACE.size()};
  std::string s(sz, ' ');
  std::generate(s.begin(), s.end(), [&](){return SPACE[rand()%SPACE.size()];});
  return s;
}

float rand_float(){
  return (float)rand() / RAND_MAX;
}

std::string to_string(float f){
  std::stringstream ss;
  ss << std::fixed << std::setprecision(6) << f;
  return ss.str();
}

// End of Utility ---------------------------------------------------------------------------------


std::string rand_port(spef::ConnectionDirection c, size_t len){
  std::string s {"*"};
  s.append(rand_str(rand()%len+1)).append(space());
  switch(c){
    case spef::ConnectionDirection::INPUT:  s.append("I"); break;
    case spef::ConnectionDirection::OUTPUT: s.append("O"); break;
    case spef::ConnectionDirection::INOUT:  s.append("B"); break;
    default: break;
  }
  s.append(space());

  if(rand()%2 == 0){
    return s;
  }
  
  if(auto num=rand()%3; num == 0 or num == 1){
    if(num == 0) s.append("*C");
    else         s.append("*S");
    for(size_t i=0; i<2; i++){
      s.append(space());
      s.append(to_string(rand_float()));
    }
  }
  else{
    s.append("*L").append(space())
     .append(to_string(rand_float()));
  }
  s.append(dontcare());
  return s;
}


std::pair<std::string, std::vector<spef::Net>> rand_net(size_t len){

  std::vector<spef::Net> nets;
  //const int net_num {rand()%3+1};
  const int net_num {1000000};
  for(int i=0; i<net_num; i++){
    nets.push_back({rand_str(len), rand_float()});

    // CONN section
    if(rand()%2){
      nets.back().connections.resize(rand()%10+1);
      for(auto&c : nets.back().connections){
        c.name = rand_str(len);
        c.type = rand()%2 ? spef::ConnectionType::INTERNAL : spef::ConnectionType::EXTERNAL;
        c.direction = rand()%2 ? spef::ConnectionDirection::INOUT :
                 rand()%2 ? spef::ConnectionDirection::INPUT :  spef::ConnectionDirection::OUTPUT;
        // Add coordinate
        if(rand()%2){
          c.coordinate = std::make_pair(rand_float(), rand_float());
        }
        // Add load
        if(rand()%2){
          c.load = rand_float();
        }
        // Add driving cell 
        if(rand()%2){
          c.driving_cell = rand_str(len);
        }
      }
    }

    // CAP section 
    if(rand()%2){
      const int cap_num {rand()%10+1};
      for(int j=0; j<cap_num; ++j){
        if(rand()%2){
          // Ground
          nets.back().caps.push_back({rand_str(len), "", rand_float()});
        }
        else{
          // Coupling
          nets.back().caps.push_back({rand_str(len), rand_str(len), rand_float()});
        }
      }
    }

    // RES section 
    if(rand()%2){
      const int res_num {rand()%10+1};
      for(int j=0; j<res_num; ++j){
        nets.back().ress.push_back({rand_str(len), rand_str(len), rand_float()});
      }
    }
  } // End of adding nets

  std::string s;
  for(const auto& n: nets){
  //for(size_t i=13 ; i<nets.size() and i < 14; i++){
  //  const auto &n {nets[i]};
    s.append("*D_NET").append(space()).append(n.name).append(space())
     .append(to_string(n.lcap)).append(dontcare());
    // CONN 
    if(not n.connections.empty()){
      s.append("*CONN").append(dontcare());
      for(const auto& c: n.connections){
        switch(c.type){
          case spef::ConnectionType::INTERNAL: s.append("*I"); break;
          case spef::ConnectionType::EXTERNAL: s.append("*P"); break;
          default:break;
        }
        s.append(space()).append(c.name).append(space());
        switch(c.direction){
          case spef::ConnectionDirection::INPUT:  s.append("I"); break;
          case spef::ConnectionDirection::INOUT:  s.append("B"); break;
          case spef::ConnectionDirection::OUTPUT: s.append("O"); break;
          default:break;
        }
        // Add coordinate 
        if(c.coordinate.has_value()){
          s.append(space()).append("*C")
           .append(space()).append(to_string(std::get<0>(*c.coordinate)))
           .append(space()).append(to_string(std::get<1>(*c.coordinate)));
        }
        // Add load 
        if(c.load.has_value()){
          s.append(space()).append("*L")
           .append(space()).append(to_string(*c.load));
        }
        // Add driving cell
        if(not c.driving_cell.empty()){
          s.append(space()).append("*D")
           .append(space()).append(c.driving_cell);
        }
        s.append(dontcare());
      }
    }

    // CAPS
    if(not n.caps.empty()){
      s.append("*CAP").append(dontcare());
      for(size_t j=0; j<n.caps.size(); j++){
        s.append(std::to_string(j+1)).append(space()).append(std::get<0>(n.caps[j]))
         .append(space()).append(std::get<1>(n.caps[j])).append(space())
         .append(to_string(std::get<2>(n.caps[j]))).append(space());
      }
    }

    // RESS
    if(not n.ress.empty()){
      s.append("*RES").append(dontcare());
      for(size_t j=0; j<n.ress.size(); j++){
        s.append(std::to_string(j+1)).append(space()).append(std::get<0>(n.ress[j]))
         .append(space()).append(std::get<1>(n.ress[j])).append(space())
         .append(to_string(std::get<2>(n.ress[j]))).append(space());
      }
    }

    s.append("*END").append(dontcare());
  }  // End of adding nets to buffer
  return std::make_pair(s, nets);
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





// Testcases --------------------------------------------------------------------------------------
TEST_CASE("Header.Fix"){
  std::srand(123); 

  std::unordered_map<std::string, std::string> 
  headers = {
    {"*SPEF",          "\"1481-2009 IEEE Standar\""},
    {"*DESIGN",        "\"Design1\""},
    {"*DATE",          "\"Mon Aug 9 03:10 2018\""},
    {"*VENDOR",        "\"OpenTimer\""},
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
      buf.append(space());
    }
    else if(num == 1){
      buf.insert(0, space());
    }
  };
  auto add_data = [&](std::string& buf, const std::unordered_map<std::string, std::string>& values){
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
          buf.append(k).append(space()).append(kvp.at(k));
        }
        else{
          // Add to front
          buf.insert(0, kvp.at(k)).insert(0, space()).insert(0, k);
        }
        add_space(buf);
      }
  };

  const size_t run {10000};
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

TEST_CASE("Header.Random"){ 
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
    return to_string(rand_float()) + space() + 
      rand_str(rand()%str_length+1);
  };
 

  spef::Spef data;
  std::string buffer;
  std::default_random_engine gen(0);

  for(int i=0; i<10000; i++){
    std::shuffle(headers.begin(), headers.end(), gen);
    std::shuffle(units.begin(), units.end(), gen);

    buffer.clear();  

    for(size_t i=0; i<headers.size(); ++i){
      if(headers[i] == "*DELIMITER" or headers[i] == "*DIVIDER"){
        kvp.at(headers[i]) = rand_str(1);
      }
      else if(headers[i] == "*BUS_DELIMITER"){
        kvp.at(headers[i]) = rand_str(1) + dontcare() + rand_str(1);
      }
      else{
        kvp.at(headers[i]) = '"' + rand_str(rand()%str_length+1) + '"';
      }
      buffer.append(headers[i]).append(space())
        .append(kvp.at(headers[i])).append(dontcare());
    }

    for(size_t i=0; i<units.size(); ++i){
      kvp.at(units[i]) = rand_unit();
      buffer.append(units[i]).append(space())
        .append(kvp.at(units[i])).append(dontcare());
    }

    //std::cout << i << "  buffer = " << buffer << '\n';
    parse(data, buffer);

    compare_header(data, kvp);
  } 
}


TEST_CASE("NameMap"){

  srand(123);
  spef::Spef data;
  std::string buffer("*NAME_MAP");

  // Empty NAME MAP section
  parse(data, buffer); 
  REQUIRE(data.name_map.empty());

  const size_t len {30};
  const size_t name_map_num {100};
  const size_t run {10000};
  std::unordered_map<std::string, std::string> name_map;

  for(size_t j=0; j<run; j++){
    name_map.clear();

    buffer = "*NAME_MAP";
    buffer.append(dontcare());

    for(size_t i=0; i<name_map_num; ++i){
      name_map.insert({rand_str(rand()%len + 1).insert(0, 1, '*'), rand_str(rand()%len+1)});
    }

    for(const auto& [k, v]: name_map){
      buffer.append(k).append(space())
        .append(v).append(dontcare());
    }

    parse(data, buffer);
    REQUIRE(data.name_map == name_map);
  }
}



TEST_CASE("Port"){
  srand(123);
  spef::Spef data;
  std::string buffer("*PORTS");

  // Empty Port section
  parse(data, buffer); 
  REQUIRE(data.ports.empty());

  std::vector<std::string> port_names;
  std::vector<spef::ConnectionDirection> port_directions;
  const size_t len {30};
  const size_t port_num {100};
  const size_t run {10000};
  for(size_t j=0; j<run; j++){
    port_names.clear();
    port_directions.clear();

    buffer = "*PORTS";
    buffer.append(dontcare());
    for(size_t i=0; i<port_num; ++i){
      size_t beg = buffer.size();
      switch(rand()%3){
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
      buffer.append(dontcare());
    }

    parse(data, buffer);

    REQUIRE(data.ports.size() == port_num);
    for(size_t k=0; k<data.ports.size(); ++k){
      REQUIRE(data.ports[k].direction == port_directions[k]);
      REQUIRE(data.ports[k].name == port_names[k]);
    }
  }
}



TEST_CASE("Net"){
  srand(123);
  spef::Spef data;
  auto [buffer, nets] = rand_net(20);

  //std::cout << "=" << buffer << "=\n";
  parse(data, buffer);

  REQUIRE(data.nets.size() == nets.size());
  REQUIRE(data.nets == nets);
}

