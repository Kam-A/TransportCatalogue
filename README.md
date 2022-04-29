# TransportCatalogue

City Routing system, processes JSON-requests. Two-staged. First stage - creating database with routes. Second stage - request for info to database. Output: json and svg(rendered stops, buses and path)

## Features: 
 - input: json-file(self-implemented json-constructor)
 - path-rendering as SVG-file
 - serialize deserialize by protobuf
 
## Usage:

Example of request in file requests.json

## To do:

 - add another type of transport

## Tech:

 - CMake 3.22.3
 - Protobuf-cpp 3.18.1
