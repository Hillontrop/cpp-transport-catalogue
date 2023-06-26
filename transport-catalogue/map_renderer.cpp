#include "map_renderer.h"

inline const double EPSILON = 1e-6;

bool IsZero(double value)
{
    return std::abs(value) < EPSILON;
}


void AddRouteOnMap(svg::Document& documents, std::vector<svg::Text>& texts_buses, transport_guide::catalogue::TransportCatalogue::Bus* const &bus,const SphereProjector proj, const MapParameter& parametr, unsigned int& number_color)
{
    if (bus->stops_.size() != 0)   // Если остановки есть
    {
        documents.Add(DrawRoute(bus, parametr, proj, number_color));            // добавить маршрут для печати

        svg::Text substrate_bus = DrawSubstrateBus(bus, parametr, proj);
        texts_buses.push_back(substrate_bus);         // добавить текст 1 остановки для печати
        svg::Text text_bus = DrawTextBus(bus, parametr, proj, number_color);
        texts_buses.push_back(text_bus);              // добавить текст 1 остановки для печати

        if (bus->is_roundtrip_ == false && *bus->stops_.begin() != bus->stops_[static_cast<int>(bus->stops_.size()) / 2])
        {
            substrate_bus.SetPosition(proj(bus->stops_[static_cast<int>(bus->stops_.size()) / 2]->coordinates_));
            texts_buses.push_back(std::move(substrate_bus));            // добавить подложку под текст last остановки для печати
            text_bus.SetPosition(proj(bus->stops_[static_cast<int>(bus->stops_.size()) / 2]->coordinates_));
            texts_buses.push_back(std::move(text_bus));                 // добавить текст last остановки для печати
        }

        number_color == parametr.color_palette_.size() - 1 ? number_color = 0 : ++number_color;
    }
}

void AddStopOnMap(std::vector<svg::Circle>& circles_stops, std::vector<svg::Text>& texts_stops, transport_guide::catalogue::TransportCatalogue::Stop* const& stop, const SphereProjector proj, const MapParameter& parametr)
{
    if (stop->buses.size() != 0)   // Если мфршруты есть
    {
        svg::Circle circle;

        circle.SetCenter(proj(stop->coordinates_));
        circle.SetRadius(parametr.stop_radius_);
        circle.SetFillColor("white");
        circles_stops.push_back(circle);

        svg::Text substrate_stop = DrawSubstrateStop(stop, parametr, proj);
        texts_stops.push_back(substrate_stop);         // добавить текст 1 остановки для печати
        svg::Text text_stop = DrawTextStop(stop, parametr, proj);
        texts_stops.push_back(text_stop);              // добавить текст 1 остановки для печати
    }
}

std::string MapRenderer(transport_guide::catalogue::TransportCatalogue& catalogue, const MapParameter& parametr)
{
    std::vector<geo::Coordinates> geo_coords = catalogue.GetAllStopCoordinates();
    // Создаём проектор сферических координат на карту
    const SphereProjector proj{ geo_coords.begin(), geo_coords.end(), parametr.width_, parametr.height_, parametr.padding_ };


    svg::Document documents;    // Документы для печати

    std::vector<svg::Text> texts_buses;
    std::vector<svg::Circle> circles_stops;
    std::vector<svg::Text> texts_stops;

    unsigned int number_color = 0;

    for (const auto& bus : catalogue.GetSortedBusesByName())
    {
        AddRouteOnMap(documents, texts_buses, bus, proj, parametr, number_color);
    }

    for (const auto& stop : catalogue.GetSortedStopsByName())
    {
        AddStopOnMap(circles_stops, texts_stops, stop, proj, parametr);
    }

   for (const auto&t_b : texts_buses)
    {
        documents.Add(t_b);
    }
    for (const auto& c_s : circles_stops)
    {
        documents.Add(c_s);
    }
    for (const auto& t_s : texts_stops)
    {
        documents.Add(t_s);
    }
    std::ostringstream oss;
    documents.Render(oss);    // Печать
    return oss.str(); // Возвращаем строку из потока
}


svg::Polyline DrawRoute(const transport_guide::catalogue::TransportCatalogue::Bus* const bus, const MapParameter& parametr, const SphereProjector& proj, unsigned int number_color)
{
    svg::Polyline route;
    for (const auto& stop : bus->stops_)
    {
        route.AddPoint(proj(stop->coordinates_));
    }
    route.SetStrokeColor(parametr.color_palette_[number_color]);
    route.SetFillColor(svg::NoneColor);
    route.SetStrokeWidth(parametr.line_width_);
    route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return route;
}

svg::Text DrawSubstrateBus(const transport_guide::catalogue::TransportCatalogue::Bus* const bus, const MapParameter& parametr, const SphereProjector& proj)
{
    svg::Text substrate_bus;
    substrate_bus.SetPosition(proj(bus->stops_[0]->coordinates_));
    substrate_bus.SetOffset(parametr.bus_label_offset_);
    substrate_bus.SetFontSize(parametr.bus_label_font_size_);
    substrate_bus.SetFontFamily("Verdana");
    substrate_bus.SetFontWeight("bold");
    substrate_bus.SetData(bus->name_bus_);

    substrate_bus.SetStrokeColor(parametr.underlayer_color_);
    substrate_bus.SetFillColor(parametr.underlayer_color_);
    substrate_bus.SetStrokeWidth(parametr.underlayer_width_);
    substrate_bus.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    substrate_bus.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return substrate_bus;
}

svg::Text DrawTextBus(const transport_guide::catalogue::TransportCatalogue::Bus* const bus, const MapParameter& parametr, const SphereProjector& proj, unsigned int number_color)
{
    svg::Text text_bus;
    text_bus.SetPosition(proj(bus->stops_[0]->coordinates_));
    text_bus.SetOffset(parametr.bus_label_offset_);
    text_bus.SetFontSize(parametr.bus_label_font_size_);
    text_bus.SetFontFamily("Verdana");
    text_bus.SetFontWeight("bold");
    text_bus.SetData(bus->name_bus_);

    text_bus.SetFillColor(parametr.color_palette_[number_color]);
    return text_bus;
}

svg::Text DrawSubstrateStop(const transport_guide::catalogue::TransportCatalogue::Stop* const stop, const MapParameter& parametr, const SphereProjector& proj)
{
    svg::Text substrate_stop;
    substrate_stop.SetPosition(proj(stop->coordinates_));
    substrate_stop.SetOffset(parametr.stop_label_offset_);
    substrate_stop.SetFontSize(parametr.stop_label_font_size_);
    substrate_stop.SetFontFamily("Verdana");
    substrate_stop.SetData(stop->name_stop_);

    substrate_stop.SetStrokeColor(parametr.underlayer_color_);
    substrate_stop.SetFillColor(parametr.underlayer_color_);
    substrate_stop.SetStrokeWidth(parametr.underlayer_width_);
    substrate_stop.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    substrate_stop.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return substrate_stop;
}

svg::Text DrawTextStop(const transport_guide::catalogue::TransportCatalogue::Stop* const stop, const MapParameter& parametr, const SphereProjector& proj)
{
    svg::Text text_stop;
    text_stop.SetPosition(proj(stop->coordinates_));
    text_stop.SetOffset(parametr.stop_label_offset_);
    text_stop.SetFontSize(parametr.stop_label_font_size_);
    text_stop.SetFontFamily("Verdana");
    text_stop.SetData(stop->name_stop_);

    text_stop.SetFillColor("black");
    return text_stop;
}