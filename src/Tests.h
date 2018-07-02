#ifndef TESTS_H
#define TESTS_H

#include "HttpParser.h"

#include <chrono>

// todo: random test of chunked http parsing of randomly generated requests
void testHttpParserPerformance() {

    char headers[] = "GET /hello.htm HTTP/1.1\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
            "Host: www.tutorialspoint.com\r\n"
            "Accept-Language: en-us\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: Keep-Alive\r\n"
            "Content-length: 1048576\r\n\r\n";

    const int requestLength = sizeof(headers) - 1 + 1048576;
    char *request = (char *) malloc(requestLength + 32);
    memset(request, 0, requestLength);
    memcpy(request, headers, sizeof(headers) - 1);

    char *data = (char *) malloc(requestLength * 10);
    int length = requestLength * 10;
    //int currentOffset = 0;


    int maxChunkSize = 10000;
    char *paddedBuffer = (char *) malloc(maxChunkSize + 32);

    // dela upp dessa 10 i 5 segment
    HttpParser httpParser;
    int validRequests = 0;
    size_t dataBytes = 0;

    for (int i = 0; i < 10; i++) {
        memcpy(data + requestLength * i, request, requestLength);
    }

    for (int j = 0; j < 1000; j++) {
        for (int currentOffset = 0; currentOffset != length; ) {

            int chunkSize = rand() % 10000;
            if (currentOffset + chunkSize > length) {
                chunkSize = length - currentOffset;
            }

            // kpiera skiten
            memcpy(paddedBuffer, data + currentOffset, chunkSize);

            httpParser.consumePostPadded(/*data + currentOffset*/ paddedBuffer, chunkSize, nullptr, [&validRequests](void *user, HttpRequest *req) {

                validRequests++;
                std::cout << "validRequests: " << validRequests << std::endl;


            }, [&dataBytes](void *, std::string_view data) {
                //std::cout << "data bytes: " << data.length() << std::endl;
                dataBytes += data.length();
            }, [](void *) {

                std::cout << "Error!" << std::endl;
                return;
            });

            currentOffset += chunkSize;
        }
    }

    std::cout << "Data bytes: " << dataBytes << std::endl;

    /*auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; i++) {
        httpParser.consumePostPadded(data, sizeof(data) - 2, nullptr, [&validRequests](void *user, HttpRequest *req) {
            validRequests++;
        }, [](void *, std::string_view data) {

        }, [](void *) {

        });
    }
    auto stop = std::chrono::high_resolution_clock::now();*/

    //std::cout << "Parsed " << validRequests << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << std::endl;
}

#endif // TESTS_H