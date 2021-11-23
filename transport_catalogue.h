#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include "geo.h"
#include <vector>

namespace transport_catalogue{

class TransportCatalogue {
public:
    struct Stop {
        std::string name;
        detail::Coordinates coord;
    };
    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
        double route_len;
        double real_route_len;
    };
    struct PairStopsHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*> stops_pair) const {
            return hasher(stops_pair.first) ^ hasher(stops_pair.second);
        }
        std::hash<const void*> hasher;
    };
    void AddStop(std::string_view request);
    void AddDistBwStops(std::string_view request);
    void AddBus(std::string_view request);
    const Stop* GetStopByName(std::string_view stop_name);
    const Bus* GetBusByName(std::string_view bus_name);
    std::optional<std::vector<std::string_view>> GetBusesByStop(std::string_view text);
private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::map<std::string_view, const Bus*> pointers_to_buses_;
    std::unordered_map<std::string_view, const Stop*> pointers_to_stops_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int64_t, PairStopsHasher> dst_bw_stops_;
    std::string GetNameFromReq(std::string_view& text);
    Stop GetStopInfoFromReq(std::string_view text);
    Bus GetBusInfoFromReq(std::string_view text);
    int64_t GetDistByStopPair(const Stop* src, const Stop* dst);
    std::vector<std::string_view> GetAllStops(std::string_view text);
    std::vector<std::string_view> GetStopByDelim(std::string_view str, char sym);
};

}
