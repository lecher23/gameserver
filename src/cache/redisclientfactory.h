#ifndef REDISCLIENTFACTORY_H
#define REDISCLIENTFACTORY_H

#include <util/common_define.h>
#include <cache/RedisClient.hpp>

BEGIN_NAMESPACE(cgserver)

class RedisClientFactory{
public:
  RedisClientFactory() {}
  ~RedisClientFactory() {}

  static CRedisClient &getClient() {
    static CRedisClient client;
    return client;
  }
};

END_NAMESPACE
#endif
