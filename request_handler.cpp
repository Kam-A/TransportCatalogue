#include "request_handler.h"

namespace transport_catalogue {
namespace request_handler {

RequestHandler::RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer) : db_(db), renderer_(renderer) {
    
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

svg::Document RequestHandler::RenderMap() const {
    return renderer_.RenderBusRoutes(db_.GetAllBuses());
}
}
}
