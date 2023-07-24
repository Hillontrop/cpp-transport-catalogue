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

struct MapParameter
{
    double width_;
    double height_;
    double padding_;
    double line_width_;
    double stop_radius_;
    int bus_label_font_size_;
    svg::Point bus_label_offset_;
    int stop_label_font_size_;
    svg::Point stop_label_offset_;
    svg::Color underlayer_color_;
    double underlayer_width_;
    std::vector<svg::Color> color_palette_;
};

struct DataRequests
{
    std::vector<BaseRequest> base_request_;
    MapParameter parametr_;
    transport_guide::catalogue::RoutingSettings routing_settings_;
    std::vector<StatRequest> stat_request_;
};

std::vector<BaseRequest> ReaderBaseRequests(const json::Node& data);

std::vector<StatRequest> ReaderStatRequests(const json::Node& data);

MapParameter ReaderRenderSettings(const json::Node& data);

DataRequests ReaderInputCatalogueUpdate(std::istream& input);   

transport_guide::catalogue::TransportCatalogue& BuildCatalogue(transport_guide::catalogue::TransportCatalogue& catalogue, const std::vector<BaseRequest>& requests, const transport_guide::catalogue::RoutingSettings& routing_settings);

std::vector<transport_guide::catalogue::TransportCatalogue::Stop*> SplitIntoStops(transport_guide::catalogue::TransportCatalogue& catalogue, const std::vector<std::string>& stops, bool is_roundtrip);