#include "pointcloud.h"

PointCloud points;

void print_packet_info(const u_char *packet, struct pcap_pkthdr packet_header) {
    printf("Packet capture length: %d\n", packet_header.caplen);
    printf("Packet total length %d\n", packet_header.len);
}
/* */
void read_into_data(const u_char *packet, struct pcap_pkthdr packet_header) {
    DataPacket temp;
    int offset = 0; //continually update in order to parse these correctly.
    /* Read the header in. */
    for (int i = 0; i < 42; i++) temp.header[i] = packet[i];
    offset += 42;
    /* Read the DataBlocks in. */
    for (int i = 0; i < 12; i++) {
      /* Grab the flag and azimuth for a given block, offsetting the header. */
      temp.blocks[i].flag    = uint16_t((packet[offset + 0]) |
                                        (packet[offset + 1]) << 8);
      temp.blocks[i].azimuth = uint16_t((packet[offset + 2]) |
                                        (packet[offset + 3]) << 8);
      offset += 4;
      /* Read the Channels in. */
      for (int j = 0; j < 32; j++) {
        /* Grab distance and reflectivity for each channel. */
        temp.blocks[i].channels[j].distance = uint16_t((packet[offset + 0]) |
                                                       (packet[offset + 1]) << 8);
        /* Granularity is 2mm. */
        temp.blocks[i].channels[j].distance *= 2;
        temp.blocks[i].channels[j].reflectivity = packet[offset + 2];
        offset += 3;
      }
    }
    /* Get the timestamp and factory. */
    temp.timestamp =    int((packet[1242]) |
                            (packet[1243]) << 8 |
                            (packet[1244]) << 16|
                            (packet[1245]) << 24);
    temp.factory = uint16_t((packet[1246]) |
                            (packet[1247]) << 8);
    /* Construct the points from this parsed data. */
    points.constructPointsFromPacket(temp);
  }
/*
  Callback when packet_body is full. Checks if caplen is correct: if incorrect,
  skips.
*/
void my_packet_handler(u_char *args,
                       const struct pcap_pkthdr *packet_header,
                       const u_char *packet_body) {
    //For now, skip the GPS packets.
    if (packet_header->len != 1248) return;
    read_into_data(packet_body, *packet_header);
    return;
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cout << "Correct usage: ./PCAP <file_here.pcap> " << std::endl;
    return 0;
  }
  /* libpcap style FILE handle. */
  pcap_t* velodyne_file;
  /* Receives any error message given by the callback. */
  char errbuf[PCAP_ERRBUF_SIZE];
  /* For now, open offline. Will use network in future. */
  velodyne_file = pcap_open_offline(argv[1], errbuf);
  if (velodyne_file == NULL) {
    std::cout << "pcap_open_live() failed: " << errbuf << std::endl;
    return 1;
  }
  pcap_loop(velodyne_file, 0, my_packet_handler, NULL);
  return 0;
}
