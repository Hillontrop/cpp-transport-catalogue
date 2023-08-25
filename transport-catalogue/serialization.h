#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <transport_catalogue.pb.h>


void SerializationStops(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // Преобразование остановок
void SerializationBuses(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // Преобразование автобусов
void SerializationDistances(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // Преобразование дистанций
void SerializationMapParameter(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // Добавляем данные из MapParameter
void SerializationRoutingSettings(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // Добавляем настройки маршрутов

void SerializationToFile(const std::string& file, const transport_guide::catalogue::TransportCatalogue& catalogue);


void DeserializeStops(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // Десериализация остановок
void DeserializeBuses(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // Десериализация автобусов
void DeserializeDistances(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // Десериализация растояний
void DeserializeMapParameter(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // Десериализация параметров карты
void DeserializeRoutingSettings(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // Десериализация параметров маршрутов

transport_guide::catalogue::TransportCatalogue DeserializationFromFile(const std::string& file);