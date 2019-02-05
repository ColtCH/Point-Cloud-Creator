#include "pointcloud.h"

PointCloud points;
int cutoff = 500;
int to = 0;

void print_packet_info(const u_char *packet, struct pcap_pkthdr packet_header) {
    printf("Packet capture length: %d\n", packet_header.caplen);
    printf("Packet total length %d\n", packet_header.len);
}

/* Yes, this is super tedious. But it is incredibly important that all of these
bytes get read in correctly. Therefore, this section is some straight up no
frills annoying as shit byte allocation. */
void read_into_data(const u_char *packet, struct pcap_pkthdr packet_header) {

    DataPacket temp;
    int offset = 0; //continually update in order to parse these correctly.

    /* Read the header in. */
    for (int i = 0; i < 42; i++) temp.header[i] = packet[i];

    offset += 42;

    /* Read the DataBlocks in. */
    for (int i = 0; i < 12; i++) {
      /* Here we go. Grab the flag for a given block, offsetting the header. */
      temp.blocks[i].flag = uint16_t((packet[offset + 0]) |
                                     (packet[offset + 1]) << 8);

      temp.blocks[i].azimuth = uint16_t((packet[offset + 2]) |
                                        (packet[offset + 3]) << 8);
      offset += 4;

      /* Figure out what a full packet is in timestamps */

      //if (temp.blocks[i].azimuth == 16263) {
        //cout << temp.timestamp << endl;
      //}

      //if (temp.blocks[i].azimuth == 16223) {
        //cout << temp.timestamp << endl;
      //}
      /* Read the Channels in. */
      for (int j = 0; j < 32; j++) {

        temp.blocks[i].channels[j].distance = uint16_t((packet[offset + 0]) |
                                                       (packet[offset + 1]) << 8);

        /* Granularity is 2mm. */
        temp.blocks[i].channels[j].distance *= 2;

        //cout << temp.blocks[i].channels[j].distance << endl;

        temp.blocks[i].channels[j].reflectivity = packet[offset + 2];

        offset += 3;
      }
    }
    /* Get the timestamp and factory. */
    temp.timestamp = int((packet[1242]) |
                         (packet[1243]) << 8 |
                         (packet[1244]) << 16|
                         (packet[1245]) << 24);

    //cout << temp.timestamp << endl;

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

    /*DEBUG, COMMENT FOR REAL RUNS*/
    to++;
    if (to == cutoff) exit(0);

    return;
}

int main(int argc, char const *argv[]) {

  if (argc < 2) {
    cout << "Correct usage: ./PCAP <file_here.pcap> " << endl;
    return 0;
  }

  pcap_t* velodyne_file;
  struct pcap_pkthdr packet_header;
  const u_char *packet;
  points.initDebugFile();

  char errbuf[PCAP_ERRBUF_SIZE]; //Receives any error message given by the callback.

  velodyne_file = pcap_open_offline(argv[1], errbuf);

  if (velodyne_file == NULL) {
    cout << "pcap_open_live() failed: " << errbuf << endl;
    return 1;
  }

  pcap_loop(velodyne_file, 0, my_packet_handler, NULL);


  cout << "File has been imported.";



  return 0;
}
