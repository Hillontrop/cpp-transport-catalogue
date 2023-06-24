#pragma once

#include "geo.h"

#include <algorithm>
#include <deque>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace transport_guide
{
	namespace catalogue
	{
		class TransportCatalogue
		{
		public:
			struct Stop
			{
				std::string name_stop_ = {};
				geo::Coordinates coordinates_ = {};
				std::set<std::string_view> buses = {};
			};
			struct Bus
			{
				std::string name_bus_ = {};
				std::vector<Stop*> stops_ = {};
				bool is_roundtrip_ = false;	// json
			};
			struct BusInfo
			{
				int count_stops = 0;
				int unique_stops = 0;
				int route_length = 0;
				double curvature = 0.0;
			};

			struct StopsHasher
			{
				size_t operator() (const std::pair<Stop*, Stop*>& stops) const
				{
					size_t hash = std::hash<const void*>{}(stops.first);
					hash = hash * 37 + std::hash<const void*>{}(stops.second);
					return hash;
				}
			};

			void AddStop(const std::string& name_stop, const geo::Coordinates& coordinates);	// Добавить остановку

			void AddBus(const std::string& name_bus, const std::vector<Stop*> stops, bool is_roundtrip);	// Добавить маршрут

			void SetDistanceBetweenStops(const std::string& name_stop_first, const std::string& name_stop_second, const int& distance);	// Добавить расстояние между остановками

			Stop* FindStop(const std::string& name_stop);	// Поиск остановки по имени

			Bus* FindBus(const std::string& name_bus);	// Поиск маршрута по имени

			int GetDistance(const std::pair<Stop*, Stop*>& two_stops);	// Получить дистанцию между 2 остановками

			BusInfo GetBusInfo(const std::string& name_bus);	// Получение информации о маршруте

			std::set<std::string_view> GetStopInfo(const std::string& name_stop);	// Получение информации об остановке

			std::vector<TransportCatalogue::Bus*> GetSortedBusesByName();

			std::vector<TransportCatalogue::Stop*> GetSortedStopsByName();

			std::vector<geo::Coordinates> GetAllStopCoordinates() const;

		private:
			std::deque<Stop> stops_;
			std::unordered_map<std::string_view, Stop*> index_stops_;
			std::deque<Bus> buses_;
			std::unordered_map<std::string_view, Bus*> index_buses_;
			std::unordered_map<std::pair<Stop*, Stop*>, int, StopsHasher> distance_;
		};
	}
}