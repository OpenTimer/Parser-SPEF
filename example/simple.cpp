
#include "parser-spef.hpp"

int main(int argc, char* argv[]){
  //{
  //  spef::Spef parser;
  //  parser.parse_spef_file("/home/clin99/Software/spirit/aes_cipher_top.spef");
  //  //parser.parse_spef_file("/home/clin99/Software/Parser-SPEF/temp/leon2_iccad.spef");
  //  parser.name_expansion();
  //  //auto origin = parser.dump();
  //  //for(auto &n : parser.nets){
  //  //  parser.name_expansion(n);
  //  //}
  //  //for(auto &p : parser.ports){
  //  //  parser.name_expansion(p);
  //  //}
  //  //assert(origin == parser.dump());
  //  //std::cout << "nof nets = " << parser.nets.size() << "\n";
  //  std::cout << parser.dump() << "\n";
  //  return 0;
  //}

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

  //auto buffer {spef::file_to_memory(argv[1])};
  //std::vector<std::string_view> vec;
  //spef::split_on_space(buffer.data(), &(buffer.data()[buffer.size()-1]), vec);
  //for(const auto& v: vec){
  //  std::cout << v << '\n';
  //}
  //exit(1);


  // Invoke the parse_spef function and check the return value
  if(data.read(argv[1])){
    // Call dump function to see the content
    data.name_expansion();
    //std::cout << data.dump() << '\n';
  }
}
