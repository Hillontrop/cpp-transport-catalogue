#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iostream>
#include <sstream>


int main()
{
	using namespace transport_guide;

	catalogue::TransportCatalogue catalogue;
	craft::CraftCatalogue(catalogue, transport_guide::reader_input::ReaderInputCatalogueUpdate(std::cin));
	print::PrintInfoBus(catalogue, transport_guide::reader_input::ReaderInputRequests(std::cin));
	return 0;
}