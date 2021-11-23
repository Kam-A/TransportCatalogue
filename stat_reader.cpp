#include "stat_reader.h"
#include "input_reader.h"
#include <iostream>
#include <iomanip>

namespace transport_catalogue{
namespace detail{

using namespace std;

string GetNameFromReq(string_view text) {
    // find name
    int64_t space = text.find(' ');
    text.remove_prefix(space + 1);
    int64_t name_end = text.find(':');
    string stop_name = string(text.substr(0,name_end));
    text.remove_prefix(name_end + 2);
    return stop_name;
}

std::vector<std::pair<RequestType,std::string>> ReadGetRequest(){
    std::vector<std::pair<RequestType,std::string>> res;
    int req_num = ReadLineWithNumber();
    string req;
    for(int i = 0;i < req_num; ++i) {
        getline(cin, req);
        if(GetReqType(req) == RequestType::BUS){
            res.push_back({RequestType::BUS, GetNameFromReq(req)});
        } else {
            res.push_back({RequestType::STOP, GetNameFromReq(req)});
        }
    }
    return res;
}

void PrintBus(string_view name, const TransportCatalogue::Bus* bus) {
    if (bus == nullptr) {
        cout << "Bus "s << name << ": not found"s << endl;
        
    } else {
        cout << "Bus "s << name << ": "s
                        << bus->stops.size() << " stops on route, "s
                        << set(bus->stops.begin(),bus->stops.end()).size() << " unique stops, "s
                        << setprecision(6)
                        << bus->real_route_len << " route length, "s
                        << bus->real_route_len / bus->route_len << " curvature"s << endl;
        
    }
    
}

void PrintStop(string_view name, optional<vector<string_view>> buses) {
    if (buses) {
        if (buses.value().size() != 0) {
            cout << "Stop "s << name << ": buses "s;
            for(string_view bus : buses.value()) {
                cout << bus << " "s ;
            }
        } else {
            cout << "Stop "s << name << ": no buses"s;
        }
        cout << endl;
    } else {
        cout << "Stop "s << name << ": not found"s << endl;
    }
}

}
}
