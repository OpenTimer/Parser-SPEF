#include <experimental/filesystem>
#include <iostream>


  namespace fs = std::experimental::filesystem;
int main(int argc, char* argv[]){
  for(auto& p: fs::directory_iterator("./benchmark/"))
    std::cout << p << '\n';

}
