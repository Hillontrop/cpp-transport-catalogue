#include "request_handler.h"

void RequestHandler(transport_guide::catalogue::TransportCatalogue& catalogue, std::vector<StatRequest> requests, const std::string& map_to_string)
{
	json::Builder builder;
	builder.StartArray();

	const graph::DirectedWeightedGraph<double>& graph = catalogue.GetGraph();
	graph::Router router(graph);


	for (const auto& request : requests)
	{
		if (request.GetType() == "Bus")
		{
			if (catalogue.FindBus(request.GetName()) == nullptr)
			{
				builder.StartDict()
					.Key("request_id").Value(request.id)
					.Key("error_message").Value("not found")
					.EndDict();
			}
			else
			{
				transport_guide::catalogue::TransportCatalogue::BusInfo bus_info = catalogue.GetBusInfo(request.GetName());

				builder.StartDict()
					.Key("curvature").Value(bus_info.curvature)
					.Key("request_id").Value(request.id)
					.Key("route_length").Value(bus_info.route_length)
					.Key("stop_count").Value(bus_info.count_stops)
					.Key("unique_stop_count").Value(bus_info.unique_stops)
					.EndDict();
			}
		}
		else if (request.GetType() == "Stop")
		{
			if (catalogue.FindStop(request.GetName()) == nullptr)
			{
				builder.StartDict()
					.Key("request_id").Value(request.id)
					.Key("error_message").Value("not found")
					.EndDict();
			}
			else
			{
				std::set<std::string_view> stop_info = catalogue.GetStopInfo(request.GetName());

				builder.StartDict().Key("buses").StartArray();

				for (const auto& bus : stop_info)
				{
					builder.Value(static_cast<std::string>(bus));
				}
				builder.EndArray().Key("request_id").Value(request.id).EndDict();
			}
		}
		else if (request.GetType() == "Map")
		{
			builder.StartDict().Key("map").Value(map_to_string).Key("request_id").Value(request.id).EndDict();
		}
		else if (request.GetType() == "Route")
		{
			auto route = router.BuildRoute(catalogue.FindStop(request.from_stop)->id_, catalogue.FindStop(request.to_stop)->id_);
			if (!route.has_value())
			{
				builder.StartDict()
					.Key("request_id").Value(request.id)
					.Key("error_message").Value("not found")
					.EndDict();
			}
			else
			{
				builder.StartDict()
					.Key("request_id").Value(request.id)
					.Key("total_time").Value(route->weight)
					.Key("items").StartArray();

				for (const auto& edge : route->edges)
				{
					builder.StartDict()
						.Key("stop_name").Value(std::string{ graph.GetEdge(edge).info_edge.stop_name_from })
						.Key("time").Value(catalogue.GetRoutingSettings().bus_wait_time_)
						.Key("type").Value("Wait")
						.EndDict();
					builder.StartDict().Key("bus").Value(std::string{ graph.GetEdge(edge).info_edge.name_bus })
						.Key("span_count").Value(graph.GetEdge(edge).info_edge.span_count)
						.Key("time").Value(graph.GetEdge(edge).weight - catalogue.GetRoutingSettings().bus_wait_time_)
						.Key("type").Value("Bus")
						.EndDict();
				}
				builder.EndArray().EndDict();
			}
		}
		else
		{
			throw std::logic_error("request type not found");
		}
	}
	builder.EndArray();

	json::Print(json::Document{ builder.Build() }, std::cout);
	std::cout << std::endl;
}