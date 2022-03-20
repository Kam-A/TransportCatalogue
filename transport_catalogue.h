#pragma once

#include <deque>
#include <map>
#include <string>
#include <string_view>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "domain.h"
#include "geo.h"

namespace transport_catalogue {
namespace detail {

template<typename T>
struct PairHasher {
    size_t operator()(const std::pair<T, T> pair_to_hash) const {
        return hasher(pair_to_hash.first) ^ hasher(pair_to_hash.second);
    }
    std::hash<T> hasher;
};
    
}

class TransportCatalogue {
    
public:
    void AddStop(Stop stop);
    void SetDistanceBetweenStops(const Stop* src, const Stop* dst, int64_t distance);
    void AddBus(Bus bus);
    const Stop* GetStopByName(std::string_view stop_name) const;
    const Bus* GetBusByName(std::string_view bus_name) const;
    std::optional<std::vector<std::string_view>> GetBusesByStopName(std::string_view stop_name) const;
    int64_t GetDistanceByStopPair(const Stop* src, const Stop* dst) const;
    BusInfo GetBusInfoByBus(const Bus* bus) const;
    const std::map<std::string_view, const Bus*>& GetAllBuses() const;
    const std::unordered_map<std::string_view, const Stop*>& GetAllStops() const;
    const std::unordered_map<std::pair<const Stop*, const Stop*>, int64_t, detail::PairHasher<const Stop*>> GetStopDistances() const;
    size_t GetNumberOfStops() const;
    size_t GetNumberOfBuses() const;
private:
    
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::map<std::string_view, const Bus*> pointers_to_buses_;
    std::unordered_map<std::string_view, const Stop*> pointers_to_stops_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int64_t, detail::PairHasher<const Stop*>> stops_distance_;

};

}
