#pragma once
#include "Generic_connection_parser.hpp"

namespace travel {
#define MAX 3276
template<class out_type,class in_value>
out_type convert(const in_value & t)
{
    std::stringstream stream;
    stream<<t;
    out_type result;
    stream>>result;
    return result;

}
class Soluce : public Generic_mapper
{ 
		public:
			virtual void read_stations(const std::string& _filename) override
			{
				std::ifstream infile(_filename.c_str(), std::ios::in);
				if (!infile.is_open()) {
					std::cout<<"File open file:"<<_filename<<std::endl;
					return ;
				}
				std::string read_string;
				std::getline(infile,read_string); //read first line, this line unuse
				//std::cout<<read_string<<std::endl; //print station infor 
				while (!infile.eof()) {
					Station create_station;
					std::getline(infile,read_string);
					
					size_t found_name = read_string.find(",",0); // find name_station
					std::string name_station = read_string.substr(0, found_name);
					
					size_t found_id = read_string.find(",", found_name+1); //find line id
					std::string line_id = read_string.substr(found_name+1, found_id-found_name-1);
					
					size_t found_short_line = read_string.find(",", found_id+1); //find short line
					std::string short_line = read_string.substr(found_id+1, found_short_line-found_id-1);
					
					size_t found_addr = read_string.find(",", found_short_line+1); //find address
					std::string addr = read_string.substr(found_short_line+1, found_addr-found_short_line-1);
					
					std::string des = read_string.substr(found_addr+1); //get des
					
					create_station.name = name_station;
					create_station.line_id = line_id;
					create_station.address = addr;
					create_station.line_name = short_line; 
					create_station.des = des;
					uint64_t id = convert<uint64_t>(line_id);
					
					stations_hashmap.insert(std::unordered_map<uint64_t, Station>::value_type(id, create_station)); // add new station, key is line id
					// print station info
					//std::cout<<" "<<name_station<<" "<<line_id<<" "<<short_line<<" "<<addr<<" "<<des<<std::endl; //print station infor
					  std::cout<<create_station<<std::endl;
				}
			};
		
		virtual void read_connections(const std::string& _filename) override
		{
				std::ifstream infile(_filename.c_str(), std::ios::in);
				if (!infile.is_open()) {
					std::cout<<"File open file:"<<_filename<<std::endl;
					return ;
				}
				std::string read_string;
				std::getline(infile,read_string);
				// std::cout<<read_string<<std::endl; //print connection infor
				while (!infile.eof()) {
					
					std::getline(infile,read_string);
					size_t found_from = read_string.find(",",0); //find from id
					uint64_t from_id = convert<uint64_t>(read_string.substr(0, found_from));
					
					size_t found_to = read_string.find(",", found_from+1); // find to id
					uint64_t to_id = convert<uint64_t>(read_string.substr(found_from+1, found_to-found_from-1));
					
					uint64_t min = convert<uint64_t>(read_string.substr(found_to+1));
					std::unordered_map<uint64_t, uint64_t> toMap;
					toMap.insert(std::make_pair(to_id, min)); 
					if (connections_hashmap.find(from_id) != connections_hashmap.end()) { // if have from_id, just add topMap to it
						connections_hashmap[from_id].insert(std::make_pair(to_id, min));
					} else { // if not have from_id, should insert
						std::unordered_map<uint64_t, uint64_t> toMap;
						toMap.insert(std::make_pair(to_id, min));
						connections_hashmap.insert(std::make_pair(from_id, toMap));
					}
					//std::cout<<" "<<from_id<<" "<<to_id<<" "<<min<<std::endl; //print connection infor
				}
		};


	  virtual std::vector<std::pair<uint64_t,uint64_t> > compute_travel(uint64_t _start, uint64_t _end) override
	  {
		std::vector<std::pair<uint64_t,uint64_t> > retVec;
		const std::unordered_map<uint64_t,std::unordered_map<uint64_t,uint64_t> >& conn_hashMap = this->get_connections_hashmap();
		std::unordered_map<uint64_t,std::unordered_map<uint64_t,uint64_t> >::const_iterator find_start = conn_hashMap.find(_start); 
		if (find_start == conn_hashMap.end()) {
			std::cout<<"failed start station!"<<std::endl;
		} else {
			std::unordered_map<uint64_t,uint64_t> stopMap = find_start->second;
			std::unordered_map<uint64_t,uint64_t>::iterator find_stop = stopMap.find(_end);
			if (find_stop != stopMap.end()) {
				retVec.push_back(std::make_pair(find_stop->first, find_stop->second));
			}
		}
		return retVec;
	  };
	  
	virtual std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(uint64_t _start, uint64_t _end) override
	{
        (void)(_start);
        (void)_end;
        std::vector<std::pair<uint64_t,uint64_t> > retVec;
        return retVec;
	};
	virtual std::vector<std::pair<uint64_t,uint64_t> > compute_travel(const std::string&, const std::string&)override{
        std::vector<std::pair<uint64_t,uint64_t> > retVec;
        return retVec;
    };
	virtual std::vector<std::pair<uint64_t,uint64_t> > compute_and_display_travel(const std::string&, const std::string&)override{
        std::vector<std::pair<uint64_t,uint64_t> > retVec;
        return retVec;
    };

    void  display(std::vector<uint64_t>& _station_vec, uint64_t _start, uint64_t _end)
    {
        const std::unordered_map<uint64_t,std::unordered_map<uint64_t,uint64_t> >& connection_hashmap = this->get_connections_hashmap();
	std::cout<<std::endl<<"Best way from "<<stations_hashmap[_start].name<<" (line "<<stations_hashmap[_start].line_name<<") to "<<
	    stations_hashmap[_end].name<<" (line "<<stations_hashmap[_end].line_name<<"): "<<std::endl;
	uint64_t all_time = 0;
        std::vector<uint64_t>::iterator it= _station_vec.begin();
	std::string stationName;
        for (; it != _station_vec.end(); ) { // print path;
            Station  station = stations_hashmap[*it];
	   stationName = station.name;
            
            if (it+1 != _station_vec.end()) { //compute time
                uint64_t line_id = convert<uint64_t>(station.line_id);
                auto it_find = connection_hashmap.find(line_id);
                if (it_find != connection_hashmap.end()) {
                    auto sec_map = it_find->second;
		    Station nextStation = stations_hashmap[*(++it)];
                    line_id = convert<uint64_t>(nextStation.line_id);
                    auto it_sec = sec_map.find(line_id);
		    uint64_t time = it_sec->second;
		    
		     if (nextStation.name == station.name) { //walk
		       std::cout<<"Walk to "<<station.name<<", line "<<nextStation.line_name<<" ("<<time<<" secs)"<<std::endl;
		       std::cout<<"Take line "<<nextStation.line_name<<" "<<nextStation.des<<std::endl;
		       stationName = nextStation.name;
		       
		     } else {
		       while (nextStation.line_name == station.line_name) {
			 //std::cout<<station.name<<" "<<station.line_id<<" "<<nextStation.line_id<<" "<<station.line_name<<" "<<nextStation.line_name<<" "<<nextStation.name<<std::endl;
			 //std::cout<<time<<std::endl;
			 station = nextStation;
			 ++it;
			 nextStation = stations_hashmap[*it];
			 
			 line_id = convert<uint64_t>(station.line_id);
			 auto it_find_conn = connection_hashmap.find(line_id);
			 sec_map = it_find_conn->second;
			 it_sec = sec_map.find(convert<uint64_t>(nextStation.line_id));
			
			//  std::cout<<" "<<station.line_id<<" "<<nextStation.line_id<<std::endl;
			 if (nextStation.line_name != station.line_name) {
			   it--; 
			   break;
			}
			 if (it_sec != sec_map.end())
			  time += it_sec->second;			 
		      }
		      if (station.line_id == stations_hashmap[_end].line_id) {
			std::cout<<"From "<<stationName<<" to "<<station.name<<" ("<<time<<" secs)"<<std::endl;
			all_time += time;
			break;
		      }
		       std::cout<<"From "<<stationName<<" to "<<nextStation.name<<" "<<nextStation.line_name<<" ("<<time<<" secs)"<<std::endl;
		       stationName = nextStation.name;
		    }
             
		      all_time += time;
                }
            }

        }
		    std::cout<<std::endl<<"After "<<all_time<<" secs , you have reached your destination!"<<std::endl;
            std::cout<<std::endl;
    }
    void dijkstra(uint64_t _start, uint64_t _end)
    {
		const std::unordered_map<uint64_t,std::unordered_map<uint64_t,uint64_t> >& connection_hashmap = this->get_connections_hashmap();
        std::unordered_map<uint64_t,uint64_t> dist;
        std::unordered_map<uint64_t,uint64_t> prev;
        std::unordered_map<uint64_t,bool> S;
        std::unordered_map<uint64_t, Station>::iterator it_s;
        for (it_s = stations_hashmap.begin(); it_s != stations_hashmap.end(); ++it_s) {
            
            dist.insert(std::make_pair(it_s->first, MAX));
            prev.insert(std::make_pair(it_s->first, 0));
            S.insert(std::make_pair(it_s->first, false));
        }
		std::unordered_map<uint64_t,uint64_t> end_map;
        auto it_find = connection_hashmap.find(_start);
        if (it_find != connection_hashmap.end()) {
            end_map = it_find->second;
        }
		const int maxint = MAX;
        std::unordered_map<uint64_t,uint64_t>::iterator it = end_map.begin();
        std::unordered_map<uint64_t,uint64_t>::iterator it_end = end_map.begin();
        std::unordered_map<uint64_t,bool>::iterator it_bool;
        for (it = dist.begin(); it != dist.end(); it++) {
            it_end = end_map.find(it->first);
            if (it_end != end_map.end()) {
                dist[it->first] = end_map[it->first];
            } else {
                dist[it->first] = MAX;
            }
            S[it->first] = false;
            if (dist[it->first] == MAX)
                prev[it->first] = -1;
            else 
                prev[it->first] = _start;
        }
		dist[_start] = 0;
		S[_start] = true;
        for (it = ++dist.begin(); it != dist.end(); it++) {
            
		//for (int i = 2; i < maxint; i++) {
			uint64_t mindist = maxint;
            uint64_t u = _start;
            //std::cout<<__LINE__<<" "<<i<<std::endl;
            std::unordered_map<uint64_t,uint64_t>::iterator it_j;
			for (it_j = dist.begin(); it_j != dist.end(); it_j++) { //find the min
				if ((!S[it_j->first]) && it_j->second < mindist) {
					u = it_j->first;
					mindist = it_j->second;
				}
			}
            //std::cout<<__LINE__<<" "<<u<<" "<<dist[u]<<std::endl;
			S[u] = true;
            it_find = connection_hashmap.find(u);
            if (it_find != connection_hashmap.end()) {
                end_map = it_find->second; // end_map is dist[u]'s connect station
            } else {
                std::cout<<"Error!"<<std::endl;
                return;
            }
            for (it_j = dist.begin(); it_j != dist.end(); it_j++) { // updata pre
			//for (int j = 1; j < maxint; j++) { // updata pre
				if ((!S[it_j->first]) && end_map[it_j->first] < MAX) {
					if ((dist[u] + end_map[it_j->first]) < it_j->second) { //_start--> u --> j  better than   _start --> j
                        it_find = connection_hashmap.find(u);
                        if ((it_find->second).find(it_j->first) != (it_find->second).end()) { // if u connect with j
						    dist[it_j->first] = dist[u] + end_map[it_j->first];
						    prev[it_j->first] = u; // save u, j prev station is u;
                           // std::cout<<__LINE__<<" "<<j<<" "<<u<<std::endl;
                        }
					}
				}
			}
		}
        std::vector<uint64_t> station_vec;
        station_vec.push_back(_end);
        uint64_t id = _end;
        while (prev[id] != _start ) { // get line start-> end;
            station_vec.insert(station_vec.begin(), prev[id]);
            id = prev[id];
        }
        station_vec.insert(station_vec.begin(), _start);
        display(station_vec, _start, _end); //print;
	};

};

}