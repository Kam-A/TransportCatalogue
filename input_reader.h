#pragma once

#include <vector>
#include <string>
#include <string_view>

namespace transport_catalogue{
namespace detail{

struct Requests {
    std::vector<std::string> buses;
    std::vector<std::string> stops;
};

enum class RequestType {
    BUS,
    STOP
};

Requests ReadInputRequest();
RequestType GetReqType(std::string_view);
int ReadLineWithNumber();

}
}
