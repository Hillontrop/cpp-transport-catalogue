#pragma once

#include "json_reader.h"
#include "json.h"
#include "json_builder.h"
#include "transport_catalogue.h"
#include "router.h"

#include <optional>


void RequestHandler(transport_guide::catalogue::TransportCatalogue& catalogue, std::vector<StatRequest> requests, const std::string& map_to_string);