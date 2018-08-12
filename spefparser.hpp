#ifndef SPEF_HPP_
#define SPEF_HPP_

// TODO:
// add constructor for sub structs so we can fully utilize the try_emplace

#include <pegtl/include/tao/pegtl.hpp>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <utility>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <array>
#include <string_view>
#include <optional>
#include <experimental/filesystem>
#include <fstream>
#include <cmath>

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




// Function: split_on_space 
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
    case ConnectionDirection::INPUT  : os << 'I';  break;
    case ConnectionDirection::OUTPUT : os << 'O'; break;
    case ConnectionDirection::INOUT  : os << 'B';  break;
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
  bool operator ==(const Connection& rhs) const;
  bool operator !=(const Connection& rhs) const;
};

inline bool Connection::operator != (const Connection& rhs) const {
  return not (*this == rhs);
}

inline bool Connection::operator == (const Connection& rhs) const {
  auto is_same_float = [](float a, float b){ 
    if(::fabs(a-b) > 1e-3){
      return false;
    }
    return true;
  };

  if(name != rhs.name or type != rhs.type or direction != rhs.direction or 
    driving_cell != rhs.driving_cell){
    return false;
  }

  if(load.has_value() and rhs.load.has_value()){
    if(not is_same_float(*load, *rhs.load)){
      return false;
    }
  }
  else{
    if(load != rhs.load){
      return false;
    }
  }
 
  if(coordinate.has_value() and rhs.coordinate.has_value()){
    auto& x = std::get<0>(*coordinate);
    auto& y = std::get<1>(*coordinate);
    auto& rx = std::get<0>(*rhs.coordinate);
    auto& ry = std::get<1>(*rhs.coordinate);
    if(not is_same_float(x, rx) or not is_same_float(y, ry)){
      return false;
    }
  }
  else{
    if(coordinate != rhs.coordinate){
      return false;
    }
  }  
  return true;
}

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
  Net(const std::string& s, const float f): name{s}, lcap{f} {}

  bool operator ==(const Net& rhs) const;
  bool operator !=(const Net& rhs) const;
};

inline bool Net::operator !=(const Net& rhs) const{
  return not (*this == rhs);
}

inline bool Net::operator == (const Net& rhs) const {
  if(name != rhs.name or ::fabs(lcap - rhs.lcap) > 1e-3)
    return false;
  if(connections != rhs.connections)
    return false;
  if(caps.size() != rhs.caps.size() or ress.size() != rhs.ress.size()){
    return false;
  }

  auto is_same_tuple = 
  [](const std::tuple<std::string, std::string, float>& l, 
     const std::tuple<std::string, std::string, float>& r){
    const auto& [l1, l2, l3] = l;
    const auto& [r1, r2, r3] = r;
    if(l1 != r1 or l2 != r2){
      return false;
    }
    if(::fabs(l3-r3) > 1e-3){
      return false;
    }
    return true;
  };

  for(size_t i=0; i<caps.size(); i++){
    if(not is_same_tuple(caps[i], rhs.caps[i])){
      return false;
    }
  }

  for(size_t i=0; i<ress.size(); i++){
    if(not is_same_tuple(ress[i], rhs.ress[i])){
      return false;
    }
  }

  return true;
}


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
  for(size_t i=0; i<n.caps.size(); ++i){
    os << i+1 << ' ' << std::get<0>(n.caps[i]);
    if(not std::get<1>(n.caps[i]).empty()){
      os << ' ' << std::get<1>(n.caps[i]);
    }
    os << ' ' << std::get<2>(n.caps[i]) << '\n';
  }
  if(not n.ress.empty()){
    os << "*RES\n";
  }
  for(size_t i=0; i<n.ress.size(); ++i){
    os << i+1 << ' ' << std::get<0>(n.ress[i]) << ' ' 
      << std::get<1>(n.ress[i]) << ' ' << std::get<2>(n.ress[i]) << '\n';
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
  void clear();

  template <typename T>
  friend struct Action;

  friend void split_on_space(const char*, const char*, std::vector<std::string_view>&);

  bool read(const std::experimental::filesystem::path &);

  // TODO: what is the terminology?
  void name_expansion();              // Expand everything
  void name_expansion(Net&);
  void name_expansion(Port&);

  private:
  
    Net* _current_net {nullptr};
    std::vector<std::string_view> _tokens;
    std::unordered_map<size_t, std::string_view> _name_map;
};

inline void Spef::clear(){
  standard.clear();
  design_name.clear();
  date.clear();
  vendor.clear();
  program.clear();
  version.clear();
  design_flow.clear();
  divider.clear();
  delimiter.clear();
  bus_delimiter.clear();

  time_unit.clear();
  capacitance_unit.clear();
  resistance_unit.clear();
  inductance_unit.clear();

  name_map.clear();
  ports.clear();
  nets.clear();
}



inline std::string Spef::dump() const {
  std::ostringstream os;
  os 
    << "*SPEF "          <<  standard         << '\n' 
    << "*DESIGN "        <<  design_name      << '\n' 
    << "*DATE "          <<  date             << '\n' 
    << "*VENDOR "        <<  vendor           << '\n'
    << "*PROGRAM "       <<  program          << '\n'
    << "*VERSION "       <<  version          << '\n'
    << "*DESIGN_FLOW "   <<  design_flow      << '\n'
    << "*DIVIDER "       <<  divider          << '\n'
    << "*DELIMITER "     <<  delimiter        << '\n'
    << "*BUS_DELIMITER " <<  bus_delimiter    << '\n'
    << "*T_UNIT "        <<  time_unit        << '\n'
    << "*C_UNIT "        <<  capacitance_unit << '\n'
    << "*R_UNIT "        <<  resistance_unit  << '\n'
    << "*L_UNIT "        <<  inductance_unit  << '\n'
  ;
  os << '\n';

  if(not name_map.empty()){
    os << "*NAME_MAP\n";
  }
  for(const auto& [k,v]: name_map){
    os << k << ' ' << v << '\n';
  }
  os << '\n';
  if(not ports.empty()){
    os << "*PORTS\n";
  }
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




using RuleToken = pegtl::until<pegtl::at<pegtl::sor<pegtl::space, pegtl::one<'*'>, pegtl::eof>>>;
//using RuleToken = pegtl::sor<pegtl::until<pegtl::at<pegtl::space>>, pegtl::until<pegtl::at<pegtl::one<'*'>>>>;
//pegtl::until<pegtl::at<pegtl::sor<pegtl::space, pegtl::one<'*'>>>>;
using RuleDontCare = pegtl::star<pegtl::space>;
using RuleSpace = pegtl::plus<pegtl::space>;

struct RuleDouble : pegtl::seq<double_::rule, pegtl::at<pegtl::sor<pegtl::space, pegtl::one<'*'>>>>
{};


template<typename T>
struct Action: pegtl::nothing<T>
{};

struct Quote: pegtl::string<'"'>
{};
struct QuotedString: pegtl::if_must<Quote, pegtl::until<Quote>>
{};

struct Header: pegtl::plus<pegtl::seq<QuotedString, pegtl::star<RuleSpace, QuotedString>>>
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
    // Remove space in middle 
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

struct RuleStandard: pegtl::seq<TAO_PEGTL_STRING("*SPEF"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleStandard>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.standard = in.string().erase(0, header_begin(in.begin()+sizeof("*SPEF"))-in.begin());
  };
};

struct RuleDesign: pegtl::seq<TAO_PEGTL_STRING("*DESIGN"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleDesign>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.design_name = in.string().erase(0, header_begin(in.begin()+sizeof("*DESIGN"))-in.begin());
  };
};

struct RuleDate: pegtl::seq<TAO_PEGTL_STRING("*DATE"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleDate>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.date = in.string().erase(0, header_begin(in.begin()+sizeof("*DATE"))-in.begin());
  };
};


struct RuleVendor: pegtl::seq<TAO_PEGTL_STRING("*VENDOR"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleVendor>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.vendor = in.string().erase(0, header_begin(in.begin()+sizeof("*VENDOR"))-in.begin());
  };
};


struct RuleProgram: pegtl::seq<TAO_PEGTL_STRING("*PROGRAM"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleProgram>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.program = in.string().erase(0, header_begin(in.begin()+sizeof("*PROGRAM"))-in.begin());
  };
};

struct RuleVersion: pegtl::seq<TAO_PEGTL_STRING("*VERSION"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleVersion>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.version = in.string().erase(0, header_begin(in.begin()+sizeof("*VERSION"))-in.begin());
  };
};

struct RuleDesignFlow: pegtl::seq<TAO_PEGTL_STRING("*DESIGN_FLOW"), pegtl::opt<RuleSpace, Header>>
{};
template<>
struct Action<RuleDesignFlow>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    d.design_flow = in.string().erase(0, header_begin(in.begin()+sizeof("*DESIGN_FLOW"))-in.begin());
  };
};

struct RuleDivider: pegtl::seq<TAO_PEGTL_STRING("*DIVIDER"), pegtl::opt<RuleSpace, Divider>>
{};

struct RuleDelimiter: pegtl::seq<TAO_PEGTL_STRING("*DELIMITER"), pegtl::opt<RuleSpace, Delimiter>>
{};

struct RuleBusDelimiter: pegtl::seq<TAO_PEGTL_STRING("*BUS_DELIMITER"), pegtl::opt<RuleSpace, BusDelimiter>>
{};


struct RuleUnit: pegtl::seq<TAO_PEGTL_STRING("*"), pegtl::one<'T','C','R','L'>,
  TAO_PEGTL_STRING("_UNIT"), 
  pegtl::must<RuleSpace, double_::rule, RuleSpace, RuleToken>
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
//struct RuleNameMapBeg: pegtl::seq<TAO_PEGTL_STRING("*NAME_MAP"), RuleSpace>
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
  TAO_PEGTL_STRING("*"), pegtl::must<RuleToken, RuleSpace, RuleToken>
>
{};
template <>
struct Action<RuleNameMap>  
{
  template <typename Input>
  static void apply(const Input& in, Spef& d){
    // Skip the '*' 
    split_on_space(in.begin(), in.end(), d._tokens); 
    d.name_map.try_emplace(std::string{d._tokens[0]}, std::string{d._tokens[1]});
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

// TODO: Ignore C/L/S for now
struct RulePort: pegtl::seq<
  pegtl::not_at<TAO_PEGTL_STRING("*D_NET")>, TAO_PEGTL_STRING("*"),
  pegtl::must<
    RuleToken, RuleSpace,
    pegtl::must<pegtl::one<'I','O','B'>>,
    pegtl::star<pegtl::sor<
      pegtl::seq<
        RuleSpace, pegtl::seq<TAO_PEGTL_STRING("*C"), RuleSpace, double_::rule, RuleSpace, double_::rule>
      >,
      pegtl::seq<
        RuleSpace, pegtl::seq<TAO_PEGTL_STRING("*L"), RuleSpace, double_::rule>
      >,
      pegtl::seq<
        RuleSpace, pegtl::seq<TAO_PEGTL_STRING("*S"), RuleSpace, double_::rule, RuleSpace, double_::rule>
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

using RuleVar = pegtl::until<pegtl::at<pegtl::sor<pegtl::space, pegtl::eof>>>;


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
  RuleSpace, RuleVar, RuleSpace, pegtl::must<pegtl::one<'I','O','B'>>, 
  
  pegtl::star<pegtl::sor<
    pegtl::seq<RuleSpace, pegtl::seq<TAO_PEGTL_STRING("*C"), RuleSpace, double_::rule, 
      RuleSpace, double_::rule>>,

    pegtl::seq<RuleSpace, pegtl::seq<TAO_PEGTL_STRING("*L"), RuleSpace, double_::rule>>,

    pegtl::seq<RuleSpace, pegtl::seq<TAO_PEGTL_STRING("*D"), RuleSpace, RuleToken>>
      //pegtl::plus<pegtl::identifier_other>>>
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
  pegtl::plus<pegtl::digit>, RuleSpace, RuleVar, RuleSpace, RuleDouble
  //double_::rule
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
  pegtl::plus<pegtl::digit>, RuleSpace, RuleVar, RuleSpace, RuleVar, RuleSpace, RuleDouble
  //double_::rule
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
  pegtl::plus<pegtl::digit>, RuleSpace,
  RuleVar, RuleSpace, RuleVar, RuleSpace, double_::rule
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
  TAO_PEGTL_STRING("*D_NET"), pegtl::must<RuleSpace, RuleVar, RuleSpace, double_::rule>
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
      //std::cout << d.dump() << "\n\n\n";
      //std::cout << "=>" << in.string() << "<=\n";
      throw tao::pegtl::parse_error("Unrecognized token", in);
    }
  }
};





// Spef Top Rule ----------------------------------------------------------------------------------- 
struct RuleSpef: pegtl::must<pegtl::star<pegtl::space>,
  pegtl::rep_max<10, 
    pegtl::sor<
      pegtl::seq<RuleStandard,     RuleDontCare>,
      pegtl::seq<RuleDate,         RuleDontCare>,
      pegtl::seq<RuleVendor,       RuleDontCare>,
      pegtl::seq<RuleProgram,      RuleDontCare>,
      pegtl::seq<RuleVersion,      RuleDontCare>,
      pegtl::seq<RuleDesignFlow,   RuleDontCare>,
      pegtl::seq<RuleDesign,       RuleDontCare>,
      pegtl::seq<RuleDivider,      RuleDontCare>,
      pegtl::seq<RuleDelimiter,    RuleDontCare>, 
      pegtl::seq<RuleBusDelimiter, RuleDontCare>>
  >,

  pegtl::rep_max<4, pegtl::seq<RuleUnit, RuleDontCare>>,

  pegtl::opt<RuleNameMapBeg, pegtl::star<pegtl::seq<RuleNameMap, RuleDontCare>>>,

  pegtl::opt<RulePortBeg,    pegtl::star<pegtl::seq<RulePort, RuleDontCare>>>,

  pegtl::star<
    pegtl::if_must<
      RuleNetBeg, RuleDontCare,
      pegtl::opt<pegtl::seq<RuleConnBeg, RuleDontCare>, pegtl::star<pegtl::seq<RuleConn, RuleDontCare>>>,
      pegtl::opt<pegtl::seq<RuleCapBeg,  RuleDontCare>, 
        pegtl::star<pegtl::seq<pegtl::sor<RuleCapGround, RuleCapCouple>, RuleSpace>>>,
      pegtl::opt<pegtl::seq<RuleResBeg,  RuleDontCare>, pegtl::star<pegtl::seq<RuleRes, RuleSpace>>>, 
      RuleNetEnd, RuleDontCare
    >
  >,
  pegtl::star<pegtl::space>, 
  RuleInputEnd
>
{};


// Error control ----------------------------------------------------------------------------------
template<typename Rule>
struct Control : tao::pegtl::normal<Rule>
{
   static const std::string error_message;

   template<typename Input, typename... States>
   static void raise(const Input& in, States&&...)
   {
     throw tao::pegtl::parse_error(error_message, in);
   }
};

template<typename T> const std::string Control<T>::error_message = "Fail to match the Spef rule: " + 
  tao::pegtl::internal::demangle< T>() ;




// API for parsing --------------------------------------------------------------------------------

std::string file_to_memory(const std::experimental::filesystem::path &p){
  if(not std::experimental::filesystem::exists(p)){
    std::cout << "The provided path does not exist!\n";
    return "";
  }
  std::ifstream ifs(p);

  ifs.seekg(0, std::ios::end);
  std::string buffer(ifs.tellg(), ' ');
  ifs.seekg(0);
  ifs.read(&buffer[0], buffer.size()); 
  ifs.close();
  return std::move(buffer);
}

inline bool Spef::read(const std::experimental::filesystem::path &p){
  auto buffer {file_to_memory(p)};
  if(buffer.empty()){
    std::cout << "Error in reading file into memory, buffer is empty!\n";
    return false;
  }

  // Remove comments 
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

  // Use Lazy mode to improve performane
  tao::pegtl::memory_input<pegtl::tracking_mode::LAZY> in(buffer, "");

  try{
    tao::pegtl::parse<spef::RuleSpef, spef::Action, spef::Control>(in, *this);
    return true;
  }
  catch(const tao::pegtl::parse_error& e){

    std::cout << e.what() << std::endl;
    const auto p = e.positions.front();
    std::cout << "Fail at line " << p.line << ":\n";
    std::cout << "  " << in.line_as_string(p) << '\n';
    std::cout << "  " << std::string(p.byte_in_line, ' ') << "^" << '\n';
    return false;
  }
}

inline void string_expansion(std::string& str, 
  const std::unordered_map<size_t, std::string_view>& mapping){
  if(str.empty() or mapping.empty()) return ;
  size_t beg {str.size()};
  size_t end {0};
  size_t last;
  size_t key;
  char* endptr {nullptr};
  while(beg > 0){
    last = beg;
    -- beg;
    if(beg = str.find_last_of('*', beg); beg != std::string::npos){
      end = beg+1;
      while(end < last and std::isdigit(str[end])){
        ++ end;
      }
      endptr = (&str.data()[end]);
      key = ::strtoul(&str.data()[beg+1], &(endptr), 10);
      if(mapping.find(key) != mapping.end()){
        str.replace(beg, end-beg, mapping.at(key));
      }
    }
    else{
      break;
    }
  }
}


inline void Spef::name_expansion(){
  for(auto &p: ports){
    name_expansion(p);
  }
  for(auto &n: nets){
    name_expansion(n);
  }
}

inline void Spef::name_expansion(Port& port){
  if(_name_map.empty()){
    size_t key;
    for(auto& [k, v]: name_map){
      //std::stoi(k, &key);
      key = ::strtoul(&k.data()[1], nullptr, 10);
      _name_map.emplace(key, v);
    }
  } 
  string_expansion(port.name, _name_map);
}


inline void Spef::name_expansion(Net& net){
  if(_name_map.empty()){
    size_t key;
    for(auto& [k, v]: name_map){
      //std::stoi(k, &key);
      key = ::strtoul(&k.data()[1], nullptr, 10);
      _name_map.emplace(key, v);
    }
  }

  string_expansion(net.name, _name_map);
  for(auto &c : net.connections){
    string_expansion(c.name, _name_map);
    string_expansion(c.driving_cell, _name_map);
  }

  for(auto &t: net.caps){
    string_expansion(std::get<0>(t), _name_map);
    string_expansion(std::get<1>(t), _name_map);
  }

  for(auto &r: net.ress){
    string_expansion(std::get<0>(r), _name_map);
    string_expansion(std::get<1>(r), _name_map);
  }
}


};    // end of namespace spef. --------------------------------------------------------------------

#endif
