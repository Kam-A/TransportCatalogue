#include <iostream>
#include <tuple>
#include <vector>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

void RequestProcess(transport_catalogue::TransportCatalogue& catalogue, istream& input, ostream& output) {
    using namespace transport_catalogue;
    input_reader::RequestProcess(catalogue, input);
    stat_reader::RequestProcess(catalogue, input, output);
}

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    RequestProcess(catalogue, cin, cout);
    return 0;
}
