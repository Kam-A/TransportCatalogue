#include "transport_catalogue.h"
#include <unordered_set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <execution>

namespace transport_catalogue{

using namespace std;

string TransportCatalogue::GetNameFromReq(string_view& text) {
    // find name
    int64_t space = text.find(' ');
    text.remove_prefix(space + 1);
    int64_t name_end = text.find(':');
    string stop_name = string(text.substr(0,name_end));
    text.remove_prefix(name_end + 2);
    return stop_name;
}

TransportCatalogue::Stop TransportCatalogue::GetStopInfoFromReq(string_view text) {
    string stop_name = GetNameFromReq(text);
    // find latidute
    int64_t comma_pos = text.find(',');
    double lat = stod(string(text.substr(0, comma_pos)));
    text.remove_prefix(comma_pos + 2);
    // find longitude
    double lng = stod(string(text.substr(0)));
    return {stop_name, lat, lng};
}

void TransportCatalogue::AddStop(string_view request){
    stops_.push_back(GetStopInfoFromReq(request));
    pointers_to_stops_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddDistBwStops(string_view request) {
    string stop_name = GetNameFromReq(request);
    const Stop* src_stop = GetStopByName(stop_name);
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
            const Stop* dst_stop = GetStopByName(dst_stop_name);
            dst_bw_stops_[{src_stop, dst_stop}] = dist;
        }
            
    }
    
}

vector<string_view> TransportCatalogue::GetStopByDelim(string_view str, char sym) {
    vector<string_view> res;
    const int64_t pos_end = str.npos;
    while(true){
        int64_t delim = str.find(sym);
        res.push_back(str.substr(0,delim - 1));
        if (delim == pos_end) {
            break;
        } else {
            str.remove_prefix(delim + 2);
        }
    }
    return res;
}

vector<string_view> TransportCatalogue::GetAllStops(string_view str) {
    if (str.find('>') != str.npos) {
        return GetStopByDelim(str,'>');
    } else {
        vector<string_view> res = GetStopByDelim(str,'-');
        for(int i = static_cast<int>(res.size() - 2); i >= 0;--i) {
            res.push_back(res[i]);
        }
        return res;
    }
}

TransportCatalogue::Bus TransportCatalogue::GetBusInfoFromReq(string_view text) {
    TransportCatalogue::Bus res;
    res.name = GetNameFromReq(text);
    res.real_route_len = 0;
    res.route_len = 0;
    unordered_set<string_view> unique_stops;
    for (const string_view bus_stop : GetAllStops(text)) {
        res.stops.push_back(GetStopByName(bus_stop));
        unique_stops.insert(bus_stop);
    }
    res.uniq_stop_number = unique_stops.size();
    for(int i = 0; i < res.stops.size() - 1; ++i){
        res.route_len += ComputeDistance(res.stops[i]->coord,
                                res.stops[i + 1]->coord);
        res.real_route_len += GetDistByStopPair(res.stops[i], res.stops[i + 1]);
        
    }
    return res;
}

void TransportCatalogue::AddBus(std::string_view request){
    buses_.push_back(GetBusInfoFromReq(request));
    pointers_to_buses_[buses_.back().name] = &buses_.back();
}

std::optional<std::vector<std::string_view>> TransportCatalogue::GetBusesByStop(string_view text) {
    if (pointers_to_stops_.count(text) == 0) {
        return {};
    } else {
        vector<string_view> res;
        for_each(pointers_to_buses_.begin(), pointers_to_buses_.end(),
                 [text, &res](const auto value) {
                for(const auto stop : value.second->stops){
                    if ( stop->name == text) {
                        res.push_back(value.second->name);
                        break;
                    }
                }
        });
        return res;
    }
}

const TransportCatalogue::Stop* TransportCatalogue::GetStopByName(std::string_view stop_name){
    return pointers_to_stops_[stop_name];
}

const TransportCatalogue::Bus* TransportCatalogue::GetBusByName(std::string_view bus_name){
    if (pointers_to_buses_.count(bus_name) != 0) {
        return pointers_to_buses_[bus_name];
    } else {
        return nullptr;
    }
}

int64_t TransportCatalogue::GetDistByStopPair(const Stop* src,const Stop* dst) {
    if (dst_bw_stops_.count({src,dst}) !=0){
        return dst_bw_stops_[{src,dst}];
    } else {
        return dst_bw_stops_[{dst,src}];
    }
}

}
