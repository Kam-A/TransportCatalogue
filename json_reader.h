#pragma once

#include <iostream>

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace transport_catalogue {
namespace json_reader{

struct Requests {
    std::vector<json::Node> buses;
    std::vector<json::Node> stops;
};

void RequestProcess(TransportCatalogue& catalogue, std::istream& input, std::ostream& output, renderer::MapRenderer& renderer, request_handler::RequestHandler& request_handler);
}
}
