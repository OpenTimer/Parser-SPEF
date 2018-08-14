#include <parser-spef.hpp>

int main(int argc, char* argv[]){

  if(argc != 2){
    std::cerr << "Usage: ./simple [SPEF file]\n";
    std::exit(EXIT_FAILURE);
  }

  if(not std::experimental::filesystem::exists(argv[1])){
    std::cerr << "can't find " <<  argv[1] << '\n';
    std::exit(EXIT_FAILURE);
  }

  // Create a SPEF object
  spef::Spef spef;

  // Invoke the read function and check the return value
  if(!spef.read(argv[1])){
    std::cerr << *spef.error;
    std::exit(EXIT_FAILURE);
  }

  // TODO: 


  // Or you can dump to a SPEF.
  //std::cout << spef.dump();
}


