#include <cstdint>
#include <pcap.h>
#include <iostream>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <vector>

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

using namespace std;

vector<DataPacket> data;

void print_packet_info(const u_char *packet, struct pcap_pkthdr packet_header) {
    printf("Packet capture length: %d\n", packet_header.caplen);
    printf("Packet total length %d\n", packet_header.len);
}

/* Yes, this is super tedious. But it is incredibly important that all of these
bytes get read in correctly. Therefore, this section is some straight up no
frills annoying as shit byte allocation. */
void read_into_data(const u_char *packet, struct pcap_pkthdr packet_header) {
    /* No clue what to do with the weirdly truncated packages. For now, skip. */
    if (packet_header.len != packet_header.caplen ||
        packet_header.caplen < 1248) {
      DataPacket temp;
      data.push_back(temp);
      return;
    }

    DataPacket temp;

    int offset = 0;
    int width_of_datablock = 100;
    int width_of_channels = 3;
    /* Read the header in. */
    for (int i = 0; i < 42; i++) temp.header[i] = packet[i];

    /* Read the DataBlocks in. */
    for (int i = 0; i < 12; i++) {
      /* Here we go. Grab the flag for a given block, offsetting the header. */
      offset = (i*width_of_datablock) + 42;
      temp.blocks[i].flag = uint16_t((packet[offset + 0]) << 8 |
                                     (packet[offset + 1]));

      temp.blocks[i].azimuth = uint16_t((packet[offset + 2]) << 8 |
                                        (packet[offset + 3]));

      cout << temp.blocks[i].azimuth << endl;
      /* Read the Channels in. */
      for (int j = 0; j < 32; j++) {
        offset = packet[42 + (i*width_of_datablock) +4+ (j*width_of_channels)];
        temp.blocks[i].channels[j].distance = uint16_t((packet[offset + 0]) << 8 |
                                                       (packet[offset + 1]));

        temp.blocks[i].channels[j].reflectivity = packet[offset + 2];
      }
    }
    /* Get the timestamp and factory. */
    //1241
    temp.timestamp = int((packet[1242]) << 24 |
                         (packet[1243]) << 16 |
                         (packet[1244]) << 8  |
                         (packet[1245]));

    temp.factory = uint16_t((packet[1246]) << 8 |
                            (packet[1247]));


    data.push_back(temp);
    //cout << data.size() << endl;
  }


void my_packet_handler(u_char *args,
                       const struct pcap_pkthdr *packet_header,
                       const u_char *packet_body) {
    //print_packet_info(packet_body, *packet_header);
    /* What do we know at this point?
       packet_body is containing our buffer of values.
       We need to move this buffer over to our structs.
       Maybe some sort of read function that takes the buffer? */

    read_into_data(packet_body, *packet_header);

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
