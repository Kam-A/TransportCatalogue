#pragma once

#include "transport_catalogue.h"
#include "router.h"

#include <memory>
#include <string>
#include <string_view>
#include <set>
#include <unordered_set>
#include <vector>

namespace transport_catalogue {

class TransportRouter {
public:
    enum class ItemType {
        BUS,
        WAIT
    };
    
    struct Item {
        ItemType type;
        std::string_view name;
        double time;
        int span_count;
    };
    
    struct RouteItems {
        double total_time;
        std::vector<Item> items;
    };
    
    TransportRouter(const TransportCatalogue& catalogue) : catalogue_(catalogue) {
    }
    void SetSettings(int time, double velocity);
    void BuildAllRoutes();
    std::optional<RouteItems>  GetRouteByStops(const Stop* start_stop, const Stop* finish_stop) const;
private:
    int bus_wait_time_;
    double bus_velocity_;
    const TransportCatalogue& catalogue_;
    std::unique_ptr<graph::DirectedWeightedGraph<double>> route_graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::map<const Stop*, std::pair<int, int>> stop_to_vertex_pair_;
    std::map<graph::EdgeId, Item> edge_to_item_;
    void AddRouteToGraph(const std::string_view bus_name, const Bus* bus_ptr);
    void InitStopToVertexAndLinkWithEdge();
    graph::VertexId GetStartWaitVertexByStop(const Stop* stop_ptr) const;
    graph::VertexId GetStartBusVertexByStop(const Stop* stop_ptr) const;
    void AddEdgeToItem(graph::VertexId start_vertex, graph::VertexId stop_vertex, Item item);
    void AddBusEdge(const Stop* start_stop, const Stop* finish_stop, std::string_view bus_name, int span, double distance);
};
}
