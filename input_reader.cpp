#include "input_reader.h"
#include <iostream>

namespace transport_catalogue{
namespace detail{

using namespace std;

string_view GetHeader(string_view req){
    int64_t space = req.find(' ');
    return req.substr(0,space);
}

RequestType GetReqType(string_view req){
    if (GetHeader(req) == "Bus"s){
        return RequestType::BUS;
    } else {
        return RequestType::STOP;
    }
}

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

Requests ReadInputRequest(){
    Requests res;
    int req_num = ReadLineWithNumber();
    string req;
    for(int i = 0;i < req_num; ++i) {
        getline(cin, req);
        if(GetReqType(req) == RequestType::BUS){
            res.buses.push_back(move(req));
        } else {
            res.stops.push_back(move(req));
        }
    }
    return res;
}

}
}
