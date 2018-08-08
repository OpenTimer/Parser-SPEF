
#include "spefparser.hpp"  // the only include you need

int main(){
  // Create a data to be filled by the parser
  spef::Spef data;

  // Invoke the parse function and check the return value
  if(spef::parse_spef("./simple.spef", data)){
    std::cout << data.dump() << '\n';
  }
}
