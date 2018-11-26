#include "pointcloud.h"

void PointCloud::constructPointsFromPacket(DataPacket dp){
  for (int block = 0; block < 12; block++) {
    FiringSequence sequence;
    uint16_t this_azimuth = dp.blocks[block].azimuth;
    for (int channel = 0; channel < 32; channel++) {
      uint16_t dist = dp.blocks[block].channels[channel].distance;
      uint16_t elev = PointCloud::laser_elevation[channel % 16];
      double _x = dist * cos(elev) * sin(this_azimuth);
      double _y = dist * cos(elev) * cos(this_azimuth);
      double _z = dist * sin(elev);
      _x *= PointCloud::corrections[channel % 16];
      _y *= PointCloud::corrections[channel % 16];
      _z *= PointCloud::corrections[channel % 16];
      sequence.points[channel] = Coordinate(_x, _y, _z);
    }
    firing_sequences.push_back(sequence);
  }
}
