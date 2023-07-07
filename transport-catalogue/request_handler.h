#pragma once

#include "json_reader.h"
#include "json.h"
#include "json_builder.h"
#include <optional>
#include "transport_catalogue.h"


void RequestHandler(transport_guide::catalogue::TransportCatalogue& catalogue, std::vector<StatRequest> requests, const std::string& map_to_string);