This is a dummy Redis copy made in C++ following
["Build Your Own Redis" Challenge](https://codecrafters.io/challenges/redis).

To put it running, you can run `./spawn_redis.sh` and it will start this dummy C++ redis.

After that, you can connect your redis client to `localhost:6379` and start playing with it.

Actually it only works in-memory without storing anything in disk.

## Commands implemented

- [x] `PING`
- [x] `SET` with `PX` option
- [x] `GET`
