#include "stat_reader.h"
#include "input_reader.h"
#include <iostream>
#include <iomanip>

namespace transport_catalogue{
namespace detail{

using namespace std;


std::vector<std::pair<RequestType,std::string>> ReadGetRequest(){
    std::vector<std::pair<RequestType,std::string>> res;
    int req_num = ReadLineWithNumber();
    string req;
    for(int i = 0;i < req_num; ++i) {
        getline(cin, req);
        if(GetReqType(req) == RequestType::BUS){
            res.push_back({RequestType::BUS, req.substr(4,req.find(':'))});
        } else {
            res.push_back({RequestType::STOP, req.substr(5,req.find(':'))});
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
                        << bus->uniq_stop_number << " unique stops, "s
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
