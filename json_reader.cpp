#include "json_reader.h"
#include "json_builder.h"
#include "request_handler.h"
#include "transport_router.h"

#include <map>
#include <string_view>
#include <vector>

namespace transport_catalogue {
namespace json_reader{

using namespace std::literals;

Stop GetStopFromDict(const json::Dict& stop_dict) {
    return {stop_dict.at("name").AsString(), stop_dict.at("latitude").AsDouble(), stop_dict.at("longitude").AsDouble()};
}

std::vector<std::string_view> GetAllStops(const json::Dict& bus_dict) {
    std::vector<std::string_view> result;
    for (const auto& stop : bus_dict.at("stops").AsArray()) {
        result.push_back(stop.AsString());
    }
    return result;
}

Bus GetBusFromDict(TransportCatalogue& catalogue, const json::Dict& bus_dict) {
    Bus bus;
    bus.name = bus_dict.at("name").AsString();
    for (const std::string_view bus_stop : GetAllStops(bus_dict)) {
        bus.stops.push_back(catalogue.GetStopByName(bus_stop));
    }
    bus.is_roundtrip = bus_dict.at("is_roundtrip").AsBool();
    return bus;
}

void SetRealDistanceForStopFromRequest(TransportCatalogue& catalogue, const json::Dict& stop_dict) {
    std::string stop_name = stop_dict.at("name").AsString();
    const Stop* src_stop = catalogue.GetStopByName(stop_name);
    for (const auto& [stop, distance] : stop_dict.at("road_distances").AsDict()) {
        const Stop* dst_stop = catalogue.GetStopByName(stop);
        catalogue.SetDistanceBetweenStops(src_stop, dst_stop, distance.AsInt());
    }
}

void GetBusDictFromBusInfo(json::Builder& builder, const std::optional<BusInfo>& bus_info, int request_id) {
    if (bus_info) {
        builder.StartDict()
                    .Key("curvature"s).Value(bus_info.value().real_route_length / bus_info.value().route_length)
                    .Key("request_id"s).Value(request_id)
                    .Key("route_length"s).Value(bus_info.value().real_route_length)
                    .Key("stop_count"s).Value(bus_info.value().stops_number)
                    .Key("unique_stop_count"s).Value( bus_info.value().unique_stop_number)
                .EndDict();
    } else {
        builder.StartDict()
                    .Key("request_id"s).Value(request_id)
                    .Key("error_message"s).Value("not found"s)
                .EndDict();
    }
}

void GetBusesDictFromBuses(json::Builder& builder, const std::optional<std::vector<std::string_view>>& buses, int request_id) {
    if (buses) {
        builder.StartDict();
        builder.Key("buses"s);
        builder.StartArray();
        for (const auto& bus_name : buses.value()) {
            builder.Value(std::string(bus_name));
        }
        builder.EndArray();
        builder.Key("request_id"s).Value(request_id);
        builder.EndDict();
    } else {
        builder.StartDict()
                    .Key("request_id"s).Value(request_id)
                    .Key("error_message"s).Value("not found"s)
                .EndDict();
    }
}

void GetMapAsDict(json::Builder& builder, request_handler::RequestHandler& request_handler, int request_id) {
    std::ostringstream out;
    svg::Document doc =  request_handler.RenderMap();
    doc.Render(out);
    builder.StartDict()
                .Key("map"s).Value(out.str())
                .Key("request_id"s).Value(request_id)
            .EndDict();
    
}
void InsertItemToResponse(json::Builder& builder,const TransportRouter::Item& item) {
    if (item.type == TransportRouter::ItemType::WAIT) {
        builder.StartDict()
                .Key("type").Value("Wait"s)
                .Key("stop_name").Value(std::string(item.name))
                .Key("time").Value(item.time)
            .EndDict();
    } else if (item.type == TransportRouter::ItemType::BUS) {
        builder.StartDict()
                .Key("type").Value("Bus"s)
                .Key("bus").Value(std::string(item.name))
                .Key("span_count").Value(item.span_count)
                .Key("time").Value(item.time)
            .EndDict();
    }
}
void GetItemMapFromItems(json::Builder& builder, const std::optional<TransportRouter::RouteItems>& items, int request_id) {
    if (items) {
        builder.StartDict()
            .Key("request_id").Value(request_id)
            .Key("total_time").Value(items.value().total_time)
            .Key("items")
            .StartArray();
        for (const auto& item : items.value().items) {
            InsertItemToResponse(builder, item);
        }
        builder.EndArray();
        builder.EndDict();
    } else {
        builder.StartDict()
                    .Key("request_id"s).Value(request_id)
                    .Key("error_message"s).Value("not found"s)
                .EndDict();
    }
}

void StatRequestProcess(const json::Node& request_body, std::ostream& output, request_handler::RequestHandler& request_handler) {
    json::Builder builder;
    builder.StartArray();
    for (const auto& stat_request : request_body.AsArray()) {
        json::Dict request = stat_request.AsDict();
        if (request["type"].AsString() == "Bus"s) {
            GetBusDictFromBusInfo(builder, request_handler.GetBusInfo(request["name"].AsString()),  request["id"s].AsInt());
        } else if (request["type"s].AsString() == "Stop"s) {
            GetBusesDictFromBuses(builder, request_handler.GetBusesByStop(request["name"].AsString()),  request["id"s].AsInt());
        } else if (request["type"s].AsString() == "Map"s) {
            GetMapAsDict(builder, request_handler, request["id"s].AsInt());
        } else if (request["type"s].AsString() == "Route"s) {
            GetItemMapFromItems(builder, request_handler.GetRouteByStops(request["from"].AsString(), request["to"].AsString()), request["id"s].AsInt());
        }
    }
    builder.EndArray();
    json::Print(json::Document{builder.Build()}, output);
}

void BaseRequestProcess(TransportCatalogue& catalogue, const json::Node& request_body) {
    Requests requests;
    for (const auto& domain_request : request_body.AsArray()) {
        json::Dict request = domain_request.AsDict();
        if (request["type"].AsString() == "Bus"s) {
            requests.buses.push_back(move(request));
        } else if (request["type"].AsString() == "Stop"s) {
            catalogue.AddStop(GetStopFromDict(request));
            requests.stops.push_back(move(request));
        }
    }
    for(const auto& add_stop_request : requests.stops) {
        SetRealDistanceForStopFromRequest(catalogue, add_stop_request.AsDict());
    }
    for(const auto& add_bus_request : requests.buses) {
        catalogue.AddBus(GetBusFromDict(catalogue, add_bus_request.AsDict()));
    }
}

svg::Color CreateColorByNode(const json::Node& color_node) {
    svg::Color color;
    if (color_node.IsString()) {
        color = color_node.AsString();
    } else if (color_node.AsArray().size() == 3) {
        color = svg::Rgb(color_node.AsArray()[0].AsDouble(),
                                    color_node.AsArray()[1].AsDouble(),
                                    color_node.AsArray()[2].AsDouble());
    } else if (color_node.AsArray().size() == 4) {
        color = svg::Rgba(color_node.AsArray()[0].AsDouble(),
                                     color_node.AsArray()[1].AsDouble(),
                                     color_node.AsArray()[2].AsDouble(),
                                     color_node.AsArray()[3].AsDouble());
    }
    return color;
}

void SetRenderSettings(renderer::MapRenderer& renderer, const json::Node& request_body) {
    json::Dict setting_dict = request_body.AsDict();
    std::vector<svg::Color> color_palette;
    for (const auto& color_node: setting_dict.at("color_palette").AsArray()) {
        color_palette.push_back(CreateColorByNode(color_node));
    }
    renderer::render_settings render_settings{
        setting_dict.at("width").AsDouble(),
        setting_dict.at("height").AsDouble(),
        setting_dict.at("padding").AsDouble(),
        setting_dict.at("line_width").AsDouble(),
        setting_dict.at("stop_radius").AsDouble(),
        setting_dict.at("bus_label_font_size").AsInt(),
        {setting_dict.at("bus_label_offset").AsArray()[0].AsDouble(), setting_dict.at("bus_label_offset").AsArray()[1].AsDouble()},
        setting_dict.at("stop_label_font_size").AsInt(),
        {setting_dict.at("stop_label_offset").AsArray()[0].AsDouble(), setting_dict.at("stop_label_offset").AsArray()[1].AsDouble()},
        CreateColorByNode(setting_dict.at("underlayer_color")),
        setting_dict.at("underlayer_width").AsDouble(),
        color_palette
    };
    renderer.SetSettings(render_settings);
}

void SetRoutingSettings(TransportRouter& router, const json::Node& request_body) {
    json::Dict setting_dict = request_body.AsDict();
    router.SetSettings(setting_dict.at("bus_wait_time").AsInt(),
                                       setting_dict.at("bus_velocity").AsDouble());
}

void RequestProcess(TransportCatalogue& catalogue,
                    std::istream& input,
                    std::ostream& output,
                    renderer::MapRenderer& renderer,
                    TransportRouter& router,
                    request_handler::RequestHandler& request_handler) {
    json::Document requests = json::Load(input);
    for (const auto& [request_type, request_body] : requests.GetRoot().AsDict()) {
        if (request_type == "base_requests"s) {
            BaseRequestProcess(catalogue, request_body);
        } else if (request_type == "stat_requests"s) {
            router.BuildAllRoutes();
            StatRequestProcess(request_body, output, request_handler);
        } else if (request_type == "render_settings"s){
            SetRenderSettings(renderer, request_body);
        } else if (request_type == "routing_settings"s){
            SetRoutingSettings(router, request_body);
        }
    }
}

}
}
