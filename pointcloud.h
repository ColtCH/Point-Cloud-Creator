#ifndef _POINTCLOUD_H_
#define _POINTCLOUD_H_
/*Includes...............*/
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
#include <iomanip>
#include <fstream>
/*.......................*/
/*====================== RAW BINARY DATA STRUCTS =============================*/
/*------------------CHANNEL-------------------*/
struct __attribute__((packed)) Channel {
  uint16_t distance;
  uint8_t reflectivity;
};
/*-----------------DATABLOCK------------------*/
struct __attribute__((packed)) DataBlock {
  uint16_t flag;
  uint16_t azimuth;
  Channel channels[32];
};
/*----------------DATAPACKET------------------*/
struct __attribute__((packed)) DataPacket {
  uint8_t header[42];
  DataBlock blocks[12];
  uint32_t timestamp;
  uint16_t factory;
};
/*====================== POINT CLOUD FORMAT STRUCTS ==========================*/
/*---------------COORDINATE------------------*/
struct Coordinate {
  double x, y, z;
  Coordinate(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
  Coordinate() :x(0), y(0), z(0) {}
};
/*-------------FIRINGSEQUENCE----------------*/
struct FiringSequence {
  Coordinate points[32];
};
/*============================================================================*/
/*============ POINT CLOUD OBJECT FOR CONVERSION FROM BINARY DATA ============*/
class PointCloud {
  private:
    std::ofstream debugfile;
    double pi = 3.141592653589;
    /* Laser elevations for lasers 0 - 15, as reported from manual. */
    int laser_elevation[16] = {-15, 1, -13, 3,
                               -11, 5, -9,  7,
                               -7,  9, -5,  11,
                               -3, 13, -1,  15};
    /* Laser corrections for lasers 0 - 15, as reported from manual. */
    double corrections[16] =  {11.2, -0.7, 9.7, -2.2,
                               8.1,  -3.7, 6.6, -5.1,
                               5.1,  -6.6, 3.7, -8.1,
                               2.2,  -9.7, 0.7, -11.2};
  public:
    void constructPointsFromPacket(DataPacket &dp);
    void initDebugFile();
};
#endif
