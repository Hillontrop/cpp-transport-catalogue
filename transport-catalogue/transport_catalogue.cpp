#include "transport_catalogue.h"

namespace transport_guide
{
	namespace catalogue
	{
		void TransportCatalogue::AddStop(const std::string& name_stop, const Coordinates& coordinates)	// Добавить остановку
		{
			stops_.push_back({ name_stop, coordinates });
			index_stops_[stops_.back().name_stop_] = &stops_.back();
		}

		void TransportCatalogue::AddBus(const std::string& name_bus, const std::vector<Stop*> stops)	// Добавить маршрут
		{
			buses_.push_back({ name_bus, stops });
			index_buses_[buses_.back().name_bus_] = &buses_.back();
		}

		void TransportCatalogue::AddDistanceBetweenStops(const std::string& name_stop_first, const std::string& name_stop_second, const size_t& distance)	// Добавить расстояние между остановками
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

		size_t TransportCatalogue::GetDistance(const std::pair<TransportCatalogue::Stop*, TransportCatalogue::Stop*>& two_stops)
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
			size_t count_stops = TransportCatalogue::FindBus(name_bus)->stops_.size();
			std::unordered_set<std::string_view> unique_stops;
			unique_stops.insert(TransportCatalogue::FindBus(name_bus)->stops_[count_stops - 1]->name_stop_);

			size_t route_length = 0;
			double geo_route_length = 0.0;

			for (size_t i = 0; i < count_stops - 1; ++i)
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

			return { count_stops, unique_stops.size(), route_length, curvature };
		}

		std::set<std::string_view> TransportCatalogue::GetStopInfo(const std::string& name_stop)	// Получение информации об остановке
		{
			return TransportCatalogue::FindStop(name_stop)->buses;
		}
	}
}