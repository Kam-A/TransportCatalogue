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

std::optional<std::vector<std::string_view>> TransportCatalogue::GetBusesByStopName(string_view stop_name) const {
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

const Stop* TransportCatalogue::GetStopByName(std::string_view stop_name) const {
    if (pointers_to_stops_.count(stop_name) != 0) {
        return pointers_to_stops_.at(stop_name);
    } else {
        return nullptr;
    }
}

const Bus* TransportCatalogue::GetBusByName(std::string_view bus_name) const {
    if (pointers_to_buses_.count(bus_name) != 0) {
        return pointers_to_buses_.at(bus_name);
    } else {
        return nullptr;
    }
}

int64_t TransportCatalogue::GetDistanceByStopPair(const Stop* src,const Stop* dst) const {
    if (stops_distance_.count({src,dst}) != 0) {
        return stops_distance_.at({src,dst});
    } else {
        return stops_distance_.at({dst,src});
    }
}

BusInfo TransportCatalogue::GetBusInfoByBus(const Bus* bus) const {
    BusInfo res;
    res.route_length = 0;
    res.real_route_length = 0;
    vector<const Stop*> stops;
    stops.insert(stops.begin(), bus->stops.begin(),bus->stops.end());
    if (!bus->is_roundtrip)  {
        for(int i = static_cast<int>(stops.size() - 2); i >= 0; --i) {
            stops.push_back(stops[i]);
        }
    }
    unordered_set<const Stop*> unique_stops(stops.begin(), stops.end());
    res.stops_number = static_cast<int>(stops.size());
    res.unique_stop_number = static_cast<int>(unique_stops.size());
    for(int i = 0; i < static_cast<int>(stops.size()) - 1; ++i) {
        res.route_length += ComputeDistance(stops[i]->coordinates,
                                         stops[i + 1]->coordinates);
        res.real_route_length += GetDistanceByStopPair(stops[i], stops[i + 1]);
    
    }
    return res;
}

const std::map<std::string_view, const Bus*>& TransportCatalogue::GetAllBuses() const {
    return pointers_to_buses_;
}

const std::unordered_map<std::string_view, const Stop*>& TransportCatalogue::GetAllStops() const {
    return pointers_to_stops_;
}

const std::unordered_map<std::pair<const Stop*, const Stop*>, int64_t, detail::PairHasher<const Stop*>> TransportCatalogue::GetStopDistances() const {
    return stops_distance_;
}

size_t TransportCatalogue::GetNumberOfStops() const {
    return stops_.size();
}

size_t TransportCatalogue::GetNumberOfBuses() const {
    return buses_.size();
}

}
