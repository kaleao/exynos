//#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define DST_MAC0	0x00
#define DST_MAC1	0x00
#define DST_MAC2	0x00
#define DST_MAC3	0x00
#define DST_MAC4	0x00
#define DST_MAC5	0x00

#define SRC_MAC0	0x00
#define SRC_MAC1	0x00
#define SRC_MAC2	0x00
#define SRC_MAC3	0x00
#define SRC_MAC4	0x00
#define SRC_MAC5	0x00


#define DEFAULT_IF	"eth0"
#define BUF_SZ	1024

#define PAYLOAD_SZ  32


char frame[BUF_SZ];

uint16_t csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main(int argc, char **argv)
{
    uint32_t i = 0;
    int sockfd = 0;
    struct ifreq if_idx;
    struct ifreq if_mac;
    int tx_len = 0;    
    struct ether_header *eh = NULL;
    struct iphdr *iph = NULL;
    struct udphdr *udph = NULL;
    
    struct sockaddr_ll socket_address;
    
    
    sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if(sockfd == -1){perror("socket");}
    
    memset(&if_idx, 0 sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, DEFAULT_IF, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0){perror("SIOCGIFINDEX");}
    
//     memset(&if_mac, 0, sizeof(struct ifreq));
//     strncpy(if_mac.ifr_name, DEFAULT_IF, IFNAMSIZ-1);
//     if (ioctl(sock, SIOCGIFHWADDR, &if_mac) < 0){perror("SIOCGIFHWADDR");}
        
    memset(frame, 0, BUF_SZ);
    
    /*ETH header*/
    eh = (struct ether_header *)frame;
    eh->ether_shost[0] = SRC_MAC0;
    eh->ether_shost[1] = SRC_MAC1;
    eh->ether_shost[2] = SRC_MAC2;
    eh->ether_shost[3] = SRC_MAC3;
    eh->ether_shost[4] = SRC_MAC4;
    eh->ether_shost[5] = SRC_MAC5;
    
    eh->ether_dhost[0] = DST_MAC0;
    eh->ether_dhost[1] = DST_MAC1;
    eh->ether_dhost[2] = DST_MAC2;
    eh->ether_dhost[3] = DST_MAC3;
    eh->ether_dhost[4] = DST_MAC4;
    eh->ether_dhost[5] = DST_MAC5;
    
    eh->ether_type = htons(ETH_P_IP);
    tx_len += sizeof(struct ether_header);
    
    /*IP header*/
    iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16; // Low delay
    iph->id = htons(54321);
    iph->ttl = ttl; // hops
    iph->protocol = 17; // UDP
    iph->saddr = inet_addr("192.168.3.101");
    iph->daddr = inet_addr("192.168.3.32");
    tx_len += sizeof(struct iphdr);
    
    /*UDP header*/
    udph = (struct udphdr *) (sendbuf + sizeof(struct iphdr) + sizeof(struct ether_header));
    udph->source = htons(3423);
    udph->dest = htons(80);
    udph->check = 0; // skip
    
    tx_len += sizeof(struct udphdr);
    
    /*UDP payload*/
    for(i=tx_len; i<PAYLOAD_SZ; i++)
    {
        frame[i] = (uint8_t)i;        
    }
    tx_len += i;
    
    /* Length of UDP payload and header */
    udph->len = htons(tx_len - sizeof(struct ether_header) - sizeof(struct iphdr));
    /* Length of IP payload and header */
    iph->tot_len = htons(tx_len - sizeof(struct ether_header));
    /* Calculate IP checksum on completed header */
    iph->check = csum((unsigned short *)(frame+sizeof(struct ether_header)), sizeof(struct iphdr)/2);
    
    
    /* Destination address */
    
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen = ETH_ALEN;
    socket_address.sll_addr[0] = DST_MAC0;
    socket_address.sll_addr[1] = DST_MAC1;
    socket_address.sll_addr[2] = DST_MAC2;
    socket_address.sll_addr[3] = DST_MAC3;
    socket_address.sll_addr[4] = DST_MAC4;
    socket_address.sll_addr[5] = DST_MAC5;

    if (sendto(sock, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
    {        
        printf("Send failed\n");  
    }
    
    return 0;
}
