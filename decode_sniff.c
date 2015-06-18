#include <pcap.h>
#include "hacking.h"
#include "hacking-network.h"

void pcap_fatal(const char *, const char *);
void decode_ethernet(const u_char *);
void decode_ip(const u_char *);
u_int decode_tcp(const u_char *);

void caught_packet(u_char *, const struct pcap_pkthdr *, const u_char *);

int main() {
  struct pcap_pkthdr cap_header;
  const u_char *packet, *pkt_data;
  char errbuf[PCAP_ERRBUF_SIZE];
  char *device;
  pcap_t *pcap_handle;

  device = pcap_lookupdev(errbuf);
  if(device == NULL)
    pcap_fatal("pcap_lookupdev", errbuf);

  printf("Sniffing on device %s\n", device);

  pcap_handle = pcap_open_live(device, 4096, 1, 0, errbuf);
  if (pcap_handle == NULL)
    pcap_fatal("pcap_open_live", errbuf);

  pcap_loop(pcap_handle, 3, caught_packet, NULL);

  pcap_close(pcap_handle);
}

void caught_packet(u_char *user_args, const struct pcap_pkthdr *cap_header, const u_char *packet) {
  int tcp_header_length, total_header_size, pkt_data_len;
  u_char *pkt_data;

  printf("===== Got a %d byte packet =====\n", cap_header->len);

  decode_ethernet(packet);
  decode_ip(packet+ETHR_HDR_LEN);
  tcp_header_length = decode_tcp(packet+ETHER_HDR_LEN+sizeof(struct ip_hdr));

  total_header_size = ETHER_HDR_LEN+sizeof(struct ip_hdr)+tcp_header_length;
  pkt_data = (u_char *)packet + total_header_size; // points to data portion
  pkt_data_len = cap_header->len - total_header_size;
  if(pkt_data_len > 0) {
    printf("\t\t\t%u bytes of packet data\n", pkt_data_len);
    dump(pkt_data, pkt_data_len);
  } else {
    printf("\t\t\tNo Packet Data\n");
  }
}

void pcap_fatal(const char *failed_in, const char *errbuf) {
  printf("Fatal Error in %s: %s\n", failed_in, errbuf);
  exit(1);
}

void decode_ethernet(const u_char *header_start) {
  int i;
  const struct ether_hdr *ethernet_header;

  ethernet_header
