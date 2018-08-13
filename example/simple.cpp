#include "parser-spef.hpp"

int main(int argc, char* argv[]){

  if(argc != 2){
    std::cout << "Usage: ./simple [spef_file]";
    return 1;
  }

  if(not std::experimental::filesystem::exists(argv[1])){
    std::cout << "Cannot found " <<  argv[1] << '\n';
    return 1;
  }

  // Create a data to be filled by the parser
  spef::Spef data;

  // Invoke the parse_spef function and check the return value
  if(data.read(argv[1])){
    // Call dump function to see the content
    std::cout << data.dump() << '\n';
  }
}
