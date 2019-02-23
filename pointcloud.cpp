#include "pointcloud.h"
/*
=============================================================================
Adds to the vector of coordinates inside of the object PointCloud when given
a data packet.

@pre  dp != NULL
      this != NULL
      size of datapacket = 1248

@post dp = #dp
points inside of PointCloud will contain 384 (12 * 32) more points.
=============================================================================
*/
void PointCloud::constructPointsFromPacket(DataPacket &dp){
  /* For debugging, uncomment this line and corresponding end brace. */
  //for (int to = 0; to < cutoff; to++) {
    for (int block = 0; block < 12; block++) {
      /* sequence that will be updated with new points. There will be
      12 total sequences created, each of which contain 32 points. */
      FiringSequence sequence;
      /* Azimuth for the block is used for each channel, so keep this. */
      uint16_t this_azimuth = dp.blocks[block].azimuth / 100;
      for (int channel = 0; channel < 32; channel++) {
        /* Save this channels distance. */
        uint16_t dist = dp.blocks[block].channels[channel].distance;
        /* Grab the elevation calculation. */
        int16_t elev = PointCloud::laser_elevation[channel % 16];
        /* Compute our cartesian points. */
        double _x =  dist * cos(elev * (PointCloud::pi / 180.0)) *
                    sin(this_azimuth * (PointCloud::pi / 180.0));
        double _y  = dist * cos(elev * (PointCloud::pi / 180.0)) *
                    cos(this_azimuth * (PointCloud::pi / 180.0));
        double _z =  dist * sin(elev * (PointCloud::pi / 180.0));
        /* Catch the failed laser executions? Not sure why they are so
           frequent. Will look into later. */
        if (_x == 0 && _y == 0 && _z == 0) continue;
        /* Apply vertical correction to the points. */
        _x += PointCloud::corrections[channel % 16];
        _y += PointCloud::corrections[channel % 16];
        _z += PointCloud::corrections[channel % 16];
        /* Keep this in case it is needed for later. Not currently in use. */
        //sequence.points[channel] = Coordinate(_x, _y, _z);
        /* Since this layer saves nothing, output into the program that
           will sit on top of this one. */
        std::cout << _x           << ' ' << std::setw(10)
                  << _y           << ' ' << std::setw(10)
                  << _z           << ' ' << std::setw(10)
                  << this_azimuth << ' ' << std::setw(10)
                  << dist         << ' ' << std::setw(10)
                  << elev         << ' ' << std::setw(10)
                  << (channel%16)        << std::setw(10) << std::endl;
      }
    }
}
void PointCloud::initDebugFile(){
  debugfile.open("debug.txt");
}
