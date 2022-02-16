
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "svg.h"
#include "transport_catalogue.h"

using namespace std;

void RequestProcess(transport_catalogue::TransportCatalogue& catalogue, istream& input, ostream& output) {
    using namespace transport_catalogue;
    renderer::MapRenderer renderer;
    TransportRouter router(catalogue);
    request_handler::RequestHandler request_handler(catalogue, renderer, router);
    json_reader::RequestProcess(catalogue, input, output, renderer, router, request_handler);
    
}

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    RequestProcess(catalogue, cin, cout);
    return 0;
}
