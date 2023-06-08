#include "input_reader.h"

//************* С удалением лишник пробелов в названии маршрута *************//
//
//std::vector<Request> ReaderInputCatalogueUpdate(std::istream& input)
//{
//    std::vector<Request> queue_request;
//
//    std::string str;
//    std::getline(input, str);
//    int query_amount = std::stoi(str);
//    for (size_t i = 0; i != query_amount; ++i)
//    {
//        std::getline(input, str);
//        std::istringstream iss(std::move(str));
//
//        std::string word;
//        iss >> word;        // Идентификатор
//        Request request;
//        request.iden = std::move(word);
//
//        while (iss >> word)        // Имя
//        {
//            if (word.back() == ':' && word.size() > 1)
//            {
//                word.pop_back();
//                request.name.empty() ? request.name = std::move(word) : request.name += " " + std::move(word);
//                break;
//            }
//            else if (word == ":")
//            {
//                break;
//            }
//            else
//            {
//                request.name.empty() ? request.name = std::move(word) : request.name += " " + std::move(word);
//            }
//        }
//        if (request.iden == "Stop")     // Для остановок
//        {
//            double lat_ = 0.0;
//            double lng_ = 0.0;
//
//            while (iss >> word)
//            {
//                if (word.front() == ':' && word.size() > 1 && word.back() != ',')
//                {
//                    word = word.substr(word.find_first_not_of(":"));
//                    lat_ = std::stod(std::move(word));
//                }
//                else if (word.front() == ':' && word.size() > 1 && word.back() == ',')
//                {
//                    word.pop_back();
//                    word = word.substr(word.find_first_not_of(":"));
//                    lat_ = std::stod(std::move(word));
//                }
//                else if (word.front() != ':' && word.size() > 1 && word.back() != ',')
//                {
//                    lat_ == 0.0 ? lat_ = std::stod(std::move(word)) : lng_ = std::stod(std::move(word));
//                }
//                else if (word == ",")
//                {
//                    continue;
//                }
//                else
//                {
//                    lat_ == 0.0 ? lat_ = std::stod(std::move(word)) : lng_ = std::stod(std::move(word));
//                }
//            }
//            request.coordinates = { lat_, lng_ };
//            queue_request.push_back(std::move(request));
//            request.clear();
//        }
//        else     // Для автобусов
//        {
//            while (iss >> word)     // остановки
//            {
//                if (word.front() == ':' && word.size() > 1)
//                {
//                    word = word.substr(word.find_first_not_of(":"));
//                    request.stops = std::move(word);
//                }
//                else if ((word.front() == '>' || word.front() == '-') && word.size() > 1)
//                {
//                    request.stops += std::move(word);
//                }
//                else
//                {
//                    if (word == ">" || word == "-")
//                    {
//                        request.stops += std::move(word);
//                    }
//                    else
//                    {
//                        request.stops.empty() ? request.stops = std::move(word) : request.stops.back() == '>' || request.stops.back() == '-' ? request.stops += std::move(word) : request.stops += " " + std::move(word);
//                    }
//                }
//            }
//            queue_request.push_back(std::move(request));
//            request.clear();
//        }
//    }
//    return queue_request;
//}

namespace transport_guide
{
    namespace reader_input
    {
        std::vector<detail::Request> ReaderInputCatalogueUpdate(std::istream& input)
        {
            std::vector<detail::Request> queue_request;

            std::string str;
            std::getline(input, str);
            size_t query_amount = std::stoi(str);
            for (size_t i = 0; i != query_amount; ++i)
            {
                std::getline(input, str);
                size_t pos = 0;
                size_t len = str.find_first_of(" ") - pos;
                detail::Request request;
                request.iden = str.substr(pos, len);
                pos = str.find_first_of(" ") + 1;
                len = str.find(':') - pos;
                request.name = str.substr(pos, len);

                pos = str.find(':') + 2;

                if (request.iden == "Stop")     // Для остановок
                {
                    len = str.find(',') - pos;
                    double lat_ = std::stod(str.substr(pos, len));
                    pos = str.find(',') + 2;
                    if (str.find(',', pos) == str.npos)
                    {
                        len = str.back() - pos + 2;
                        double lng_ = std::stod(str.substr(pos, len));
                        request.coordinates = { lat_, lng_ };
                    }
                    else
                    {
                        len = str.find(',', pos) - pos + 2;
                        double lng_ = std::stod(str.substr(pos, len));
                        request.coordinates = { lat_, lng_ };
                        pos = str.find(',', pos) + 2;
                        request.stops_or_distance = str.substr(pos);
                    }
                }
                else   // Для автобусов
                {
                    request.stops_or_distance = str.substr(pos);
                }
                queue_request.push_back(request);
                request.clear();
            }
            return queue_request;
        }

        std::vector<std::pair<std::string, std::string>> ReaderInputRequests(std::istream& input)
        {
            std::vector<std::pair<std::string, std::string>> queue_request;

            std::string str;
            std::getline(input, str);
            size_t query_amount = std::stoi(str);
            for (size_t i = 0; i != query_amount; ++i)
            {
                std::getline(input, str);
                size_t space_pos = str.find_first_of(" ");

                queue_request.push_back({ str.substr(0, space_pos), str.substr(space_pos + 1,str.back() - space_pos + 1) });
            }
            return queue_request;
        }
    }
    namespace craft
    {
        catalogue::TransportCatalogue& CraftCatalogue(catalogue::TransportCatalogue& catalogue, const std::vector<reader_input::detail::Request>& queue_request)
        {
            for (const auto& request : queue_request)
            {
                if (request.iden == "Stop")
                {
                    catalogue.AddStop(request.name, request.coordinates);
                }
            }
            for (const auto& request : queue_request)
            {
                if (request.iden == "Bus")
                {
                    std::vector<catalogue::TransportCatalogue::Stop*> stops = detail::SplitIntoStops(catalogue, request.stops_or_distance);
                    catalogue.AddBus(request.name, stops);
                    for (auto& stop : stops)    // 2_3
                    {
                        catalogue.FindStop(stop->name_stop_)->buses.insert(catalogue.FindBus(request.name)->name_bus_);
                    }
                }
            }
            for (const auto& request : queue_request)   // 3_3
            {
                if (request.iden == "Stop" && !request.stops_or_distance.empty())
                {
                    for (const auto& [distance, name_stop] : detail::SplitIntoDistanceToStops(request.stops_or_distance))
                    {
                        catalogue.AddDistanceBetweenStops(request.name, name_stop, distance);
                    }
                }
            }
            return catalogue;
        }

        namespace detail
        {
            std::vector<catalogue::TransportCatalogue::Stop*> SplitIntoStops(catalogue::TransportCatalogue& catalogue, const std::string str)
            {
                std::vector<catalogue::TransportCatalogue::Stop*> stops;

                char type = str.find('>') != str.npos ? '>' : '-';

                size_t pos = 0;

                for (size_t i = 0; i < static_cast<size_t>(std::count(str.begin(), str.end(), type) + 1); ++i, pos = str.find(type, pos) + 2)
                {
                    size_t len = str.find_first_of(type, pos) - 1 - pos;
                    stops.push_back(catalogue.FindStop(str.substr(pos, len)));
                }

                if (type == '-')
                {
                    const size_t stops_count = stops.size();        // 3
                    for (size_t i = stops_count - 1; i != 0;)        // i = 1 i = 0
                    {
                        --i;
                        stops.push_back(stops[i]);
                    }
                }
                return stops;
            }

            std::vector<std::pair<size_t, std::string>> SplitIntoDistanceToStops(std::string distance_to_stop)  // 3_3
            {
                std::vector<std::pair<size_t, std::string>> result;

                size_t pos_distance = 0;
                size_t pos_name = distance_to_stop.find_first_of("m") + 5;

                for (size_t i = 0; i < static_cast<size_t>(std::count(distance_to_stop.begin(), distance_to_stop.end(), ',') + 1);
                    ++i, pos_distance = distance_to_stop.find(',', pos_distance) + 2, pos_name = distance_to_stop.find('m', pos_distance) + 5)
                {
                    size_t len_distance = distance_to_stop.find_first_of("m", pos_distance) - pos_distance;
                    size_t len_name = distance_to_stop.find_first_of(",", pos_name) - pos_name;
                    result.push_back({ std::stoi(distance_to_stop.substr(pos_distance, len_distance)),distance_to_stop.substr(pos_name, len_name) });
                }
                return result;
            }
        }
    }
}