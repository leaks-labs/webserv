#include "HttpHeader.hpp"
//todo improve struct
const HttpHeaderList header_dictionary[N_HEADERS] = {
        {"Content-Type", {}},
        {"Authorization", {}},
        {"Content-Length", {"1024"}},
        {"User-Agent", {}},
        {"Cache-Control", {}},
        {"Host", {}},
        {"Accept", {}},
        {"Accept-Encoding", {"gzip", "deflate", "br"}},
        {"Accept-Language", {"en-US", "fr-FR", "es-ES"}},
        {"Cookie", {}},
};

HttpHeader::HttpHeader()
{
}

HttpHeader::~HttpHeader()
{
}

const std::map<std::string, std::string>& HttpHeader::get_header_map() const
{
    return header_map_;
}

size_t HttpHeader::FindHeader(
        const HttpHeaderList *dictionary,
        const std::string &header)
{
    size_t i;
    for (i = 0; i < N_HEADERS; ++i) {
        if (dictionary[i].header == header)
            break;
    }
    return i;
}