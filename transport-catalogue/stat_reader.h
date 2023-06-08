#pragma once

#include "transport_catalogue.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace transport_guide
{
	namespace print
	{
		void PrintInfoBus(catalogue::TransportCatalogue& catalogue, std::vector<std::pair<std::string, std::string>> queue_request);
	}
}