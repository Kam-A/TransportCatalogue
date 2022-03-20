#pragma once

#include <string>

#include "map_renderer.h"
#include "transport_router.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

class Serializer {
public:
    Serializer(TransportCatalogue& db, renderer::MapRenderer& renderer, TransportRouter& router) : db_(db), renderer_(renderer), router_(router) {
    }
    void SerializeBaseToFile();
    void DeserializeBaseFromFile();
    void SetSettings(std::string file_name);
private:
    TransportCatalogue& db_;
    renderer::MapRenderer& renderer_;
    TransportRouter& router_;
    std::string file_name_;
};
}
