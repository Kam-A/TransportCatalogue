#pragma once

#include <string>
#include <vector>

#include "geo.h"

namespace transport_catalogue {

struct Stop {
    std::string name;
    transport_catalogue::detail::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
};

struct BusInfo {
    int stops_number;
    double route_length;
    double real_route_length;
    int unique_stop_number;
};

}
