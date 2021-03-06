#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "transport_catalogue.h"

namespace transport_catalogue {
namespace detail {

std::string ReadLine(std::istream& input);
int ReadLineWithNumber(std::istream& input);

}

namespace input_reader {

struct Requests {
    std::vector<std::string> buses;
    std::vector<std::string> stops;
};

enum class RequestType {
    BUS,
    STOP,
    UNKNOWN
};

void RequestProcess(TransportCatalogue& catalogue, std::istream& input);
void SetRealDistanceForStopFromRequest(TransportCatalogue& catalogue, std::string_view request);
TransportCatalogue::Bus GetBusFromRequest(TransportCatalogue& catalogue, std::string_view request);
TransportCatalogue::Stop GetStopFromRequest(std::string_view request);
RequestType GetRequestType(std::string_view request);
std::string GetNameFromRequest(std::string_view& request);
std::vector<std::string_view> GetAllStops(std::string_view request);
std::vector<std::string_view> GetStopByDelimiter(std::string_view request, char delimiter);

}
}
