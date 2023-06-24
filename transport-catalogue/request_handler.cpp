#include "request_handler.h"

std::string PrintAnswer(const json::Node& node)
{
	std::ostringstream out;
	json::print::Print(json::Document{ node }, out);
	return out.str();
}


void RequestHandler(transport_guide::catalogue::TransportCatalogue& catalogue, std::vector<StatRequest> requests, const std::string& map_to_string)
{
	json::Array arr;

	for (const auto& request : requests)
	{
		if (request.GetType() == "Bus")
		{
			json::Dict dict;
			if (catalogue.FindBus(request.GetName()) == nullptr)
			{
				dict["request_id"] = request.id;
				dict["error_message"] = std::string("not found");
			}
			else
			{
				transport_guide::catalogue::TransportCatalogue::BusInfo bus_info = catalogue.GetBusInfo(request.GetName());
				dict["curvature"] = bus_info.curvature;
				dict["request_id"] = request.id;
				dict["route_length"] = bus_info.route_length;
				dict["stop_count"] = bus_info.count_stops;
				dict["unique_stop_count"] = bus_info.unique_stops;
			}
			arr.push_back(dict);
		}
		else if (request.GetType() == "Stop")
		{
			json::Dict dict;
			if (catalogue.FindStop(request.GetName()) == nullptr)
			{
				dict["request_id"] = request.id;
				dict["error_message"] = std::string("not found");
			}
			else
			{
				std::set<std::string_view> stop_info = catalogue.GetStopInfo(request.GetName());
				json::Array buses;

				for (const auto& bus : stop_info)
				{
					buses.push_back(static_cast<std::string>(bus));
				}

				dict["buses"] = buses;
				dict["request_id"] = request.id;
			}
			arr.push_back(dict);
		}
		else if (request.GetType() == "Map")
		{
			json::Dict dict;
			dict["map"] = map_to_string;
			dict["request_id"] = request.id;
			arr.push_back(dict);
		}
		else
		{
			throw std::logic_error("request type not found");
		}
	}
	std::cout << PrintAnswer(arr) << std::endl;;
}