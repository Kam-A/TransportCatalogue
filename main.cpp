#include <iostream>
#include <tuple>
#include <vector>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

void RequestProcess(transport_catalogue::TransportCatalogue& tr_cat){
    transport_catalogue::detail::Requests input_requests = transport_catalogue::detail::ReadInputRequest();
    for(const auto& add_stop_req : input_requests.stops){
        tr_cat.AddStop(add_stop_req);
    }
    for(const auto& add_stop_req : input_requests.stops){
        tr_cat.AddDistBwStops(add_stop_req);
    }
    for(const auto& add_bus_req : input_requests.buses){
        tr_cat.AddBus(add_bus_req);
    }
    std::vector<std::pair<transport_catalogue::detail::RequestType,std::string>> get_requests = transport_catalogue::detail::ReadGetRequest();
    for(const auto& [req_type, name] : get_requests){
        if (req_type == transport_catalogue::detail::RequestType::BUS) {
            transport_catalogue::detail::PrintBus(name, tr_cat.GetBusByName(name));
        }
        else {
            transport_catalogue::detail::PrintStop(name, tr_cat.GetBusesByStop(name));
        }
    }
}

int main() {
    transport_catalogue::TransportCatalogue tr_cat;
    RequestProcess(tr_cat);
    return 0;
}
