#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream& out, StrokeLineCap linecap) {
    if (linecap == StrokeLineCap::BUTT) {
        out << "butt"s;
    } else if (linecap == StrokeLineCap::ROUND) {
        out << "round"s;
    } else {
        out << "square"s;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, StrokeLineJoin linejoin) {
    if (linejoin == StrokeLineJoin::ARCS) {
        out << "arcs"s;
    } else if (linejoin == StrokeLineJoin::BEVEL) {
        out << "bevel"s;
    } else if (linejoin == StrokeLineJoin::MITER) {
        out << "miter"s;
    } else if (linejoin == StrokeLineJoin::MITER_CLIP) {
        out << "miter-clip"s;
    } else {
        out << "round"s;
    }
    return out;
}

void ColorPrinter::operator()(std::monostate) const {
    out << "none"s;
}

void ColorPrinter::operator()(std::string color) const {
    out << color;
}

void ColorPrinter::operator()(Rgb color) const {
    out << "rgb("s << std::to_string(color.red) << ","s
    << std::to_string(color.green) << ","s
    << std::to_string(color.blue) << ")"s;
}

void ColorPrinter::operator()(Rgba color) const {
    out << "rgba("s << std::to_string(color.red) << ","s
    << std::to_string(color.green) << ","s
    << std::to_string(color.blue) << ","s
    << color.opacity << ")"s;
}

std::ostream& operator<<(std::ostream& out, const Color color) {
    std::visit(ColorPrinter{out}, color);
    return out;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""s << center_.x << "\" cy=\""s << center_.y << "\" "s;
    out << "r=\""s << radius_ << "\" "s;
    RenderAttrs(context.out);
    out << "/>"s;
}

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

// <polyline points="0,100 50,25 50,75 100,0" />
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""s;
    int num_points  = static_cast<int>(points_.size());
    for (int i = 0; i < num_points; ++i) {
        if (i != num_points - 1) {
            out << points_[i].x << ","s << points_[i].y << " "s;
        } else {
            out << points_[i].x << ","s << points_[i].y;
        }
    }
    out << "\""s;
    RenderAttrs(context.out);
    out << " />"s;
}

Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}

// <text x="20" y="35" class="small">My</text>
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text "s;
    out << "x=\""s << position_.x << "\" y=\""s << position_.y << "\" "s;
    out << "dx=\""s << offset_.x << "\" dy=\""s << offset_.y << "\" "s;
    out << "font-size=\""s << font_size_ << "\" "s;
    if (font_family_)
        out << "font-family=\""s << *font_family_ << "\" "s;
    if (font_weight_)
        out << "font-weight=\""s << *font_weight_ << "\""s;
    RenderAttrs(context.out);
    out << ">"sv;
    for (const auto& letter : data_) {
        if (letter == '"') {
            out << "&quot;"s;
        } else if (letter == '<') {
            out << "&lt;"s;
        } else if (letter == '>') {
            out << "&gt;"s;
        } else if (letter == '\'') {
            out << "&apos;"s;
        } else if (letter == '&') {
            out << "&amp;"s;
        } else {
            out << letter;
        }
    }
    out << "</text>"s;
    
}

// Добавляет в svg-документ объект-наследник svg::Object
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_ptr_.emplace_back(std::move(obj));
}

// Выводит в ostream svg-представление документа
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"s << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"s << std::endl;
    for (const auto& object : objects_ptr_) {
        object->Render(RenderContext(out));
    }
    out << "</svg>"s;
}

}  // namespace svg
