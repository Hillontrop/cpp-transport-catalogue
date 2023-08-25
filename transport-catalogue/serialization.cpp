#include "serialization.h"

void SerializationToFile(const std::string& file, const transport_guide::catalogue::TransportCatalogue& catalogue)
{
	catalogue.SerializationToFile(file);
}

transport_guide::catalogue::TransportCatalogue DeserializationFromFile(const std::string& file)
{
	transport_guide::catalogue::TransportCatalogue catalogue;

	std::ifstream in(file, std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		std::cerr << "Failed to open file for reading: " << file << std::endl;
		return {};
	}

	transport_guide_catalogue::TransportCatalogue catalogue_proto;
	if (!catalogue_proto.ParseFromIstream(&in))
	{
		std::cerr << "Failed to parse protobuf data from file: " << file << std::endl;
		return {};
	}

	// Десериализация остановок
	for (const auto& stop_proto : catalogue_proto.stops())
	{
		catalogue.AddStop(stop_proto.name_stop(), { stop_proto.coordinates().lat(), stop_proto.coordinates().lng() }, stop_proto.id());	// Добавить остановку
	}

	// Десериализация автобусов
	for (const auto& bus_proto : catalogue_proto.buses())
	{
		std::vector<transport_guide::catalogue::TransportCatalogue::Stop*> stops;
		for (const auto stop_name : bus_proto.stops())
		{
			transport_guide::catalogue::TransportCatalogue::Stop* stop = catalogue.FindStop(stop_name);
			if (stop)
			{
				stops.push_back(stop);
			}
		}
		catalogue.AddBus(bus_proto.name_bus(), stops, bus_proto.is_roundtrip());

		for (auto& stop : stops)
		{
			catalogue.FindStop(stop->name_stop_)->buses.insert(catalogue.FindBus(bus_proto.name_bus())->name_bus_);
		}
	}

	// Десериализация растояний
	for (const auto& stop_proto : catalogue_proto.distances())
	{
		catalogue.SetDistanceBetweenStops(stop_proto.name_stop_from(), stop_proto.name_stop_to(), stop_proto.distance());
	}

	// Десериализация параметров карты
	transport_guide::catalogue::TransportCatalogue::MapParameter map_parameter;
	const auto& map_parameter_proto = catalogue_proto.map_parameter();
	map_parameter.width_ = map_parameter_proto.width();
	map_parameter.height_ = map_parameter_proto.height();
	map_parameter.padding_ = map_parameter_proto.padding();
	map_parameter.line_width_ = map_parameter_proto.line_width();
	map_parameter.stop_radius_ = map_parameter_proto.stop_radius();
	map_parameter.bus_label_font_size_ = map_parameter_proto.bus_label_font_size();
	map_parameter.bus_label_offset_ = { map_parameter_proto.bus_label_offset().x(), map_parameter_proto.bus_label_offset().y() };
	map_parameter.stop_label_font_size_ = map_parameter_proto.stop_label_font_size();
	map_parameter.stop_label_offset_ = { map_parameter_proto.stop_label_offset().x(), map_parameter_proto.stop_label_offset().y() };
	map_parameter.underlayer_color_ = map_parameter_proto.underlayer_color();
	map_parameter.underlayer_width_ = map_parameter_proto.underlayer_width();

	for (const auto& color : map_parameter_proto.color_palette())
	{
		map_parameter.color_palette_.push_back(color);
	}
	catalogue.SetMapParameter(map_parameter);

	// Десериализация параметров маршрутов
	transport_guide::catalogue::RoutingSettings routing_settings;
	const auto& routing_settings_proto = catalogue_proto.routing_settings();
	routing_settings.bus_wait_time_ = routing_settings_proto.bus_wait_time();
	routing_settings.bus_velocity_ = routing_settings_proto.bus_velocity();
	catalogue.SetRoutingSettings(routing_settings);


	in.close();
	return catalogue;
}