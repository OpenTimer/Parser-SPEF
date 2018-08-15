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
  std::cout << '\n';

  spef.expand_name();
  
  if(!spef.name_map.empty()) {
    std::cout << "*NAME_MAP\n";
    for(const auto& [k, v] : spef.name_map){
      std::cout << '*' << k << ' ' << v << '\n';
    }
    std::cout << '\n';
  }
  
  if(!spef.ports.empty()) {
    std::cout << "*PORTS\n";
    for(const auto &p : spef.ports){
      std::cout << p.name << ' ';
      switch(p.direction){
        case spef::ConnectionDirection::INPUT:  std::cout << 'I' << '\n'; break;
        case spef::ConnectionDirection::OUTPUT: std::cout << 'O' << '\n'; break;
        case spef::ConnectionDirection::INOUT:  std::cout << 'B' << '\n'; break;
      }
    }
    std::cout << '\n';
  }

  for(const auto &n : spef.nets){
    std::cout << "*D_NET " << n.name << ' ' << n.lcap << '\n';
    // *CONN 
    std::cout << "*CONN\n";
    for(const auto& c : n.connections){
      std::cout << c.name << ' ';
      switch(c.type){
        case spef::ConnectionType::EXTERNAL:  std::cout << "*P" << ' '; break;
        case spef::ConnectionType::INTERNAL:  std::cout << "*I" << ' '; break;
      }
      switch(c.direction){
        case spef::ConnectionDirection::INPUT:  std::cout << 'I' << '\n'; break;
        case spef::ConnectionDirection::OUTPUT: std::cout << 'O' << '\n'; break;
        case spef::ConnectionDirection::INOUT:  std::cout << 'B' << '\n'; break;
      }
			if(c.coordinate.has_value()){
        std::cout << " *C " << std::get<0>(*c.coordinate) << ' ' << std::get<1>(*c.coordinate);
			}
			if(c.load.has_value()){
        std::cout << " *L " << *c.load;
			}
			if(not c.driving_cell.empty()){
        std::cout << " *D " << c.driving_cell;
			}
    }

    // *CAP 
    std::cout << "*CAP\n";
    for(const auto& c : n.caps){
      auto& [node1, node2, value] = c;
      std::cout << node1 << ' ' << node2 << ' ' << value << '\n';
    }

    // *RES 
    std::cout << "*RES\n";
    for(const auto& r : n.ress){
      auto& [node1, node2, value] = r;
      std::cout << node1 << ' ' << node2 << ' ' << value << '\n';
    }
    std::cout << "*END\n\n";
  }

  // Or you can dump to a SPEF.
  //std::cout << spef.dump();
}


