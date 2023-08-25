#include "transport_catalogue.h"

namespace transport_guide
{
	namespace catalogue
	{
		void TransportCatalogue::AddStop(const std::string& name_stop, const geo::Coordinates& coordinates)	// �������� ���������
		{
			size_t id = stops_.empty() ? 0 : (stops_.back().id_ + 1);
			stops_.push_back({ id, name_stop, coordinates });
			index_stops_[stops_.back().name_stop_] = &stops_.back();
		}

		void TransportCatalogue::AddStop(const std::string& name_stop, const geo::Coordinates& coordinates, size_t id)
		{
			stops_.push_back({ id, name_stop, coordinates });
			index_stops_[stops_.back().name_stop_] = &stops_.back();
		}

		void TransportCatalogue::AddBus(const std::string& name_bus, const std::vector<Stop*> stops, bool is_roundtrip)	// �������� �������
		{
			buses_.push_back({ name_bus, stops, is_roundtrip });
			index_buses_[buses_.back().name_bus_] = &buses_.back();
		}

		void TransportCatalogue::SetDistanceBetweenStops(const std::string& name_stop_first, const std::string& name_stop_second, const int& distance)	// �������� ���������� ����� �����������
		{
			distance_[{TransportCatalogue::FindStop(name_stop_first), TransportCatalogue::FindStop(name_stop_second)}] = distance;
		}

		TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name_stop)	// ����� ��������� �� �����
		{
			if (index_stops_.find(name_stop) == index_stops_.end())
			{
				return nullptr;
			}
			else
			{
				return index_stops_[name_stop];
			}
		}

		TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name_bus)	// ����� �������� �� �����
		{
			if (index_buses_.find(name_bus) == index_buses_.end())
			{
				return nullptr;
			}
			else
			{
				return index_buses_[name_bus];
			}
		}

		int TransportCatalogue::GetDistance(const std::pair<TransportCatalogue::Stop*, TransportCatalogue::Stop*>& two_stops)
		{
			if (distance_.find(two_stops) == distance_.end())
			{
				return distance_[{ two_stops.second, two_stops.first}];
			}
			else
			{
				return distance_[two_stops];
			}
		}

		TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(const std::string& name_bus)	// ��������� ���������� � ��������
		{
			int count_stops = static_cast<int>(TransportCatalogue::FindBus(name_bus)->stops_.size());
			std::unordered_set<std::string_view> unique_stops;
			unique_stops.insert(TransportCatalogue::FindBus(name_bus)->stops_[count_stops - 1]->name_stop_);

			int route_length = 0;
			double geo_route_length = 0.0;

			for (int i = 0; i < count_stops - 1; ++i)
			{
				auto first_stop = TransportCatalogue::FindBus(name_bus)->stops_[i];
				auto second_stop = TransportCatalogue::FindBus(name_bus)->stops_[i + 1];
				geo_route_length += ComputeDistance(first_stop->coordinates_, second_stop->coordinates_);

				route_length += GetDistance({ first_stop, second_stop });
				unique_stops.insert(TransportCatalogue::FindBus(name_bus)->stops_[i]->name_stop_);
			}

			if (count_stops % 2 == 1)	// ���������, ��� �� � ��������� �������� � ����� ��������� ��� ��������������� ����������
			{
				bool symmetry = true;
				auto begin = TransportCatalogue::FindBus(name_bus)->stops_.begin();
				auto end = TransportCatalogue::FindBus(name_bus)->stops_.end() - 1;

				while (begin < end)
				{
					if (*begin != *end)
					{
						symmetry = false;
					}
					++begin;
					--end;
				}
				if (symmetry)
				{
					auto name_symetry = TransportCatalogue::FindBus(name_bus)->stops_[count_stops / 2];
					if (distance_.find({ name_symetry, name_symetry }) != distance_.end())
					{
						route_length += GetDistance({ name_symetry, name_symetry });
					}
				}
			}
			double curvature = route_length / geo_route_length;

			return { count_stops, static_cast<int>(unique_stops.size()), route_length, curvature };
		}

		std::set<std::string_view> TransportCatalogue::GetStopInfo(const std::string& name_stop)	// ��������� ���������� �� ���������
		{
			return TransportCatalogue::FindStop(name_stop)->buses;
		}

		std::vector<TransportCatalogue::Bus*> TransportCatalogue::GetSortedBusesByName()
		{
			std::vector<TransportCatalogue::Bus*> sorted_buses;
			sorted_buses.reserve(index_buses_.size());

			for (const auto& busPair : index_buses_)
			{
				sorted_buses.push_back(busPair.second);
			}
			std::sort(sorted_buses.begin(), sorted_buses.end(),[](const TransportCatalogue::Bus* a, const TransportCatalogue::Bus* b)
				{
					return a->name_bus_ < b->name_bus_;
				});

			return sorted_buses;
		}

		std::vector<TransportCatalogue::Stop*> TransportCatalogue::GetSortedStopsByName()
		{
			std::vector<TransportCatalogue::Stop*> sorted_stops;
			sorted_stops.reserve(index_stops_.size());

			for (const auto& stopPair : index_stops_)
			{
				sorted_stops.push_back(stopPair.second);
			}
			std::sort(sorted_stops.begin(), sorted_stops.end(), [](const TransportCatalogue::Stop* a, const TransportCatalogue::Stop* b)
				{
					return a->name_stop_ < b->name_stop_;
				});

			return sorted_stops;
		}

		std::vector<geo::Coordinates> TransportCatalogue::GetAllStopCoordinates() const
		{
			std::vector<geo::Coordinates> all_coordinates;
			all_coordinates.reserve(stops_.size());

			for (const auto& stop : stops_)
			{
				if (static_cast<int>(stop.buses.size()) != 0)
				{
					all_coordinates.push_back(stop.coordinates_);
				}
			}
			all_coordinates.resize(all_coordinates.size());
			return all_coordinates;
		}

		void TransportCatalogue::BuildGraph()
		{
			graph::DirectedWeightedGraph<double> graph(stops_.size());

			for (const auto& [name_bus, info_bus] : index_buses_)
			{
				std::vector<int> distance_between_stops;
				for (size_t i = 0; i < info_bus->stops_.size() - 1; ++i)
				{
					distance_between_stops.push_back(GetDistance({ info_bus->stops_[i], info_bus->stops_[i + 1] }));
				}

				for (size_t i = 0; i < info_bus->stops_.size(); ++i)
				{
					for (size_t j = i; j < info_bus->stops_.size(); ++j)
					{
						int span_count = j - i;

						int road_distances = std::accumulate(distance_between_stops.begin() + i, distance_between_stops.begin() + j, 0);

						graph::Edge<double> enge_bus{ { name_bus, span_count, info_bus->stops_[i]->name_stop_, info_bus->stops_[j]->name_stop_ }, info_bus->stops_[i]->id_,info_bus->stops_[j]->id_,
							(road_distances / GetRoutingSettings().bus_velocity_) + GetRoutingSettings().bus_wait_time_ };

						graph.AddEdge(std::move(enge_bus));
					}
				}
				graph::Edge<double> enge_bus{ { name_bus, 1, info_bus->stops_[info_bus->stops_.size() - 1]->name_stop_, info_bus->stops_[0]->name_stop_}, info_bus->stops_[info_bus->stops_.size() - 1]->id_,info_bus->stops_[0]->id_,
					GetRoutingSettings().bus_wait_time_ };
				graph.AddEdge(std::move(enge_bus));
			}
			graph_ = std::move(graph);
		}

		const graph::DirectedWeightedGraph<double>& TransportCatalogue::GetGraph() const
		{
			return graph_;
		}

		void TransportCatalogue::SetRoutingSettings(RoutingSettings routing_settings)
		{
			routing_settings_ = std::move(routing_settings);
		}

		void TransportCatalogue::SetMapParameter(MapParameter map_parameter)
		{
			map_parameter_ = std::move(map_parameter);
		}

		void TransportCatalogue::SetGraph(graph::DirectedWeightedGraph<double> graph)
		{
			graph_ = std::move(graph);
		}

		const RoutingSettings& TransportCatalogue::GetRoutingSettings() const
		{
			return routing_settings_;
		}

		const TransportCatalogue::MapParameter& TransportCatalogue::GetMapParameter() const
		{
			return map_parameter_;
		}

		void TransportCatalogue::SerializationToFile(const std::string& file) const
		{
			std::ofstream out(file, std::ios::out | std::ios::binary);

			if (!out.is_open())
			{
				std::cerr << "Failed to open file for writing: " << file << std::endl;
				return;
			}

			transport_guide_catalogue::TransportCatalogue catalogue_proto;

			// �������������� ���������
			for (const auto stop : stops_)
			{
				transport_guide_catalogue::Stop* stop_proto = catalogue_proto.add_stops();
				stop_proto->set_id(stop.id_);
				stop_proto->set_name_stop(stop.name_stop_);

				transport_guide_catalogue::Coordinates* coordinates_proto = stop_proto->mutable_coordinates();
				coordinates_proto->set_lat(stop.coordinates_.lat);
				coordinates_proto->set_lng(stop.coordinates_.lng);
			}

			// �������������� ���������
			for (const auto& bus : buses_)
			{
				transport_guide_catalogue::Bus* bus_proto = catalogue_proto.add_buses();
				bus_proto->set_name_bus(bus.name_bus_);
				bus_proto->set_is_roundtrip(bus.is_roundtrip_);

				for (const Stop* stop : bus.stops_)
				{
					bus_proto->add_stops(stop->name_stop_);
				}
			}

			// �������������� ���������
			for (const auto& [from_to, distance] : distance_)
			{
				transport_guide_catalogue::Distance* distance_proto = catalogue_proto.add_distances();
				distance_proto->set_name_stop_from(from_to.first->name_stop_);
				distance_proto->set_name_stop_to(from_to.second->name_stop_);
				distance_proto->set_distance(distance);
			}

			// ��������� ������ �� MapParameter
			TransportCatalogue::MapParameter parameter = GetMapParameter();
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

			// ��������� ����� �������
			for (const auto& color : parameter.color_palette_)
			{
				map_parameter_proto->add_color_palette(color);
			}
			
			// ��������� ��������� ���������
			transport_guide_catalogue::RoutingSettings* routing_settings_proto = catalogue_proto.mutable_routing_settings();
			routing_settings_proto->set_bus_wait_time(routing_settings_.bus_wait_time_);
			routing_settings_proto->set_bus_velocity(routing_settings_.bus_velocity_);

			// ������������ � ����
			std::string serialized_data;
			catalogue_proto.SerializeToString(&serialized_data);
			out.write(serialized_data.c_str(), serialized_data.size());
			out.close();
		}
	}
}
