#pragma once

#include "json_reader.h"
#include "json.h"
#include <optional>
#include "transport_catalogue.h"


std::string PrintAnswer(const json::Node& node);

void RequestHandler(transport_guide::catalogue::TransportCatalogue& catalogue, std::vector<StatRequest> requests, const std::string& map_to_string);