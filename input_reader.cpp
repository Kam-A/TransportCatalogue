#include <iostream>

#include "input_reader.h"

namespace transport_catalogue {

using namespace std;

namespace detail {

string ReadLine(istream& input) {
    string s;
    getline(input, s);
    return s;
}

int ReadLineWithNumber(istream& input) {
    int result;
    input >> result;
    ReadLine(input);
    return result;
}

}

namespace input_reader {

string_view GetHeader(string_view request) {
    int64_t space = request.find(' ');
    return request.substr(0,space);
}

RequestType GetRequestType(string_view request) {
    string_view request_header = GetHeader(request);
    if (request_header == "Bus"s) {
        return RequestType::BUS;
    } else if (request_header == "Stop"s) {
        return RequestType::STOP;
    }
    return RequestType::UNKNOWN;
}


string GetNameFromRequest(string_view& request) {
    // find name
    int64_t space = request.find(' ');
    request.remove_prefix(space + 1);
    int64_t name_end = request.find(':');
    string stop_name = string(request.substr(0,name_end));
    request.remove_prefix(name_end + 2);
    return stop_name;
}

TransportCatalogue::Stop GetStopFromRequest(string_view request) {
    string stop_name = GetNameFromRequest(request);
    // find latidute
    int64_t comma_pos = request.find(',');
    double lat = stod(string(request.substr(0, comma_pos)));
    request.remove_prefix(comma_pos + 2);
    // find longitude
    double lng = stod(string(request.substr(0)));
    return {stop_name, lat, lng};
}

vector<string_view> GetStopByDelimiter(string_view request, char delimiter) {
    vector<string_view> res;
    const int64_t pos_end = request.npos;
    while(true) {
        int64_t delimiter_position = request.find(delimiter);
        res.push_back(request.substr(0, delimiter_position - 1));
        if (delimiter_position == pos_end) {
            break;
        } else {
            request.remove_prefix(delimiter_position + 2);
        }
    }
    return res;
}

vector<string_view> GetAllStops(string_view request) {
    if (request.find('>') != request.npos) {
        return GetStopByDelimiter(request, '>');
    } else {
        vector<string_view> res = GetStopByDelimiter(request,'-');
        for(int i = static_cast<int>(res.size() - 2); i >= 0; --i) {
            res.push_back(res[i]);
        }
        return res;
    }
}

TransportCatalogue::Bus GetBusFromRequest(TransportCatalogue& catalogue, string_view request) {
    TransportCatalogue::Bus res;
    res.name = GetNameFromRequest(request);
    for (const string_view bus_stop : GetAllStops(request)) {
        res.stops.push_back(catalogue.GetStopByName(bus_stop));
    }
    return res;
}

void RequestProcess(TransportCatalogue& catalogue, istream& input) {
    Requests requests;
    int request_num = detail::ReadLineWithNumber(input);
    string request;
    for(int i = 0; i < request_num; ++i) {
        getline(input, request);
        RequestType request_type = GetRequestType(request);
        if(request_type == RequestType::BUS) {
            requests.buses.push_back(move(request));
        } else if (request_type == RequestType::STOP) {
            catalogue.AddStop(GetStopFromRequest(request));
            requests.stops.push_back(move(request));
        }
    }
    for(const auto& add_stop_request : requests.stops) {
        SetRealDistanceForStopFromRequest(catalogue, add_stop_request);
    }
    for(const auto& add_bus_request : requests.buses) {
        catalogue.AddBus(GetBusFromRequest(catalogue, add_bus_request));
    }
}

void SetRealDistanceForStopFromRequest(TransportCatalogue& catalogue, string_view request) {
    string stop_name = GetNameFromRequest(request);
    const TransportCatalogue::Stop* src_stop = catalogue.GetStopByName(stop_name);
    int64_t comma_pos = request.find(',');
    request.remove_prefix(comma_pos + 2);
    comma_pos = request.find(',');
    if (comma_pos != request.npos) {
        request.remove_prefix(comma_pos + 2);
        while(comma_pos != request.npos) {
            // find end of dist
            int64_t m_pos = request.find('m');
            // get distance as int64
            int64_t dist = stoi(string(request.substr(0,m_pos)));
            // move to station
            request.remove_prefix(m_pos + 5);
            // find end of name or end of request
            comma_pos = request.find(',');
            string_view dst_stop_name = request.substr(0,comma_pos);
            request.remove_prefix(comma_pos + 2);
            const TransportCatalogue::Stop* dst_stop = catalogue.GetStopByName(dst_stop_name);
            catalogue.SetDistanceBetweenStops(src_stop, dst_stop, dist);
        }
            
    }
}
}
}
