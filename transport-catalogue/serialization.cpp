#include "serialization.h"

void SerializationStops(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto)
{
	for (const auto stop : catalogue.GetStops())
	{
		transport_guide_catalogue::Stop* stop_proto = catalogue_proto.add_stops();
		stop_proto->set_id(stop.id_);
		stop_proto->set_name_stop(stop.name_stop_);

		transport_guide_catalogue::Coordinates* coordinates_proto = stop_proto->mutable_coordinates();
		coordinates_proto->set_lat(stop.coordinates_.lat);
		coordinates_proto->set_lng(stop.coordinates_.lng);
	}
}

void SerializationBuses(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto)
{
	for (const auto& bus : catalogue.GetBuses())
	{
		transport_guide_catalogue::Bus* bus_proto = catalogue_proto.add_buses();
		bus_proto->set_name_bus(bus.name_bus_);
		bus_proto->set_is_roundtrip(bus.is_roundtrip_);

		for (const auto stop : bus.stops_)
		{
			bus_proto->add_stops(stop->name_stop_);
		}
	}
}

void SerializationDistances(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto)
{
	for (const auto& [from_to, distance] : catalogue.GetDistance())
	{
		transport_guide_catalogue::Distance* distance_proto = catalogue_proto.add_distances();
		distance_proto->set_name_stop_from(from_to.first->name_stop_);
		distance_proto->set_name_stop_to(from_to.second->name_stop_);
		distance_proto->set_distance(distance);
	}
}

void SerializationMapParameter(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto)
{
	transport_guide::catalogue::TransportCatalogue::MapParameter parameter = catalogue.GetMapParameter();

	transport_guide_catalogue::MapParameter* map_parameter_proto = catalogue_proto.mutable_map_parameter();
	map_parameter_proto->set_width(parameter.width_);
	map_parameter_proto->set_height(parameter.height_);
	map_parameter_proto->set_padding(parameter.padding_);
	map_parameter_proto->set_line_width(parameter.line_width_);
	map_parameter_proto->set_stop_radius(parameter.stop_radius_);
	map_parameter_proto->set_bus_label_font_size(parameter.bus_label_font_size_);
	map_parameter_proto->mutable_bus_label_offset()->set_x(parameter.bus_label_offset_.x);
	map_parameter_proto->mutable_bus_label_offset()->set_y(parameter.bus_label_offset_.y);
	map_parameter_proto->set_stop_label_font_size(parameter.stop_label_font_size_);
	map_parameter_proto->mutable_stop_label_offset()->set_x(parameter.stop_label_offset_.x);
	map_parameter_proto->mutable_stop_label_offset()->set_y(parameter.stop_label_offset_.y);
	map_parameter_proto->set_underlayer_color(parameter.underlayer_color_);
	map_parameter_proto->set_underlayer_width(parameter.underlayer_width_);

	// Добавляем цвета палитры
	for (const auto& color : parameter.color_palette_)
	{
		map_parameter_proto->add_color_palette(color);
	}
}

void SerializationRoutingSettings(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto)
{
	transport_guide::catalogue::RoutingSettings routing_settings = catalogue.GetRoutingSettings();

	transport_guide_catalogue::RoutingSettings* routing_settings_proto = catalogue_proto.mutable_routing_settings();
	routing_settings_proto->set_bus_wait_time(routing_settings.bus_wait_time_);
	routing_settings_proto->set_bus_velocity(routing_settings.bus_velocity_);
}

void SerializationToFile(const std::string& file, const transport_guide::catalogue::TransportCatalogue& catalogue)
{
	std::ofstream out(file, std::ios::out | std::ios::binary);

	if (!out.is_open())
	{
		std::cerr << "Failed to open file for writing: " << file << std::endl;
		return;
	}

	transport_guide_catalogue::TransportCatalogue catalogue_proto;

	SerializationStops(catalogue, catalogue_proto);
	SerializationBuses(catalogue, catalogue_proto);
	SerializationDistances(catalogue, catalogue_proto);
	SerializationMapParameter(catalogue, catalogue_proto);
	SerializationRoutingSettings(catalogue, catalogue_proto);

	// Сериализация в файл
	std::string serialized_data;
	catalogue_proto.SerializeToString(&serialized_data);
	out.write(serialized_data.c_str(), serialized_data.size());
	out.close();
}



void DeserializeStops(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue)
{
	for (const auto& stop_proto : catalogue_proto.stops())
	{
		catalogue.AddStop(stop_proto.name_stop(), { stop_proto.coordinates().lat(), stop_proto.coordinates().lng() }, stop_proto.id());
	}
}

void DeserializeBuses(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue)
{
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
}

void DeserializeDistances(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue)
{
	for (const auto& stop_proto : catalogue_proto.distances())
	{
		catalogue.SetDistanceBetweenStops(stop_proto.name_stop_from(), stop_proto.name_stop_to(), stop_proto.distance());
	}
}

void DeserializeMapParameter(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue)
{
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
}

void DeserializeRoutingSettings(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue)
{
	transport_guide::catalogue::RoutingSettings routing_settings;

	const auto& routing_settings_proto = catalogue_proto.routing_settings();
	routing_settings.bus_wait_time_ = routing_settings_proto.bus_wait_time();
	routing_settings.bus_velocity_ = routing_settings_proto.bus_velocity();
	catalogue.SetRoutingSettings(routing_settings);
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

	DeserializeStops(catalogue_proto, catalogue);
	DeserializeBuses(catalogue_proto, catalogue);
	DeserializeDistances(catalogue_proto, catalogue);
	DeserializeMapParameter(catalogue_proto, catalogue);
	DeserializeRoutingSettings(catalogue_proto, catalogue);

	in.close();
	return catalogue;
}