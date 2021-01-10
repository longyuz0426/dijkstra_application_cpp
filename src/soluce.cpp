#include <iostream>
#include <string>
#include <unordered_map>
#include "Generic_mapper.hpp"
#include "myClass.hpp"
using namespace std;
template<class out_type,class in_value>
out_type str2Int(const in_value & t)
{
    std::stringstream stream;
    stream<<t;
    out_type result;
    stream>>result;
    return result;

}
int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout<<"Please input file Name"<<std::endl;
		return 1;
	}
	std::cout<<"Arg 1: stations"<<std::endl;
	std::cout<<"Arg 2: connections"<<std::endl;
	std::string stationFile(argv[1]);
	std::string connectFile(argv[2]);
	travel::Soluce soluce;
	soluce.read_stations(stationFile);
	soluce.read_connections(connectFile);
    if (argc == 5) {
 	std::cout<<"Arg 3: start"<<std::endl;
      std::cout<<"Arg 4: end"<<std::endl;
	uint64_t start = str2Int<uint64_t>(argv[3]);
        uint64_t end = str2Int<uint64_t>(argv[4]);
        soluce.dijkstra(start, end);

    }
    return 0;
}