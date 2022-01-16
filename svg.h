#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <vector>
#include <variant>

namespace svg {


struct Rgb {
    Rgb(uint8_t red, uint8_t green, uint8_t blue)
    : red(red)
    , green(green)
    , blue(blue) {
    }
    Rgb() {
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba : public Rgb {
    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
    : Rgb(red, green, blue)
    , opacity(opacity){
    }
    Rgba() {
    }
    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

inline const Color NoneColor{};

struct ColorPrinter {
    std::ostream& out;
    
    void operator()(std::monostate) const;
    void operator()(std::string color) const;
    void operator()(Rgb color) const;
    void operator()(Rgba color) const;
    
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

std::ostream& operator<<(std::ostream& out, StrokeLineCap linecap);

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream& out, StrokeLineJoin linejoin);

std::ostream& operator<<(std::ostream& out, const Color color);

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        width_ = width;
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        linecap_ = line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        linejoin_ = line_join;
        return AsOwner();
    }
protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;
        if (fill_color_) {
            out << " fill=\""s << *fill_color_ << "\""s;
        }
        if (stroke_color_) {
            out << " stroke=\""s << *stroke_color_ << "\""s;
        }
        if (width_) {
            out << " stroke-width=\""s << *width_ << "\""s;
        }
        if (linecap_) {
            out << " stroke-linecap=\""s << *linecap_ << "\""s;
        }
        if (linejoin_) {
            out << " stroke-linejoin=\""s << *linejoin_ << "\""s;
        }
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> linecap_;
    std::optional<StrokeLineJoin> linejoin_;
    
};


struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }
protected:
    ~ObjectContainer() = default;
private:
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
};

class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void Draw(ObjectContainer& container) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);
    
private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);
    
private:
    void RenderObject(const RenderContext& context) const override;
    Point position_;
    Point offset_;
    uint32_t font_size_ = 1;
    std::optional<std::string> font_family_;
    std::optional<std::string> font_weight_;
    std::string data_;
};

class Document : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override;
    void Render(std::ostream& out) const;
private:
    std::vector<std::unique_ptr<Object>> objects_ptr_;
};

}  // namespace svg
