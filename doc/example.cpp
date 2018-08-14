#include <parser-spef.hpp>               // Parser-SPEF is header-only

int main(){
  spef::Spef parser;                     // create a parser object
  if(parser.read("simple.spef")){        // parse a .spef
    std::cout << parser.dump() << '\n';  // dump the parsed spef
  }
  else {
    std::cerr << *parser.error;          // show the error message
  }
  return 0;
}
