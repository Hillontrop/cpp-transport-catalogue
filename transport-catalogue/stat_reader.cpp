#include "stat_reader.h"

namespace transport_guide
{
	namespace print
	{
		void PrintInfoBus(catalogue::TransportCatalogue& catalogue, std::vector<std::pair<std::string, std::string>> queue_request)
		{
			for (const auto& [iden, request] : queue_request)
			{
				if (iden == "Bus")
				{
					if (catalogue.FindBus(request) == nullptr)
					{
						std::cout << "Bus " << request << ": not found" << std::endl;
					}
					else
					{
						catalogue::TransportCatalogue::BusInfo info = catalogue.GetBusInfo(request);
						std::cout << "Bus " << request << ": "
							<< info.count_stops << " stops on route, "
							<< info.unique_stops << " unique stops, "
							<< info.route_length << " route length, " << std::setprecision(6)
							<< info.curvature << " curvature" << std::endl;
					}
				}
				else
				{
					if (catalogue.FindStop(request) == nullptr)
					{
						std::cout << "Stop " << request << ": not found" << std::endl;
					}
					else if (catalogue.FindStop(request)->buses.empty())
					{
						std::cout << "Stop " << request << ": no buses" << std::endl;
					}
					else
					{
						std::cout << "Stop " << request << ": buses";
						for (const auto& bus : catalogue.GetStopInfo(request))
						{
							std::cout << ' ' << bus;
						}
						std::cout << std::endl;
					}
				}
			}
		}
	}
}