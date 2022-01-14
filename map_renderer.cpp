#include "map_renderer.h"

#include <set>

namespace transport_catalogue {
namespace renderer {
using namespace std::literals;

bool IsZero(double value) {
    return std::abs(value) < 1e-6;
}

svg::Polyline MapRenderer::RenderBusRoute(const Bus* bus_ptr,  SphereProjector& projector, int color_number) const {
    svg::Polyline route;
    for (const auto& stop_ptr : bus_ptr->stops) {
        route.AddPoint(projector(stop_ptr->coordinates));
    }
    if (!bus_ptr->is_roundtrip)  {
        for(int i = static_cast<int>(bus_ptr->stops.size() - 2); i >= 0; --i) {
            route.AddPoint(projector(bus_ptr->stops[i]->coordinates));
        }
    }
    route.SetFillColor("none");
    route.SetStrokeColor(render_settings_.color_palette[color_number % render_settings_.color_palette.size()]);
    route.SetStrokeWidth(render_settings_.line_width);
    route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return route;
}

svg::Text MapRenderer::RenderBusNameBase(const Bus* bus_ptr,  svg::Point position) const {
    svg::Text route_name_base;
    
    route_name_base.SetPosition(position);
    route_name_base.SetOffset(render_settings_.bus_label_offset);
    route_name_base.SetFontSize(render_settings_.bus_label_font_size);
    route_name_base.SetFontFamily("Verdana"s);
    route_name_base.SetFontWeight("bold"s);
    route_name_base.SetData(bus_ptr->name);
    
    route_name_base.SetFillColor(render_settings_.underlayer_color);
    route_name_base.SetStrokeColor(render_settings_.underlayer_color);
    route_name_base.SetStrokeWidth(render_settings_.underlayer_width);
    route_name_base.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route_name_base.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    
    return route_name_base;
}

svg::Text MapRenderer::RenderBusName(const Bus* bus_ptr,  svg::Point position, int color_number) const {
    svg::Text route_name;
    
    route_name.SetPosition(position);
    route_name.SetOffset(render_settings_.bus_label_offset);
    route_name.SetFontSize(render_settings_.bus_label_font_size);
    route_name.SetFontFamily("Verdana"s);
    route_name.SetFontWeight("bold"s);
    route_name.SetData(bus_ptr->name);
    
    route_name.SetFillColor(render_settings_.color_palette[color_number % render_settings_.color_palette.size()]);
    
    return route_name;
}

svg::Circle MapRenderer::RenderStopSymbol(svg::Point position) const {
    svg::Circle stop_symbol;
    stop_symbol.SetCenter(position);
    stop_symbol.SetRadius(render_settings_.stop_radius);
    stop_symbol.SetFillColor("white"s);
    return stop_symbol;
}

svg::Text MapRenderer::RenderStopNameBase(const Stop* stop_ptr, svg::Point position) const {
    svg::Text stop_name_base;
    stop_name_base.SetPosition(position);
    stop_name_base.SetOffset(render_settings_.stop_label_offset);
    stop_name_base.SetFontSize(render_settings_.stop_label_font_size);
    stop_name_base.SetFontFamily("Verdana"s);
    stop_name_base.SetData(stop_ptr->name);
    
    stop_name_base.SetFillColor(render_settings_.underlayer_color);
    stop_name_base.SetStrokeColor(render_settings_.underlayer_color);
    stop_name_base.SetStrokeWidth(render_settings_.underlayer_width);
    stop_name_base.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    stop_name_base.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    
    return stop_name_base;
}

svg::Text MapRenderer::RenderStopName(const Stop* stop_ptr, svg::Point position) const {
    svg::Text stop_name;

    stop_name.SetPosition(position);
    stop_name.SetOffset(render_settings_.stop_label_offset);
    stop_name.SetFontSize(render_settings_.stop_label_font_size);
    stop_name.SetFontFamily("Verdana"s);
    stop_name.SetData(stop_ptr->name);
    
    
    stop_name.SetFillColor("black"s);
    
    return stop_name;
}

svg::Document MapRenderer::RenderBusRoutes(const std::map<std::string_view, const Bus*>& buses_dict) const {
    svg::Document svg_doc;
    auto comp = [] (const Stop* lhs, const Stop* rhs) {
                                             return lhs->name < rhs->name;
                     };
    std::set<const Stop*, decltype(comp)> stop_ptr_arr(comp);
    for (const auto& [bus_name, bus_ptr] : buses_dict) {
        stop_ptr_arr.insert(bus_ptr->stops.begin(), bus_ptr->stops.end());
    }
    SphereProjector projector(stop_ptr_arr.begin(),
                              stop_ptr_arr.end(),
                              render_settings_.width,
                              render_settings_.height,
                              render_settings_.padding);
    int route_count = 0;
    for (const auto& [bus_name, bus_ptr] : buses_dict) {
        svg_doc.Add(RenderBusRoute(bus_ptr, projector, route_count));
        ++route_count;
    }
    route_count = 0;
    for (const auto& [bus_name, bus_ptr] : buses_dict) {
        if (bus_ptr->stops.size() == 0) {
            break;
        }
        svg_doc.Add(RenderBusNameBase(bus_ptr, projector(bus_ptr->stops[0]->coordinates)));
        svg_doc.Add(RenderBusName(bus_ptr, projector(bus_ptr->stops[0]->coordinates), route_count));
        if (bus_ptr->stops[0] != bus_ptr->stops[bus_ptr->stops.size() - 1]) {
            svg_doc.Add(RenderBusNameBase(bus_ptr,
                                          projector(bus_ptr->stops[bus_ptr->stops.size() - 1]->coordinates)));
            svg_doc.Add(RenderBusName(bus_ptr,
                                      projector(bus_ptr->stops[bus_ptr->stops.size() - 1]->coordinates), route_count));
        }
        ++route_count;
    }
    
    for (const Stop* stop_ptr : stop_ptr_arr) {
        svg_doc.Add(RenderStopSymbol(projector(stop_ptr->coordinates)));
    }
    
    for (const Stop* stop_ptr : stop_ptr_arr) {
        svg_doc.Add(RenderStopNameBase(stop_ptr,  projector(stop_ptr->coordinates)));
        svg_doc.Add(RenderStopName(stop_ptr,  projector(stop_ptr->coordinates)));
        
    }
    
    return svg_doc;
}
}
}
