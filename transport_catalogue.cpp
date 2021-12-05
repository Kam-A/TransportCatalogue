#include <algorithm>
#include <execution>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "transport_catalogue.h"

namespace transport_catalogue {

using namespace std;

void TransportCatalogue::AddStop(Stop stop) {
    stops_.push_back(stop);
    pointers_to_stops_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::SetDistanceBetweenStops(const Stop* src, const Stop* dst, int64_t distance) {
    stops_distance_[{src, dst}] = distance;
}

void TransportCatalogue::AddBus(Bus bus) {
    buses_.push_back(bus);
    pointers_to_buses_[buses_.back().name] = &buses_.back();
}

std::optional<std::vector<std::string_view>> TransportCatalogue::GetBusesByStopName(string_view stop_name) {
    if (GetStopByName(stop_name) == nullptr) {
        return {};
    } else {
        vector<string_view> res;
        for_each(pointers_to_buses_.begin(), pointers_to_buses_.end(),
                 [stop_name, &res](const auto value) {
                for(const auto stop : value.second->stops) {
                    if ( stop->name == stop_name) {
                        res.push_back(value.second->name);
                        break;
                    }
                }
        });
        return res;
    }
}

const TransportCatalogue::Stop* TransportCatalogue::GetStopByName(std::string_view stop_name) {
    if (pointers_to_stops_.count(stop_name) != 0) {
        return pointers_to_stops_[stop_name];
    } else {
        return nullptr;
    }
}

const TransportCatalogue::Bus* TransportCatalogue::GetBusByName(std::string_view bus_name) {
    if (pointers_to_buses_.count(bus_name) != 0) {
        return pointers_to_buses_[bus_name];
    } else {
        return nullptr;
    }
}

int64_t TransportCatalogue::GetDistanceByStopPair(const Stop* src,const Stop* dst) {
    if (stops_distance_.count({src,dst}) != 0) {
        return stops_distance_[{src,dst}];
    } else {
        return stops_distance_[{dst,src}];
    }
}

TransportCatalogue::BusInfo TransportCatalogue::GetBusInfoByBus(const Bus* bus) {
    BusInfo res;
    res.route_length = 0;
    res.real_route_length = 0;
    unordered_set<const TransportCatalogue::Stop*> unique_stops(bus->stops.begin(), bus->stops.end());
    res.stops_number = bus->stops.size();
    res.unique_stop_number = unique_stops.size();
    for(int i = 0; i < bus->stops.size() - 1; ++i) {
        res.route_length += ComputeDistance(bus->stops[i]->coordinates,
                                         bus->stops[i + 1]->coordinates);
        res.real_route_length += GetDistanceByStopPair(bus->stops[i], bus->stops[i + 1]);
        
    }
    return res;
}

}
