#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <transport_catalogue.pb.h>


void SerializationStops(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // �������������� ���������
void SerializationBuses(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // �������������� ���������
void SerializationDistances(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // �������������� ���������
void SerializationMapParameter(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // ��������� ������ �� MapParameter
void SerializationRoutingSettings(const transport_guide::catalogue::TransportCatalogue& catalogue, transport_guide_catalogue::TransportCatalogue& catalogue_proto); // ��������� ��������� ���������

void SerializationToFile(const std::string& file, const transport_guide::catalogue::TransportCatalogue& catalogue);


void DeserializeStops(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // �������������� ���������
void DeserializeBuses(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // �������������� ���������
void DeserializeDistances(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // �������������� ���������
void DeserializeMapParameter(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // �������������� ���������� �����
void DeserializeRoutingSettings(const transport_guide_catalogue::TransportCatalogue& catalogue_proto, transport_guide::catalogue::TransportCatalogue& catalogue); // �������������� ���������� ���������

transport_guide::catalogue::TransportCatalogue DeserializationFromFile(const std::string& file);