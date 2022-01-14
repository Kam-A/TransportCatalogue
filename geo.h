#pragma once

#include <cmath>

namespace transport_catalogue {
namespace detail {

struct Coordinates {
    double lat;
    double lng;
};

double ComputeDistance(Coordinates from, Coordinates to);

}
}
