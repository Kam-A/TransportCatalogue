#include "json_reader.h"
#include "request_handler.h"

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
    Bus res;
    res.name = bus_dict.at("name").AsString();
    for (const std::string_view bus_stop : GetAllStops(bus_dict)) {
        res.stops.push_back(catalogue.GetStopByName(bus_stop));
    }
    res.is_roundtrip = bus_dict.at("is_roundtrip").AsBool();
    return res;
}

void SetRealDistanceForStopFromRequest(TransportCatalogue& catalogue, const json::Dict& stop_dict) {
    std::string stop_name = stop_dict.at("name").AsString();
    const Stop* src_stop = catalogue.GetStopByName(stop_name);
    for (const auto& [stop, distance] : stop_dict.at("road_distances").AsMap()) {
        const Stop* dst_stop = catalogue.GetStopByName(stop);
        catalogue.SetDistanceBetweenStops(src_stop, dst_stop, distance.AsInt());
    }
}

json::Dict GetBusDictFromBusInfo(const std::optional<BusInfo>& bus_info, int request_id) {
    if (bus_info) {
        return json::Dict{
            {"curvature"s, bus_info.value().real_route_length / bus_info.value().route_length},
            { "request_id"s, request_id},
            {"route_length"s, bus_info.value().real_route_length},
            {"stop_count"s, bus_info.value().stops_number},
            {"unique_stop_count"s, bus_info.value().unique_stop_number}
        };
    } else {
        return json::Dict{
            {"request_id"s, request_id},
            {"error_message"s, "not found"s}
        };
    }
}

json::Dict GetBusesDictFromBuses(const std::optional<std::vector<std::string_view>>& buses, int request_id) {
    if (buses) {
        json::Array bus_array;
        for (const auto& bus_name : buses.value()) {
            bus_array.push_back(std::string(bus_name));
        }
        return json::Dict{
            {"buses"s, bus_array},
            { "request_id"s, request_id}
        };
    } else {
        return json::Dict{
            {"request_id"s, request_id},
            {"error_message"s, "not found"s}
        };
    }
}
json::Dict GetMapAsDict(request_handler::RequestHandler& request_handler, int request_id) {
    std::ostringstream out;
    svg::Document doc =  request_handler.RenderMap();
    doc.Render(out);
    return json::Dict{
        {"map"s, out.str()},
        { "request_id"s, request_id}
    };;
    
}

void StatRequestProcess(const json::Node& request_body, std::ostream& output, request_handler::RequestHandler& request_handler) {
    json::Array result;
    for (const auto& stat_request : request_body.AsArray()) {
        json::Dict request = stat_request.AsMap();
        if (request["type"].AsString() == "Bus"s) {
            result.push_back(GetBusDictFromBusInfo(request_handler.GetBusInfo(request["name"].AsString()),  request["id"s].AsInt()));
        } else if (request["type"s].AsString() == "Stop"s) {
            result.push_back(GetBusesDictFromBuses(request_handler.GetBusesByStop(request["name"].AsString()),  request["id"s].AsInt()));
        } else if (request["type"s].AsString() == "Map"s) {
            result.push_back(GetMapAsDict(request_handler, request["id"s].AsInt()));
        }
    }
    json::Print(json::Document{result}, output);
}

void BaseRequestProcess(TransportCatalogue& catalogue, const json::Node& request_body) {
    Requests requests;
    for (const auto& domain_request : request_body.AsArray()) {
        json::Dict request = domain_request.AsMap();
        if (request["type"].AsString() == "Bus"s) {
            requests.buses.push_back(move(request));
        } else if (request["type"].AsString() == "Stop"s) {
            catalogue.AddStop(GetStopFromDict(request));
            requests.stops.push_back(move(request));
        }
    }
    for(const auto& add_stop_request : requests.stops) {
        SetRealDistanceForStopFromRequest(catalogue, add_stop_request.AsMap());
    }
    for(const auto& add_bus_request : requests.buses) {
        catalogue.AddBus(GetBusFromDict(catalogue, add_bus_request.AsMap()));
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
    json::Dict setting_dict = request_body.AsMap();
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

void RequestProcess(TransportCatalogue& catalogue, std::istream& input, std::ostream& output, renderer::MapRenderer& renderer, request_handler::RequestHandler& request_handler) {
    json::Document requests = json::Load(input);
    for (const auto& [request_type, request_body] : requests.GetRoot().AsMap()) {
        if (request_type == "base_requests"s) {
            BaseRequestProcess(catalogue, request_body);
        } else if (request_type == "stat_requests"s) {
            StatRequestProcess(request_body, output, request_handler);
        } else if (request_type == "render_settings"s){
            SetRenderSettings(renderer, request_body);
        }
    }
}

}
}
