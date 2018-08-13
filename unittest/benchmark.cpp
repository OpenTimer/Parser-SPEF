#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <parser-spef.hpp>

TEST_CASE("Benchmark"){

  std::vector<std::string> benchmarks{
    "../benchmark/ac97_ctrl.spef",
    "../benchmark/aes_core.spef",
    "../benchmark/c1355.spef",
    "../benchmark/c17_slack.spef",
    "../benchmark/c17.spef",
    "../benchmark/c1908.spef",
    "../benchmark/c2670.spef",
    "../benchmark/c3540.spef",
    "../benchmark/c3_slack.spef",
    "../benchmark/c432.spef",
    "../benchmark/c499.spef",
    "../benchmark/c5315.spef",
    "../benchmark/c6288.spef",
    "../benchmark/c7552_slack.spef",
    "../benchmark/c7552.spef",
    "../benchmark/c880.spef",
    "../benchmark/des_perf.spef",
    "../benchmark/s1196.spef",
    "../benchmark/s1494.spef",
    "../benchmark/s27.spef",
    "../benchmark/s344.spef",
    "../benchmark/s349.spef",
    "../benchmark/s386.spef",
    "../benchmark/s400.spef",
    "../benchmark/s510.spef",
    "../benchmark/s526.spef",
    "../benchmark/simple.spef",
    "../benchmark/vga_lcd.spef"
  };


  for(const auto& b: benchmarks){
    std::cout << b << '\n';
    const std::string path(b);
    spef::Spef parser;
    REQUIRE(parser.read(path));

    auto buffer {spef::file_to_memory(path)};
    std::vector<std::string_view> tokens;
    spef::split_on_space(buffer.data(), buffer.data()+buffer.size(), tokens);

    std::optional<size_t> name_map_beg;
    std::optional<size_t> port_beg; 

    std::vector<size_t> net_beg;
    std::vector<size_t> conn_beg;
    std::vector<size_t> cap_beg;
    std::vector<size_t> res_beg;
    std::vector<size_t> net_end;

    for(size_t i=0; i<tokens.size(); ++i){
      if(tokens[i] == "*NAME_MAP"){
        name_map_beg = i;
      }
      else if(tokens[i] == "*PORTS"){
        port_beg = i;
      }
      else if(tokens[i] == "*D_NET"){
        net_beg.push_back(i);
      }
      else if(tokens[i] == "*CONN"){
        conn_beg.push_back(i);
      }
      else if(tokens[i] == "*CAP"){
        cap_beg.push_back(i);
      }
      else if(tokens[i] == "*RES"){
        res_beg.push_back(i);
      }
      else if(tokens[i] == "*END"){
        if(conn_beg.size() < net_beg.size()){
          conn_beg.push_back(net_beg.back());
        }
        if(cap_beg.size() < net_beg.size()){
          cap_beg.push_back(net_beg.back());
        }
        if(res_beg.size() < net_beg.size()){
          res_beg.push_back(i);
        }
        net_end.push_back(i);
      }
    }

    REQUIRE(conn_beg.size() == net_beg.size());
    REQUIRE(cap_beg.size()  == net_beg.size());
    REQUIRE(res_beg.size()  == net_beg.size());
    REQUIRE(net_end.size()  == net_beg.size());

    // Check name map section
    if(name_map_beg.has_value()){
      REQUIRE(parser.name_map.size() > 0);
      size_t name_map_num {0};
      for(size_t i = *name_map_beg+1;; i+=2){
        if(tokens[i][0] !='*' or 
            tokens[i].find_first_not_of("0123456789", 1) != std::string_view::npos){
          break;
        }
        ++ name_map_num;
      }
      REQUIRE(name_map_num == parser.name_map.size());
    }
    else{
      REQUIRE(parser.name_map.size() == 0);
    }

    // Check port section
    if(port_beg.has_value()){
      REQUIRE(parser.ports.size() > 0);
      size_t port_num {0};
      size_t input_num {0};
      size_t output_num {0};
      size_t inout_num {0};

      for(size_t i=*port_beg+1; i<tokens.size(); ++i){
        if(tokens[i] == "*D_NET"){
          break;
        }
        if(tokens[i][0] == '*' and 
            tokens[i].find_first_not_of("0123456789", 1) == std::string_view::npos){
          port_num ++;
        }
        if(tokens[i].size() == 1){
          switch(tokens[i][0]){
            case 'I': input_num++;  break;
            case 'O': output_num++; break;
            case 'B': inout_num++;  break;
            default: break;
          }
        }
      }
      REQUIRE(parser.ports.size() == port_num);

      REQUIRE(std::count_if(parser.ports.begin(), parser.ports.end(), 
            [](const auto &p){return p.direction == spef::ConnectionDirection::INPUT;})  == input_num);
      REQUIRE(std::count_if(parser.ports.begin(), parser.ports.end(), 
            [](const auto &p){return p.direction == spef::ConnectionDirection::OUTPUT;}) == output_num);
      REQUIRE(std::count_if(parser.ports.begin(), parser.ports.end(), 
            [](const auto &p){return p.direction == spef::ConnectionDirection::INOUT;})  == inout_num);
    }
    else{
      REQUIRE(parser.ports.size() == 0);
    }


    // Check net section
    REQUIRE(parser.nets.size() == net_beg.size());
    if(not parser.nets.empty()){
      for(size_t i=0; i<net_beg.size(); ++i){
        auto beg = conn_beg[i]+1;
        auto end = cap_beg[i];
        size_t conn_num {0};
        for(size_t j=beg; j<end; j++){
          if(tokens[j] == "*I" or tokens[j] == "*P"){
            conn_num ++;
          }
        }
        REQUIRE(parser.nets[i].connections.size() == conn_num);

        beg = cap_beg[i] + 1;
        end = res_beg[i];
        size_t cap_num {0};
        for(size_t j=beg; j<end; j++){
          if(tokens[j].find_first_not_of("0123456789") == std::string_view::npos){
            cap_num ++;
          }
        }
        REQUIRE(parser.nets[i].caps.size() == cap_num);

        beg = res_beg[i] + 1;
        end = net_end[i];
        size_t res_num {0};
        for(size_t j=beg; j<end; j+=4){
          REQUIRE(tokens[j].find_first_not_of("0123456789") == std::string_view::npos);
          res_num ++;
        }
        REQUIRE(parser.nets[i].ress.size() == res_num);
      }
    }
  }
}


