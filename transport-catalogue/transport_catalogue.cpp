#include "transport_catalogue.h"

namespace transport_guide
{
	namespace catalogue
	{
		void TransportCatalogue::AddStop(const std::string& name_stop, const geo::Coordinates& coordinates)	// Добавить остановку
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

		void TransportCatalogue::AddBus(const std::string& name_bus, const std::vector<Stop*> stops, bool is_roundtrip)	// Добавить маршрут
		{
			buses_.push_back({ name_bus, stops, is_roundtrip });
			index_buses_[buses_.back().name_bus_] = &buses_.back();
		}

		void TransportCatalogue::SetDistanceBetweenStops(const std::string& name_stop_first, const std::string& name_stop_second, const int& distance)	// Добавить расстояние между остановками
		{
			distance_[{TransportCatalogue::FindStop(name_stop_first), TransportCatalogue::FindStop(name_stop_second)}] = distance;
		}

		TransportCatalogue::Stop* TransportCatalogue::FindStop(const std::string& name_stop)	// Поиск остановки по имени
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

		TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string& name_bus)	// Поиск маршрута по имени
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

		TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(const std::string& name_bus)	// Получение информации о маршруте
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

			if (count_stops % 2 == 1)	// Проверяем, что не в кольцевом маршруте в точке разворота нет дополнительного расстояния
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

		std::set<std::string_view> TransportCatalogue::GetStopInfo(const std::string& name_stop)	// Получение информации об остановке
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

		const std::deque<TransportCatalogue::Stop>& TransportCatalogue::GetStops() const
		{
			return stops_;
		}

		const std::deque<TransportCatalogue::Bus>& TransportCatalogue::GetBuses() const
		{
			return buses_;
		}

		const std::unordered_map<std::pair<TransportCatalogue::Stop*, TransportCatalogue::Stop*>, int, TransportCatalogue::StopsHasher>& TransportCatalogue::GetDistance() const
		{
			return distance_;
		}
	}
}
