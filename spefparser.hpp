#ifndef SPEF_HPP_
#define SPEF_HPP_

// TODO:
// add constructor for sub structs so we can fully utilize the try_emplace

#include <pegtl/include/tao/pegtl.hpp>
#include <iostream>
#include <charconv>
#include <cstring>
#include <algorithm>
#include <utility>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <array>
#include <map>
#include <cctype>
#include <regex>
#include <string_view>
#include <optional>
#include <experimental/filesystem>
#include <fstream>

namespace spef{

namespace double_
{
  using namespace tao::TAO_PEGTL_NAMESPACE;  // NOLINT

  struct plus_minus : opt< one< '+', '-' > > {}; 
  struct dot : one< '.' > {}; 

  struct inf : seq< istring< 'i', 'n', 'f' >,
                    opt< istring< 'i', 'n', 'i', 't', 'y' > > > {}; 

  struct nan : seq< istring< 'n', 'a', 'n' >,
                    opt< one< '(' >,
                         plus< alnum >,
                         one< ')' > > > {}; 

  template< typename D > 
  struct number : if_then_else< dot,
                                plus< D >,
                                seq< plus< D >, opt< dot, star< D > > > > {}; 

  struct e : one< 'e', 'E' > {}; 
  struct p : one< 'p', 'P' > {}; 
  struct exponent : seq< plus_minus, plus< digit > > {}; 

  struct decimal : seq< number< digit >, opt< e, exponent > > {}; 
  struct hexadecimal : seq< one< '0' >, one< 'x', 'X' >, number< xdigit >, opt< p, exponent > > {}; 

  struct rule : seq< plus_minus, sor< hexadecimal, decimal, inf, nan > > {}; 
};



// TODO:
/*
void split_on_space1(
  const char* beg,
  const char* end,  
  std::vector<std::string_view>& tokens
) {

  tokens.clear();
  size_t len {0};
  const char* itr {beg};
  while(itr != end and *itr != 0){
    if(std::isspace(*itr)) {
      // Consume space
      while(itr != end and *itr != 0 and std::isspace(*itr)){
        ++ itr;
      }
    }
    else{
      len = 0;
      // Consume non-space
      while(itr != end and *itr != 0 and not std::isspace(*itr)){
        itr ++;
        len ++;
      }
      if(len > 0){
        tokens.push_back({itr-len, len});
      }
    }
  }
}
*/


// Function: split_on_space 
///*
void split_on_space(const char* beg, const char* end, std::vector<std::string_view>& tokens) {

  // Parse the token.
  const char *token {nullptr};
  size_t len {0};

  tokens.clear();

  for(const char* itr = beg; itr != end && *itr != 0; ++itr) {
    if(std::isspace(*itr)) {
      if(len > 0) {                            // Add the current token.
        tokens.push_back({token, len});
        token = nullptr;
        len = 0;
      }
    } else {
      if(len == 0) {
        token = itr;
      }
      ++len;
    }
  }

  if(len > 0) {
    tokens.push_back({token, len});
  } 
}
//*/






enum class ConnectionType {
  INTERNAL,
  EXTERNAL
};

std::ostream& operator<<(std::ostream& os, const ConnectionType& c)
{
	switch(c){
    case ConnectionType::INTERNAL: os << "*I"; break;
    case ConnectionType::EXTERNAL: os << "*P"; break;
		default    : os.setstate(std::ios_base::failbit);
	}
	return os;
}


enum class ConnectionDirection {
  INPUT,
  OUTPUT,
  INOUT
};

std::ostream& operator<<(std::ostream& os, const ConnectionDirection& c)
{
	switch(c){
    case ConnectionDirection::INPUT  : os << "I";  break;
    case ConnectionDirection::OUTPUT : os << "O"; break;
    case ConnectionDirection::INOUT  : os << "B";  break;
		default    : os.setstate(std::ios_base::failbit);
	}
	return os;
}


struct Port {
  Port() = default;
  Port(const std::string& s): name(s){}
  std::string name;
  ConnectionDirection direction;  // I, O, B 
  //char type;  // C, L or S
  //std::vector<float> values;

  friend std::ostream& operator<<(std::ostream&, const Port&);
};

std::ostream& operator<<(std::ostream& os, const Port& p)  
{  
  os << p.name << ' ';
  switch(p.direction){
    case ConnectionDirection::INPUT:  os << 'I'; break;
    case ConnectionDirection::OUTPUT: os << 'O'; break;
    case ConnectionDirection::INOUT:  os << 'B'; break;
    default: break;
  }
  //os << p.type << ' ';
  //for(const auto&v : p.values){
  //  os << v << ' ';
  //}
  return os;  
}  


struct Connection {

  std::string name;
  ConnectionType type;
  ConnectionDirection direction;

  std::optional<std::pair<float, float>> coordinate;
  std::optional<float> load;    
  std::string driving_cell;

  Connection() = default;
  Connection(Connection&&) = default;

  Connection& operator = (Connection&&) = default;
};

std::ostream& operator<<(std::ostream& os, const Connection& c)
{
  os << c.type << ' ' << c.name << ' ' << c.direction;
  if(c.coordinate.has_value()){
    os << " *C " << std::get<0>(*c.coordinate) << ' ' << std::get<1>(*c.coordinate);
  }
  if(c.load.has_value()){
    os << " *L " << *c.load;
  }
  if(not c.driving_cell.empty()){
    os << " *D " << c.driving_cell;
  }
  return os;  
}



struct Net {
  std::string name;
  float lcap;
  std::vector<Connection> connections;
  std::vector<std::tuple<std::string, std::string, float>> caps;
  std::vector<std::tuple<std::string, std::string, float>> ress;

  Net() = default;
};

std::ostream& operator<<(std::ostream& os, const Net& n)
{
  os << "*D_NET " << n.name << ' ' << n.lcap << '\n';
  if(not n.connections.empty()){
    os << "*CONN\n";
  }
  for(const auto& c: n.connections){
    os << c << '\n';
  }
  if(not n.caps.empty()){
    os << "*CAP\n";
  }
  for(const auto& c: n.caps){
    os << std::get<0>(c);
    if(not std::get<1>(c).empty()){
      os << ' ' << std::get<1>(c);
    }
    os << ' ' << std::get<2>(c) << '\n';
  }
  if(not n.ress.empty()){
    os << "*RES\n";
  }
  for(const auto& r: n.ress){
    os << std::get<0>(r) << ' ' << std::get<1>(r) << ' ' << std::get<2>(r) << '\n';
  }
  os << "*END\n";
  return os;  
}
 
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

  template <typename T>
  friend struct Action;

  friend void split_on_space(const char*, const char*, std::vector<std::string_view>&);

  private:
  
    Net* _current_net {nullptr};
    std::vector<std::string_view> _tokens;
};

inline std::string Spef::dump() const {
  std::ostringstream os;
  os 
    << "Standard:"      << standard         << "\n" 
    << "Design name:"   << design_name      << "\n" 
    << "Date:"          << date             << "\n" 
    << "Vendor:"        << vendor           << "\n"
    << "Program:"       << program          << "\n"
    << "Version:"       << version          << "\n"
    << "Design Flow:"   << design_flow      << "\n"
    << "Divider:"       << divider          << "\n"
    << "Delimiter:"     << delimiter        << "\n"
    << "Bus Delimiter:" << bus_delimiter    << "\n"
    << "T_UNIT:"        << time_unit        << "\n"
    << "C_UNIT:"        << capacitance_unit << "\n"
    << "R_UNIT:"        << resistance_unit  << "\n"
    << "L_UNIT:"        << inductance_unit  << "\n"
  ;
  os << '\n';

  if(not name_map.empty()){
    os << "*NAME_MAP\n";
  }
  for(const auto& [k,v]: name_map){
    os << k << ' ' << v << '\n';
  }
  os << '\n';
  os << "*PORTS\n";
  for(const auto& p: ports){
    os << p << '\n';
  }
  os << '\n';
  for(const auto& net : nets) {
    os << net << '\n';
  }
  return os.str();
}




namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using RuleToken = pegtl::until<pegtl::at<pegtl::space>>;
using RuleDontCare = pegtl::plus<pegtl::space>;

template<typename T>
struct Action: pegtl::nothing<T>
{};


using Quote = pegtl::string<'"'>;
struct QuotedString: pegtl::seq<Quote, pegtl::until<Quote>>
{};

struct Header: pegtl::plus<pegtl::seq<QuotedString, pegtl::star<RuleDontCare, QuotedString>>>
{};

struct Divider: pegtl::any
{};
template<>
struct Action<Divider>  
{
  template <typename Input>
  static bool apply(const Input& in, Spef& d){
    if(in.size() != 1){
      return false;
    }
    d.divider = in.string();
    return true;
  };
};

struct Delimiter: pegtl::any
{};
template<>
struct Action<Delimiter>  
{
  template <typename Input>
  static bool apply(const Input& in, Spef& d){
    if(in.size() != 1){
      return false;
    }
    d.delimiter = in.string();
    return true;
  };
};

struct BusDelimiter: pegtl::must<pegtl::any, pegtl::star<pegtl::space>, pegtl::any>
{};
template<>
struct Action<BusDelimiter>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.bus_delimiter = in.string();
    // Remove space between middle 
    d.bus_delimiter.erase(std::remove_if(d.bus_delimiter.begin(), d.bus_delimiter.end(), 
      [](auto c){return std::isspace(c);}), d.bus_delimiter.end());
  };
};





//  Header Section -------------------------------------------------------------------------------- 

const char* header_begin(const char* beg){
  while(std::isspace(*beg)){
    ++ beg;
  }
  return beg;
}

struct RuleStandard: pegtl::seq<TAO_PEGTL_STRING("*SPEF"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleStandard>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.standard = in.string().erase(0, header_begin(in.begin()+sizeof("*SPEF"))-in.begin());
  };
};

struct RuleDesign: pegtl::seq<TAO_PEGTL_STRING("*DESIGN"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleDesign>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.design_name = in.string().erase(0, header_begin(in.begin()+sizeof("*DESIGN"))-in.begin());
  };
};

struct RuleDate: pegtl::seq<TAO_PEGTL_STRING("*DATE"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleDate>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.date = in.string().erase(0, header_begin(in.begin()+sizeof("*DATE"))-in.begin());
  };
};


struct RuleVendor: pegtl::seq<TAO_PEGTL_STRING("*VENDOR"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleVendor>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.vendor = in.string().erase(0, header_begin(in.begin()+sizeof("*VENDOR"))-in.begin());
  };
};


struct RuleProgram: pegtl::seq<TAO_PEGTL_STRING("*PROGRAM"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleProgram>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.program = in.string().erase(0, header_begin(in.begin()+sizeof("*PROGRAM"))-in.begin());
  };
};

struct RuleVersion: pegtl::seq<TAO_PEGTL_STRING("*VERSION"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleVersion>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.version = in.string().erase(0, header_begin(in.begin()+sizeof("*VERSION"))-in.begin());
  };
};

struct RuleDesignFlow: pegtl::seq<TAO_PEGTL_STRING("*DESIGN_FLOW"), pegtl::must<RuleDontCare, Header>>
{};
template<>
struct Action<RuleDesignFlow>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.design_flow = in.string().erase(0, header_begin(in.begin()+sizeof("*DESIGN_FLOW"))-in.begin());
  };
};

struct RuleDivider: pegtl::seq<TAO_PEGTL_STRING("*DIVIDER"), pegtl::must<RuleDontCare, Divider>>
{};

struct RuleDelimiter: pegtl::seq<TAO_PEGTL_STRING("*DELIMITER"), pegtl::must<RuleDontCare, Delimiter>>
{};

struct RuleBusDelimiter: pegtl::seq<TAO_PEGTL_STRING("*BUS_DELIMITER"), pegtl::must<RuleDontCare, BusDelimiter>>
{};


struct RuleUnit: pegtl::seq<TAO_PEGTL_STRING("*"), pegtl::one<'T','C','R','L'>,
  TAO_PEGTL_STRING("_UNIT"), 
  pegtl::must<RuleDontCare, double_::rule, 
    RuleDontCare, pegtl::opt<pegtl::one<'K','M','U','N','P','F'>>, 
    pegtl::sor<TAO_PEGTL_STRING("HENRY"), TAO_PEGTL_STRING("OHM"), pegtl::one<'S','F','H'>>>
>
{};
template <>
struct Action<RuleUnit>  
{
  template <typename Input>
  static bool apply(const Input& in, Spef& d){
    switch(in.peek_char(1)){
      case 'T': d.time_unit = in.string().erase(0, header_begin(in.begin()+sizeof("*T_UNIT"))-in.begin());
        break;
      case 'C': d.capacitance_unit = in.string().erase(0, header_begin(in.begin()+sizeof("*C_UNIT"))-in.begin());
        break;
      case 'R': d.resistance_unit = in.string().erase(0, header_begin(in.begin()+sizeof("*R_UNIT"))-in.begin());
        break;
      case 'L': d.inductance_unit = in.string().erase(0, header_begin(in.begin()+sizeof("*L_UNIT"))-in.begin());
        break;
      default: break;
    }
    return true;
  }
};


//  Name Map Section -------------------------------------------------------------------------------
struct RuleNameMapBeg: pegtl::seq<TAO_PEGTL_STRING("*NAME_MAP"), RuleDontCare>
{};
template <>
struct Action<RuleNameMapBeg>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){}
};

struct RuleNameMap: pegtl::seq<
  pegtl::not_at<TAO_PEGTL_STRING("*PORTS")>, pegtl::not_at<TAO_PEGTL_STRING("*D_NET")>, 
  TAO_PEGTL_STRING("*"), pegtl::must<RuleToken, RuleDontCare, RuleToken>
>
{};
template <>
struct Action<RuleNameMap>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    split_on_space(in.begin(), in.end(), d._tokens); 
    //d.name_map.insert({vec[0], vec[1]});
    d.name_map.try_emplace( std::string{d._tokens[0]}, std::string{d._tokens[1]} );
  }
};



//  Port Section ----------------------------------------------------------------------------------

struct RulePortBeg: pegtl::seq<TAO_PEGTL_STRING("*PORTS"), RuleDontCare>
{};
template <>
struct Action<RulePortBeg>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){}
};

// TODO: All sections are optional
struct RulePort: pegtl::seq<
  pegtl::not_at<TAO_PEGTL_STRING("*D_NET")>, TAO_PEGTL_STRING("*"),
  pegtl::must<
    RuleToken, RuleDontCare,
    pegtl::must<pegtl::one<'I','O','B'>>,
    pegtl::star<pegtl::sor<
      pegtl::seq<
        RuleDontCare, pegtl::seq<TAO_PEGTL_STRING("*C"), RuleDontCare, double_::rule, RuleDontCare, double_::rule>
      >,
      pegtl::seq<
        RuleDontCare, pegtl::seq<TAO_PEGTL_STRING("*L"), RuleDontCare, double_::rule>
      >,
      pegtl::seq<
        RuleDontCare, pegtl::seq<TAO_PEGTL_STRING("*S"), RuleDontCare, double_::rule, RuleDontCare, double_::rule>
      >
    >>
  >
>
{};
template <>
struct Action<RulePort>  
{
  template <typename Input>
  static bool apply(const Input& in, Spef& d){
    split_on_space(in.begin(), in.end(), d._tokens); 

    d.ports.emplace_back(std::string{d._tokens[0]});

    // Set up port direction
    switch(d._tokens[1][0]){
      case 'O':
        d.ports.back().direction = ConnectionDirection::OUTPUT;
        break;
      case 'I':
        d.ports.back().direction = ConnectionDirection::INPUT;
        break;
      case 'B':
        d.ports.back().direction = ConnectionDirection::INOUT;
        break;
      default:
        std::cout << "Unknown port type!\n";
        return false;
        break;
    }

    //// Set up type 
    //if(d._tokens.size() > 2){
    //  p.type = d._tokens[2][1];
    //}

    //// Insert values
    //for(size_t i=3; i<d._tokens.size(); i++){
    //  p.values.emplace_back(std::strtof(d._tokens[i].data(), nullptr));
    //}
    return true;
  }
};




//  Net Section -----------------------------------------------------------------------------------

struct RuleConnBeg: pegtl::seq<TAO_PEGTL_STRING("*CONN")>
{};
template <>
struct Action<RuleConnBeg>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
  }
};


struct RuleConn: pegtl::seq<
  pegtl::sor<TAO_PEGTL_STRING("*P"), TAO_PEGTL_STRING("*I")>, 
  RuleDontCare, RuleToken, RuleDontCare, pegtl::must<pegtl::one<'I','O','B'>>, 
  
  pegtl::star<pegtl::sor<
    pegtl::seq<RuleDontCare, pegtl::seq<TAO_PEGTL_STRING("*C"), RuleDontCare, double_::rule, 
      RuleDontCare, double_::rule>>,

    pegtl::seq<RuleDontCare, pegtl::seq<TAO_PEGTL_STRING("*L"), RuleDontCare, double_::rule>>,

    pegtl::seq<RuleDontCare, pegtl::seq<TAO_PEGTL_STRING("*D"), RuleDontCare, 
      pegtl::plus<pegtl::identifier_other>>>
    >
  >
>
{};
template <>
struct Action<RuleConn>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    auto &c = d._current_net->connections.emplace_back();

    split_on_space(in.begin(), in.end(), d._tokens);

    c.type = d._tokens[0][1] == 'P' ? ConnectionType::EXTERNAL : ConnectionType::INTERNAL;
    c.name = d._tokens[1];
    switch(d._tokens[2][0]){
      case 'I':
        c.direction = ConnectionDirection::INPUT;
        break;
      case 'O':
        c.direction = ConnectionDirection::OUTPUT;
        break;
      default:
        c.direction = ConnectionDirection::INOUT;
        break;
    }

    for(size_t i=3; i<d._tokens.size(); i++){
      if(d._tokens[i].compare("*C") == 0){
        c.coordinate = std::make_pair(
          std::strtof(d._tokens[i+1].data(), nullptr), std::strtof(d._tokens[i+2].data(), nullptr)
        );
        i += 2;
      }
      else if(d._tokens[i].compare("*L") == 0){
        c.load = std::strtof(d._tokens[i+1].data(), nullptr);
        i += 1;
      }
      else if(d._tokens[i].compare("*D") == 0){
        c.driving_cell = d._tokens[i+1];
        i += 1;
      }
      else{
        std::cout << "Unknown connection information!\n";
      }
    }
  }
};

struct RuleCapBeg: pegtl::seq<TAO_PEGTL_STRING("*CAP")>
{};
template <>
struct Action<RuleCapBeg>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){}
};


struct RuleCapGround: pegtl::seq<
  pegtl::plus<pegtl::digit>, RuleDontCare, RuleToken, RuleDontCare, double_::rule
>
{};
template <>
struct Action<RuleCapGround>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    split_on_space(in.begin(), in.end(), d._tokens);
    // TODO: verify...?
    d._current_net->caps.emplace_back(
      std::forward_as_tuple(d._tokens[1], "", std::strtof(d._tokens[2].data(), nullptr))
    );
  }
};


struct RuleCapCouple: pegtl::seq<
  pegtl::plus<pegtl::digit>, RuleDontCare, RuleToken, RuleDontCare, RuleToken, RuleDontCare, double_::rule
>
{};
template <>
struct Action<RuleCapCouple>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    split_on_space(in.begin(), in.end(), d._tokens);
    d._current_net->caps.emplace_back(
      std::forward_as_tuple(d._tokens[1], d._tokens[2], std::strtof(d._tokens[3].data(), nullptr))
    );
  }
};



struct RuleResBeg: pegtl::seq<TAO_PEGTL_STRING("*RES")>
{};
template <>
struct Action<RuleResBeg>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){}
};

struct RuleRes: pegtl::seq<
  pegtl::plus<pegtl::digit>, RuleDontCare,
  RuleToken, RuleDontCare, RuleToken, RuleDontCare, double_::rule
>
{};
template <>
struct Action<RuleRes>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    split_on_space(in.begin(), in.end(), d._tokens);
    d._current_net->ress.emplace_back(
      std::forward_as_tuple(d._tokens[1], d._tokens[2], std::strtof(d._tokens[3].data(), nullptr))
    );
  }
};

struct RuleNetBeg: pegtl::seq<
  TAO_PEGTL_STRING("*D_NET"), pegtl::must<RuleDontCare, RuleToken, RuleDontCare, double_::rule>
>
{};
template <>
struct Action<RuleNetBeg>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    split_on_space(in.begin(), in.end(), d._tokens);
    
    d._current_net = &(d.nets.emplace_back());
    d._current_net->name = d._tokens[1];
    d._current_net->lcap = std::strtof(d._tokens[2].data(), nullptr);
  }
};


struct RuleNetEnd: pegtl::seq<TAO_PEGTL_STRING("*END")>
{};
template <>
struct Action<RuleNetEnd>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){}
};


struct RuleInputEnd: pegtl::star<pegtl::any>
{};
template <>
struct Action<RuleInputEnd>
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    if(in.size() != 0){
      throw tao::pegtl::parse_error("Unrecognized token", in);
    }
  }
};





// Spef Top Rule -----------------------------------------------------------------------------------
struct RuleSpef: pegtl::must<pegtl::star<pegtl::space>,

  pegtl::opt<pegtl::seq<RuleStandard,     RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleDesign,       RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleDate,         RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleVendor,       RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleProgram,      RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleVersion,      RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleDesignFlow,   RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleDivider,      RuleDontCare>>,
  pegtl::opt<pegtl::seq<RuleDelimiter,    RuleDontCare>>, 
  pegtl::opt<pegtl::seq<RuleBusDelimiter, RuleDontCare>>,

  pegtl::rep_max<4, pegtl::seq<RuleUnit, RuleDontCare>>,

  pegtl::opt<RuleNameMapBeg, pegtl::star<pegtl::seq<RuleNameMap, RuleDontCare>>>,

  pegtl::opt<RulePortBeg,    pegtl::star<pegtl::seq<RulePort, RuleDontCare>>>,

  pegtl::star<
    pegtl::if_must<
      RuleNetBeg, RuleDontCare,
      pegtl::opt<pegtl::seq<RuleConnBeg, RuleDontCare>, pegtl::star<pegtl::seq<RuleConn, RuleDontCare>>>,
      pegtl::opt<pegtl::seq<RuleCapBeg,  RuleDontCare>, 
        pegtl::star<pegtl::seq<pegtl::sor<RuleCapGround, RuleCapCouple>, RuleDontCare>>>,
      pegtl::opt<pegtl::seq<RuleResBeg,  RuleDontCare>, pegtl::star<pegtl::seq<RuleRes, RuleDontCare>>>, 
      RuleNetEnd, RuleDontCare
    >
  >,
  pegtl::star<pegtl::space>, 
  RuleInputEnd
>
{};



// Error control ----------------------------------------------------------------------------------
template<typename Rule>
struct ParserControl : tao::pegtl::normal<Rule>
{
   static const std::string error_message;

   template< typename Input, typename... States >
   static void raise(const Input& in, States&&...)
   {
     throw tao::pegtl::parse_error(error_message, in);
   }
};

template<typename T> const std::string ParserControl<T>::error_message = "Fail to match the Spef rule: \033[31m" + 
  tao::pegtl::internal::demangle< T>() + "\033[0m";



inline bool parse_spef(const std::experimental::filesystem::path &p, Spef& sf){
  if(not std::experimental::filesystem::exists(p)){
    std::cout << "The provided path does not exist!\n";
    return false;
  }

  std::ifstream ifs(p);

  ifs.seekg(0, std::ios::end);
  std::string buffer(ifs.tellg(), ' ');
  ifs.seekg(0);
  ifs.read(&buffer[0], buffer.size()); 
  ifs.close();

  for(size_t i=0; i<buffer.size(); i++){
    if(buffer[i] == '/' && i+1 < buffer.size() && buffer[i+1] == '/') {
      buffer[i] = buffer[i+1] = ' ';
      for(i=i+2; i<buffer.size(); ++i) {
        if(buffer[i] == '\n' || buffer[i] == '\r') {
          break;
        }
        else buffer[i] = ' ';
      }
    }
  }

  tao::pegtl::memory_input<> in(buffer, "");
  try{
    tao::pegtl::parse<spef::RuleSpef, spef::Action, spef::ParserControl>(in, sf);
    return true;
  }
  catch(const tao::pegtl::parse_error& e){
    std::cout << e.what() << std::endl;
    const auto p = e.positions.front();
    std::cout << "Fail at line " << p.line << ":\n";
    std::cout << "  " << in.line_as_string(p) << '\n';
    std::cout << "  " << std::string(p.byte_in_line, ' ') << "\033[31m^\033[0m" << '\n';
    return false;
  }
}



};    // end of namespace spef. --------------------------------------------------------------------

#endif
