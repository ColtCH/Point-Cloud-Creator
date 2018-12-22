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
void PointCloud::constructPointsFromPacket(DataPacket dp, int cutoff){
  /* For debugging, uncomment this line and corresponding end brace. */
  //for (int to = 0; to < cutoff; to++) {
    for (int block = 0; block < 12; block++) {
      /* sequence that will be updated with new points. There will be
      12 total sequences created, each of which contain 32 points. */
      FiringSequence sequence;
      /* Azimuth for the block is used for each channel, so keep this. */
      uint16_t this_azimuth = dp.blocks[block].azimuth;
      for (int channel = 0; channel < 32; channel++) {
        /* Save this channels distance. */
        uint16_t dist = dp.blocks[block].channels[channel].distance;

        /* Grab the elevation calculation. */
        uint16_t elev = PointCloud::laser_elevation[channel % 16];

        /* Compute our cartesian points. */
        double _x = dist * cos(elev) * sin(this_azimuth);
        double _y = dist * cos(elev) * cos(this_azimuth);
        double _z = dist * sin(elev);

        /* Apply vertical correction to the points. */
        _x *= PointCloud::corrections[channel % 16];
        _y *= PointCloud::corrections[channel % 16];
        _z *= PointCloud::corrections[channel % 16];

        /* Save this coordinate into the sequence. */
        sequence.points[channel] = Coordinate(_x, _y, _z);

        debugfile << _x << ' ' << _y << ' ' << _z << endl;



        //cout << setw(15) << _x << setw(15) << _y << setw(15) << _z << endl;
      }
      /* Push the sequence into our vector of firing sequences. */
      firing_sequences.push_back(sequence);
    }
  //}
}

void PointCloud::initDebugFile(){
  debugfile.open("debug.txt");
}
