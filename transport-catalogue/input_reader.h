#pragma once

#include "transport_catalogue.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace transport_guide
{
    namespace reader_input
    {
        namespace detail
        {
            struct Request
            {
                void clear()
                {
                    iden.clear();
                    name.clear();
                    coordinates.clear();
                    stops_or_distance.clear();
                }
                std::string iden;
                std::string name;
                Coordinates coordinates;
                std::string stops_or_distance;
            };
        }
        std::vector<detail::Request> ReaderInputCatalogueUpdate(std::istream& input);
        std::vector<std::pair<std::string, std::string>> ReaderInputRequests(std::istream& input);
    }
    namespace craft
    {
        catalogue::TransportCatalogue& CraftCatalogue(catalogue::TransportCatalogue& catalogue, const std::vector<reader_input::detail::Request>& queue_request);

        namespace detail
        {
            std::vector<catalogue::TransportCatalogue::Stop*> SplitIntoStops(catalogue::TransportCatalogue& catalogue, const std::string str);
            std::vector<std::pair<size_t, std::string>> SplitIntoDistanceToStops(std::string distance_to_stop);
        }
    }
}