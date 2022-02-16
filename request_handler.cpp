#include "request_handler.h"

namespace transport_catalogue {
namespace request_handler {

RequestHandler::RequestHandler(const TransportCatalogue& db,
                               const renderer::MapRenderer& renderer,
                            TransportRouter& router) : db_(db), renderer_(renderer), router_(router) {
    
}

std::optional<BusInfo> RequestHandler::GetBusInfo(const std::string_view& bus_name) const {
    const Bus* bus = db_.GetBusByName(bus_name);
    if (bus == nullptr) {
        return {};
    } else {
        return db_.GetBusInfoByBus(bus);
    }
}

std::optional<std::vector<std::string_view>>  RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    return db_.GetBusesByStopName(stop_name);
}

// here the main functional
std::optional<TransportRouter::RouteItems> RequestHandler::GetRouteByStops(const std::string_view start_stop, const std::string_view finish_stop) const {
    return router_.GetRouteByStops(db_.GetStopByName(start_stop), db_.GetStopByName(finish_stop));
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.RenderBusRoutes(db_.GetAllBuses());
}
}
}
