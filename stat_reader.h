#pragma once

#include <vector>
#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"

namespace transport_catalogue{
namespace detail{

std::vector<std::pair<RequestType,std::string>> ReadGetRequest();
void PrintBus(std::string_view req, const TransportCatalogue::Bus* bus);
void PrintStop(std::string_view req, std::optional<std::vector<std::string_view>> buses);

}
}
