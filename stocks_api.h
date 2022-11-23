#ifndef StocksApi_h
#define StocksApi_h

#include <HTTPClient.h>
#include <rpcWiFi.h>

class StocksApi {

  public:
    bool connect();
    bool isConnected();
    String httpGET(String url);
};

#endif
