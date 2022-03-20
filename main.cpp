
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "svg.h"
#include "serialization.h"
#include "transport_catalogue.h"


using namespace std;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

void MakeBaseProcess() {
    using namespace transport_catalogue;
    TransportCatalogue catalogue;
    renderer::MapRenderer renderer;
    TransportRouter router(catalogue);
    Serializer serialiser(catalogue, renderer, router);
    json_reader::MakeBaseRequestProcess(catalogue, cin, renderer, router, serialiser);
}

void ProcessRequestProcess() {
    using namespace transport_catalogue;
    TransportCatalogue catalogue;
    renderer::MapRenderer renderer;
    TransportRouter router(catalogue);
    request_handler::RequestHandler request_handler(catalogue, renderer, router);
    Serializer serialiser(catalogue, renderer, router);
    json_reader::FromDbRequestProcess(catalogue, cin, cout, request_handler, router, serialiser);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        MakeBaseProcess();
    } else if (mode == "process_requests"sv) {
        ProcessRequestProcess();
    } else {
        PrintUsage();
        return 1;
    }
}
