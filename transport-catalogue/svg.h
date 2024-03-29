#pragma once

#include <cstdint>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

namespace svg
{
    using Color = std::string;

    inline const Color NoneColor{ "none" };

    enum class StrokeLineCap
    {
        BUTT,
        ROUND,
        SQUARE
    };

    enum class StrokeLineJoin
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND
    };

    inline std::ostream& operator<<(std::ostream& out, const StrokeLineCap& stroke_line_cap)
    {
        switch (stroke_line_cap)
        {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
        }
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& stroke_line_join)
    {
        switch (stroke_line_join)
        {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        }
        return out;
    }

// +++++++++++++++++++++++++++++ class PathProps begin +++++++++++++++++++++++++++++ //
    template<typename Owner>
    class PathProps
    {
    public:
        Owner& SetFillColor(Color fill_color)
        {
            fill_color_ = std::move(fill_color);
            return AsOwner();
        }

        Owner& SetStrokeColor(Color stroke_color)
        {
            stroke_color_ = stroke_color;
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width)
        {
            width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap)
        {
            line_cap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            line_join_ = line_join;
            return AsOwner();
        }

    private:
        Owner& AsOwner()
        {
            // static_cast ��������� ����������� *this � Owner&,
            // ���� ����� Owner � ��������� PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_ = std::nullopt;
        std::optional<Color> stroke_color_ = std::nullopt;
        std::optional<double> width_ = std::nullopt;
        std::optional<StrokeLineCap> line_cap_ = std::nullopt;
        std::optional<StrokeLineJoin> line_join_ = std::nullopt;


    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const
        {
            using namespace std::literals;

            if (fill_color_)
            {
                out << "fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_)
            {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (width_)
            {
                out << " stroke-width=\""sv << *width_ << "\""sv;
            }
            if (line_cap_)
            {
                out << " stroke-linecap=\"" << *line_cap_ << "\""sv;
            }
            if (line_join_)
            {
                out << " stroke-linejoin=\"" << *line_join_ << "\""sv;
            }
        }
    };
// _____________________________ class PathProps end _____________________________ //


    struct Point
    {
        Point() = default;
        Point(double x, double y) : x(x), y(y)
        {
        }
        double x = 0.0;
        double y = 0.0;
    };

    /*
     * ��������������� ���������, �������� �������� ��� ������ SVG-��������� � ���������.
     * ������ ������ �� ����� ������, ������� �������� � ��� ������� ��� ������ ��������
     */
    struct RenderContext
    {
        RenderContext(std::ostream& out) : out(out)
        {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0) : out(out), indent_step(indent_step), indent(indent)
        {
        }

        RenderContext Indented() const
        {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const
        {
            for (int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };


// +++++++++++++++++++++++++++++ class Object begin +++++++++++++++++++++++++++++ //
    /*
     * ����������� ������� ����� Object ������ ��� ���������������� ��������
     * ���������� ����� SVG-���������
     * ��������� ������� "��������� �����" ��� ������ ����������� ����
     */
    class Object
    {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };
// _____________________________ class Object end _____________________________ //


// +++++++++++++++++++++++++++++ class Circle begin +++++++++++++++++++++++++++++ //
    /*
     * ����� Circle ���������� ������� <circle> ��� ����������� �����
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle : public Object, public PathProps<Circle>
    {
    public:
        Circle() = default;
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_{ 0.0, 0.0 };
        double radius_ = 1.0;
    };
// _____________________________ class Circle end _____________________________ //


// +++++++++++++++++++++++++++++ class Polyline begin +++++++++++++++++++++++++++++ //
    /*
     * ����� Polyline ���������� ������� <polyline> ��� ����������� ������� �����
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline : public Object, public PathProps<Polyline>
    {
    public:
        Polyline() = default;
        // ��������� ��������� ������� � ������� �����
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;

        //std::list<Point> polyline_;
        std::string polyline_ = "";
    };
// _____________________________ class Polyline end _____________________________ //


// +++++++++++++++++++++++++++++ class Text begin +++++++++++++++++++++++++++++ //
    /*
     * ����� Text ���������� ������� <text> ��� ����������� ������
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text : public Object, public PathProps<Text>
    {
    public:
        Text() = default;
        // ����� ���������� ������� ����� (�������� x � y)
        Text& SetPosition(Point pos);

        // ����� �������� ������������ ������� ����� (�������� dx, dy)
        Text& SetOffset(Point offset);

        // ����� ������� ������ (������� font-size)
        Text& SetFontSize(uint32_t size);

        // ����� �������� ������ (������� font-family)
        Text& SetFontFamily(std::string name);

        // ����� ������� ������ (������� font-weight)
        Text& SetFontWeight(std::string font_weight);

        // ����� ��������� ���������� ������� (������������ ������ ���� text)
        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point pos_ = { 0.0, 0.0 };
        Point offset_ = { 0.0, 0.0 };
        std::string font_size_ = "1";
        std::string font_family_;
        std::string font_weight_;
        std::string data_ = "";
    };
// _____________________________ class Text end _____________________________ //


// +++++++++++++++++++++++++++++ class ObjectContainer begin +++++++++++++++++++++++++++++ //
    class ObjectContainer
    {
    public:
        virtual void AddPtr(std::unique_ptr<Object>&& object) = 0;

        template<typename Obj>
        void Add(Obj object);

        virtual ~ObjectContainer() = default;
    protected:
        std::list<std::unique_ptr<Object>> objects_;
    };
// _____________________________ class ObjectContainer end _____________________________ //


// +++++++++++++++++++++++++++++ class Document begin +++++++++++++++++++++++++++++ //

    class Document : public ObjectContainer
    {
    public:
        Document() = default;

        // ��������� � svg-�������� ������-��������� svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // ������� � ostream svg-������������� ���������
        void Render(std::ostream& out) const;
    };
// _____________________________ class Document end _____________________________ //


// +++++++++++++++++++++++++++++ class Drawable begin +++++++++++++++++++++++++++++ //
    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };
// _____________________________ class Drawable end _____________________________ //



// ----------------------------- ��������� ������ � ������� ----------------------------- //
    template<typename Obj>
    void ObjectContainer::Add(Obj object)
    {
        objects_.emplace_back(std::make_unique<Obj>(std::move(object)));
    }
}  // namespace svg