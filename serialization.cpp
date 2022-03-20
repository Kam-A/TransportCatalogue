#include "serialization.h"
#include "transport_catalogue.pb.h"

#include <fstream>

namespace transport_catalogue {

void Serializer::SetSettings(std::string file_name) {
    file_name_ = std::move(file_name);
}

transport_catalogue_serialize::Stop CreateSerializeStop(const Stop* stop_ptr) {
    transport_catalogue_serialize::Stop stop;
    stop.set_name(stop_ptr->name);
    stop.mutable_coordinates()->set_lng(stop_ptr->coordinates.lng);
    stop.mutable_coordinates()->set_lat(stop_ptr->coordinates.lat);
    return stop;
}

transport_catalogue_serialize::Bus CreateSerializeBus(const Bus* bus_ptr) {
    transport_catalogue_serialize::Bus bus;
    bus.set_name(bus_ptr->name);
    bus.set_is_roundtrip(bus_ptr->is_roundtrip);
    for(const Stop* stop_ptr : bus_ptr->stops) {
        bus.add_stops(reinterpret_cast<uint64_t>(stop_ptr));
    }
    return bus;
}

transport_catalogue_serialize::StopDistance CreateSerializeDistance(const Stop* src_stop_ptr, const Stop* dst_stop_ptr, int64_t distance) {
    transport_catalogue_serialize::StopDistance stop_dist;
    stop_dist.set_src(reinterpret_cast<uint64_t>(src_stop_ptr));
    stop_dist.set_dst(reinterpret_cast<uint64_t>(dst_stop_ptr));
    stop_dist.set_distance(distance);
    return stop_dist;
}

transport_catalogue_serialize::Color CreateSerializeColor(svg::Color color) {
    transport_catalogue_serialize::Color serialized_color;
    if (std::holds_alternative<std::string>(color)) {
        serialized_color.set_string_value(std::get<std::string>(color));
    } else if (std::holds_alternative<svg::Rgb>(color)) {
        svg::Rgb rgb_color = std::get<svg::Rgb>(color);
        serialized_color.mutable_rgb_value()->set_r(rgb_color.red);
        serialized_color.mutable_rgb_value()->set_g(rgb_color.green);
        serialized_color.mutable_rgb_value()->set_b(rgb_color.blue);
    } else if (std::holds_alternative<svg::Rgba>(color)) {
        svg::Rgba rgb_color = std::get<svg::Rgba>(color);
        serialized_color.mutable_rgba_value()->set_r(rgb_color.red);
        serialized_color.mutable_rgba_value()->set_g(rgb_color.green);
        serialized_color.mutable_rgba_value()->set_b(rgb_color.blue);
        serialized_color.mutable_rgba_value()->set_opacity(rgb_color.opacity);
    }
    return serialized_color;
}

transport_catalogue_serialize::RenderSettings CreateSerializeRenderSettings(renderer::render_settings settings) {
    transport_catalogue_serialize::RenderSettings serialized_settings;
    serialized_settings.set_width(settings.width);
    serialized_settings.set_height(settings.height);
    serialized_settings.set_padding(settings.padding);
    serialized_settings.set_line_width(settings.line_width);
    serialized_settings.set_stop_radius(settings.stop_radius);
    serialized_settings.set_bus_label_font_size(settings.bus_label_font_size);
    serialized_settings.mutable_bus_label_offset()->set_x(settings.bus_label_offset.x);
    serialized_settings.mutable_bus_label_offset()->set_y(settings.bus_label_offset.y);
    serialized_settings.set_stop_label_font_size(settings.stop_label_font_size);
    serialized_settings.mutable_stop_label_offset()->set_x(settings.stop_label_offset.x);
    serialized_settings.mutable_stop_label_offset()->set_y(settings.stop_label_offset.y);
    *serialized_settings.mutable_underlayer_color() = CreateSerializeColor(settings.underlayer_color);
    serialized_settings.set_underlayer_width(settings.underlayer_width);
    for (const auto& color : settings.color_palette) {
        *serialized_settings.add_color_palette() = CreateSerializeColor(color);
    }
    return serialized_settings;
}

transport_catalogue_serialize::RouterSettings CreateSerializeRouterSettings(TransportRouter::RouterSettings settings) {
    transport_catalogue_serialize::RouterSettings serialized_settings;
    serialized_settings.set_time(settings.time);
    serialized_settings.set_velocity(settings.velocity);
    return serialized_settings;
}
    
Stop CreateStopFromSerialized(const transport_catalogue_serialize::Stop& stop) {
    return {stop.name(), {stop.coordinates().lat(), stop.coordinates().lng()}};
}

Bus CreateBusFromSerialized(const TransportCatalogue& db, transport_catalogue_serialize::TransportCatalogue& catalog, const transport_catalogue_serialize::Bus& bus) {
    Bus bus_result;
    bus_result.name = bus.name();
    bus_result.is_roundtrip = bus.is_roundtrip();
    for (const auto stops_id : bus.stops()) {
        bus_result.stops.push_back(db.GetStopByName((*catalog.mutable_stops())[stops_id].name()));
    }
    return bus_result;
    
}

const Stop* GetStopPtrFromId(const TransportCatalogue& db, transport_catalogue_serialize::TransportCatalogue& catalog, uint64_t stops_id) {
    return db.GetStopByName((*catalog.mutable_stops())[stops_id].name());
}

svg::Color CreateColorFromSerialized(transport_catalogue_serialize::Color color) {
    if (color.has_rgb_value()) {
        svg::Rgb rgb_color;
        rgb_color.red = color.rgb_value().r();
        rgb_color.green = color.rgb_value().g();
        rgb_color.blue = color.rgb_value().b();
        return rgb_color;
    } else if (color.has_rgba_value()) {
        svg::Rgba rgba_color;
        rgba_color.red = color.rgba_value().r();
        rgba_color.green = color.rgba_value().g();
        rgba_color.blue = color.rgba_value().b();
        rgba_color.opacity = color.rgba_value().opacity();
        return rgba_color;
    } else {
        std::string string_color = color.string_value();
        if (string_color.size() != 0) {
            return string_color;
        } else {
            return {};
        }
    }
}

renderer::render_settings CreateRenderSettings(const transport_catalogue_serialize::RenderSettings& settings) {
    renderer::render_settings unserialised_settings;
    unserialised_settings.width = settings.width();
    unserialised_settings.height = settings.height();
    unserialised_settings.padding = settings.padding();
    unserialised_settings.line_width = settings.line_width();
    unserialised_settings.stop_radius = settings.stop_radius();
    unserialised_settings.bus_label_font_size = settings.bus_label_font_size();
    unserialised_settings.bus_label_offset = {settings.bus_label_offset().x(),settings.bus_label_offset().y()};
    unserialised_settings.stop_label_font_size = settings.stop_label_font_size();
    unserialised_settings.stop_label_offset = {settings.stop_label_offset().x(),settings.stop_label_offset().y()};
    unserialised_settings.underlayer_color = CreateColorFromSerialized(settings.underlayer_color());
    unserialised_settings.underlayer_width = settings.underlayer_width();
    for(const auto& color : settings.color_palette()) {
        unserialised_settings.color_palette.push_back(CreateColorFromSerialized(color));
    }
    return unserialised_settings;
}

TransportRouter::RouterSettings CreateRouterSettings(const transport_catalogue_serialize::RouterSettings& settings) {
    return {settings.time(), settings.velocity()};
}

void Serializer::SerializeBaseToFile() {
    std::ofstream output(file_name_, std::ios::binary);
    transport_catalogue_serialize::TransportCatalogue serialized_catalogue;
    for (const auto& [name, stop_ptr] : db_.GetAllStops()) {
        (*serialized_catalogue.mutable_stops())[reinterpret_cast<uint64_t>(stop_ptr)] = CreateSerializeStop(stop_ptr);
    }
    for (const auto& [name, bus_ptr] : db_.GetAllBuses()) {
        *serialized_catalogue.add_buses() = CreateSerializeBus(bus_ptr);
    }
    for(const auto& [stop_ptrs, distance] : db_.GetStopDistances()) {
        *serialized_catalogue.add_distances() = CreateSerializeDistance(stop_ptrs.first, stop_ptrs.second, distance);
    }
    *serialized_catalogue.mutable_render_settings() = CreateSerializeRenderSettings(renderer_.GetSettings());
    *serialized_catalogue.mutable_router_settings() = CreateSerializeRouterSettings(router_.GetSettings());
    serialized_catalogue.SerializeToOstream(&output);
}

void Serializer::DeserializeBaseFromFile() {
    std::ifstream input(file_name_, std::ios::binary);
    // extract stops
    transport_catalogue_serialize::TransportCatalogue serialized_catalogue;
    serialized_catalogue.ParseFromIstream(&input);
    for(const auto& [stop_ptr, stop] : serialized_catalogue.stops()) {
        db_.AddStop(CreateStopFromSerialized(stop));
    }
    // extract buses
    for (const auto& bus : serialized_catalogue.buses()) {
        db_.AddBus(CreateBusFromSerialized(db_, serialized_catalogue, bus));
    }
    // set distances
    for (const auto& distance_info : serialized_catalogue.distances()) {
        db_.SetDistanceBetweenStops(GetStopPtrFromId(db_, serialized_catalogue, distance_info.src()),
                                    GetStopPtrFromId(db_, serialized_catalogue, distance_info.dst()),
                                    distance_info.distance());
    }
    renderer_.SetSettings(CreateRenderSettings(serialized_catalogue.render_settings()));
    router_.SetSettings(CreateRouterSettings(serialized_catalogue.router_settings()));
}

}
