#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <transport_catalogue.pb.h>


void SerializationToFile(const std::string& file, const transport_guide::catalogue::TransportCatalogue& catalogue);

transport_guide::catalogue::TransportCatalogue DeserializationFromFile(const std::string& file);