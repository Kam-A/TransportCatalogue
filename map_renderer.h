#pragma once

#include <map>

#include "svg.h"
#include "geo.h"
#include "domain.h"
/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
namespace transport_catalogue {
namespace renderer {

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width,
                    double max_height, double padding)
        : padding_(padding) {
            if (points_begin == points_end) {
                return;
            }

            const auto [left_it, right_it]
                = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                      return lhs->coordinates.lng < rhs->coordinates.lng;
                  });
            min_lon_ = (*left_it)->coordinates.lng;
            const double max_lon = (*right_it)->coordinates.lng;

            const auto [bottom_it, top_it]
                = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                      return lhs->coordinates.lat < rhs->coordinates.lat;
                  });
            const double min_lat = (*bottom_it)->coordinates.lat;
            max_lat_ = (*top_it)->coordinates.lat;

            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            } else if (width_zoom) {
                zoom_coeff_ = *width_zoom;
            } else if (height_zoom) {
                zoom_coeff_ = *height_zoom;
            }
    }

    svg::Point operator()(detail::Coordinates coords) const {
        return {(coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_};
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct render_settings {
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    svg::Point bus_label_offset;
    int stop_label_font_size;
    svg::Point stop_label_offset;
    svg::Color underlayer_color;
    double underlayer_width;
    std::vector<svg::Color> color_palette;
};

class MapRenderer {
public:
    MapRenderer() {
        
    }
    void SetSettings(render_settings settings) {
        render_settings_ = settings;
    }
    svg::Document RenderBusRoutes(const std::map<std::string_view, const Bus*>& buses_dict) const;
private:
    void CreateBusNameText(svg::Text& route_name_base,
                                svg::Text& route_name,
                                svg::Point position,
                                const std::string& bus_name,
                                        int bus_count) const;
    svg::Polyline RenderBusRoute(const Bus* bus_ptr,  SphereProjector& projector, int color_number) const;
    svg::Text RenderBusNameBase(const Bus* bus_ptr,  svg::Point position) const;
    svg::Text RenderBusName(const Bus* bus_ptr,  svg::Point position, int color_number) const;
    svg::Circle RenderStopSymbol(svg::Point position) const;
    svg::Text RenderStopNameBase(const Stop* stop_ptr, svg::Point position) const;
    svg::Text RenderStopName(const Stop* stop_ptr, svg::Point position) const;
    render_settings render_settings_;
};

}
}
