#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_

#include <cstdint>
#include <pcap.h>
#include <iostream>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <vector>
#include <cmath>

struct __attribute__((packed)) Channel {
  uint16_t distance;
  uint8_t reflectivity;
};

struct __attribute__((packed)) DataBlock {
  uint16_t flag;
  uint16_t azimuth;
  Channel channels[32];
};

struct __attribute__((packed)) DataPacket {
  uint8_t header[42];
  DataBlock blocks[12];
  uint32_t timestamp;
  uint16_t factory;
};

struct Coordinate {
  double x, y, z;
  Coordinate(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
  Coordinate() :x(0), y(0), z(0) {}
};

struct FiringSequence {

  Coordinate points[32];
};

using namespace std;

class PointCloud {
  private:
    vector<FiringSequence> firing_sequences;
    int laser_elevation[16] = {-15, 1, -13, 3,
                               -11, 5, -9,  7,
                               -7,  9, -5,  11,
                               -3, 13, -1,  15};

    double corrections[16] =  {11.2, -0.7, 9.7, -2.2,
                               8.1,  -3.7, 6.6, -5.1,
                               5.1,  -6.6, 3.7, -8.1,
                               2.2,  -9.7, 0.7, -11.2};
  public:
    void constructPointsFromPacket(DataPacket dp);
};

#endif
