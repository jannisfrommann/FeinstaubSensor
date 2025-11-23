#ifndef API_CLIENT_H
#define API_CLIENT_H

#include "config.h"


class ApiClient {
public:
    ApiClient() {}
    void sendData(float pm10, float pm25); // sends only PM-values (pm25, pm10) to API_URL
};

extern ApiClient apiClient;
#endif
