#pragma once

#include "geo.h"
#include "json.h"
#include "svg.h"
#include "transport_catalogue.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

class Request
{
public:
    Request() = default;

    virtual ~Request() = default;

    const std::string& GetTypeRequest() const;
    const std::string& GetType() const;
    const std::string& GetName() const;

    void SetTypeRequest(std::string type_request);
    void SetType(std::string type);
    void SetName(std::string name);
private:
    std::string type_request_;
    std::string type_;
    std::string name_;
};

class BaseRequest : public Request
{
public:
    geo::Coordinates coordinates;
    std::unordered_map<std::string, int> road_distances;
    std::vector<std::string> stops;
    bool is_roundtrip;
};

class StatRequest : public Request
{
public:
    int id;
    std::string from_stop;
    std::string to_stop;
};

struct DataRequests
{
    std::vector<BaseRequest> base_request_;
    transport_guide::catalogue::TransportCatalogue::MapParameter parametr_;
    transport_guide::catalogue::RoutingSettings routing_settings_;
    std::vector<StatRequest> stat_request_;
    std::string save_path_;
};

std::vector<BaseRequest> ReaderBaseRequests(const json::Node& data);

std::vector<StatRequest> ReaderStatRequests(const json::Node& data);

transport_guide::catalogue::TransportCatalogue::MapParameter ReaderRenderSettings(const json::Node& data);

DataRequests ReaderInputCatalogueUpdate(std::istream& input);   // Приняли поток выдали вектор запросов

transport_guide::catalogue::TransportCatalogue& BuildCatalogue(transport_guide::catalogue::TransportCatalogue& catalogue, const std::vector<BaseRequest>& requests, const transport_guide::catalogue::RoutingSettings& routing_settings, const transport_guide::catalogue::TransportCatalogue::MapParameter& map_parameter);

std::vector<transport_guide::catalogue::TransportCatalogue::Stop*> SplitIntoStops(transport_guide::catalogue::TransportCatalogue& catalogue, const std::vector<std::string>& stops, bool is_roundtrip);