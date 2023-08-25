#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "serialization.h"

#include <string>
#include <fstream>
#include <iostream>
#include <string_view>
#include <filesystem>

using namespace std::literals;
using namespace transport_guide;

void PrintUsage(std::ostream& stream = std::cerr)
{
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv)
    {
        catalogue::TransportCatalogue catalogue;

        DataRequests query = ReaderInputCatalogueUpdate(std::cin);
        BuildCatalogue(catalogue, query.base_request_, query.routing_settings_, query.parametr_);
        SerializationToFile(query.save_path_, catalogue);    // Cериализация в файл
    }
    else if (mode == "process_requests"sv)
    {
        DataRequests query = ReaderInputCatalogueUpdate(std::cin);

        catalogue::TransportCatalogue catalogue = DeserializationFromFile(query.save_path_);
        catalogue.BuildGraph();
        std::string map_buses_and_stopes = MapRenderer(catalogue);
        RequestHandler(catalogue, query.stat_request_, map_buses_and_stopes);
    }
    else
    {
        PrintUsage();
        return 1;
    }
}