
#include "transport_router.h"

#include <memory>
#include <numeric>

namespace transport_catalogue {

void TransportRouter::SetSettings(int time, double velocity) {
    bus_wait_time_ = time;
    bus_velocity_ = velocity * 1000 / 60;
}

graph::VertexId TransportRouter::GetStartWaitVertexByStop(const Stop* stop_ptr) const {
    return stop_to_vertex_pair_.at(stop_ptr).first;
}

graph::VertexId TransportRouter::GetStartBusVertexByStop(const Stop* stop_ptr) const {
    return stop_to_vertex_pair_.at(stop_ptr).second;
}

void TransportRouter::AddEdgeToItem(graph::VertexId start_vertex, graph::VertexId stop_vertex, Item item) {
    edge_to_item_[route_graph_->AddEdge({start_vertex, stop_vertex, item.time})] = item;
}

void TransportRouter::AddBusEdge(const Stop* start_stop, const Stop* finish_stop, std::string_view bus_name, int span, double distance) {
    Item item;
    item.type = ItemType::BUS;
    item.name = bus_name;
    item.time = distance / bus_velocity_;
    item.span_count = span;
    AddEdgeToItem(GetStartBusVertexByStop(start_stop),
                  GetStartWaitVertexByStop(finish_stop),
                  item);
}

void TransportRouter::AddRouteToGraph(const std::string_view bus_name, const Bus* bus_ptr) {
    for (int i = 0; i < bus_ptr->stops.size() - 1; ++i) {
        double forward_distance = 0;
        double backward_distance = 0;
        for (int j = i; j < bus_ptr->stops.size() - 1; ++j) {
            forward_distance += catalogue_.GetDistanceByStopPair(bus_ptr->stops[j], bus_ptr->stops[j + 1]);
            AddBusEdge(bus_ptr->stops[i], bus_ptr->stops[j + 1], bus_name, j - i + 1, forward_distance);
            if (!bus_ptr->is_roundtrip){
                backward_distance += catalogue_.GetDistanceByStopPair(bus_ptr->stops[j + 1], bus_ptr->stops[j]);
                AddBusEdge(bus_ptr->stops[j + 1], bus_ptr->stops[i], bus_name, j - i + 1, backward_distance);
            }
        }
    }
}

void TransportRouter::InitStopToVertexAndLinkWithEdge() {
    graph::VertexId vertex_id = 0;
    for (const auto& [stop_name, stop_ptr] : catalogue_.GetAllStops()) {
        stop_to_vertex_pair_[stop_ptr] = {vertex_id, vertex_id + 1};
        edge_to_item_[route_graph_->AddEdge({vertex_id, vertex_id + 1, static_cast<double>(bus_wait_time_)})] = {ItemType::WAIT, stop_name, static_cast<double>(bus_wait_time_), 1};
        vertex_id += 2;
    }
}

void TransportRouter::BuildAllRoutes() {
    route_graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(catalogue_.GetNumberOfStops() * 2);
    InitStopToVertexAndLinkWithEdge();
    for(const auto [bus_name, bus_ptr] : catalogue_.GetAllBuses()) {
        AddRouteToGraph(bus_name, bus_ptr);
    }
    router_ = std::make_unique<graph::Router<double>>(*route_graph_);
}

std::optional<TransportRouter::RouteItems> TransportRouter::GetRouteByStops(const Stop* start_stop, const Stop* finish_stop) const {
    RouteItems items_info;
    std::optional<graph::Router<double>::RouteInfo> router_info = router_->BuildRoute(GetStartWaitVertexByStop(start_stop), GetStartWaitVertexByStop(finish_stop));
    if (router_info) {
        items_info.total_time = router_info.value().weight;
        for (const auto& edge : router_info.value().edges) {
            items_info.items.push_back(edge_to_item_.at(edge));
        }
        return items_info;
    } else {
        return {};
    }
}
}
