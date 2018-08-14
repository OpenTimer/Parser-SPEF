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
  if(not spef.read(argv[1])){
    std::cerr << *spef.error;
    std::exit(EXIT_FAILURE);
  }

  // TODO: 
  std::cout << spef.standard         << '\n';
  std::cout << spef.design_name      << '\n';
  std::cout << spef.date             << '\n';
  std::cout << spef.vendor           << '\n';
  std::cout << spef.program          << '\n';
  std::cout << spef.version          << '\n';
  std::cout << spef.design_flow      << '\n';
  std::cout << spef.divider          << '\n';
  std::cout << spef.delimiter        << '\n';
  std::cout << spef.bus_delimiter    << '\n';
  std::cout << spef.time_unit        << '\n';
  std::cout << spef.capacitance_unit << '\n';
  std::cout << spef.resistance_unit  << '\n';
  std::cout << spef.inductance_unit  << '\n';

  for(const auto& [k, v] : spef.name_map){
    std::cout << k << ' ' << v << '\n';
  }

  for(const auto &p : spef.ports){
    std::cout << p << '\n';
  }

  for(const auto &n : spef.nets){
    std::cout << n << '\n';
  }

  // Or you can dump to a SPEF.
  std::cout << spef.dump();
}


