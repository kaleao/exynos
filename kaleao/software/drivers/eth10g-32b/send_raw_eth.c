#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>

#include <net/if.h>
#include <sys/ioctl.h>


#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <netinet/in.h>


#define PRINT_FFLUSH
#define PRINT_IO	stdout
#ifdef PRINT_FFLUSH
	#define vpr(...) {if(verbose==1){fprintf(PRINT_IO, __VA_ARGS__);fflush(PRINT_IO);}}
	#define pr(...) {fprintf(PRINT_IO, __VA_ARGS__);fflush(PRINT_IO);}
#else
	#define vpr(...) {if(verbose==1){printf(__VA_ARGS__);}}
	#define pr(...) {printf(__VA_ARGS__);}
#endif

#define DEFAULT_IF	"eth0"
#define PAYLOAD_SZ 64

char *iface = NULL;
unsigned char src_mac[6] = {0x00, 0x00, 0xC0, 0xA8, 0x03, 0x66};
unsigned char dst_mac[6] = {0x00, 0x0a, 0xf7, 0x3b, 0xfe, 0x40};

unsigned char *src = NULL;
unsigned char *dst = NULL;

unsigned char* frame = NULL;
unsigned int payload_size = 0;

void parse_args(int argc, char **argv);
int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len);
void print_mac(unsigned char *mac);

int  main(int argc, char **argv)
{ 
    int i = 0;
    int status = 0;
    int sock = 0;
    unsigned int sz = 0;
    struct sockaddr_ll socket_address;    
    unsigned char* eth_hdr = NULL;
    unsigned char* eth_payload = NULL;    
    struct ethhdr *eh = NULL;
    
    struct ifreq if_idx;
    
    parse_args(argc, argv);
    
    sz = PAYLOAD_SZ + 14;
    
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock == -1) {printf("Error: cannot open socket\n");return 0;}    
    
    memset(&if_idx, 0, sizeof(struct ifreq));
    if(iface!=NULL)
    {
        strncpy(if_idx.ifr_name, iface, IFNAMSIZ-1);
    }else
    {
        strncpy(if_idx.ifr_name, DEFAULT_IF, IFNAMSIZ-1);        
    }    
    if (ioctl(sock, SIOCGIFINDEX, &if_idx) < 0){perror("SIOCGIFINDEX");}
    


    frame = (unsigned char *)malloc(sz);
                 
    eth_hdr = frame;
    eth_payload = frame + 14;    
    eh = (struct ethhdr *)eth_hdr;
    
    socket_address.sll_family   = PF_PACKET;	
    socket_address.sll_protocol = htons(ETH_P_IP);
    socket_address.sll_ifindex  = 2;
    socket_address.sll_hatype   = ARPHRD_ETHER;
    socket_address.sll_pkttype  = PACKET_OTHERHOST;	
    socket_address.sll_halen    = ETH_ALEN;	

    if(dst!=NULL)
    {
        socket_address.sll_addr[0]  = dst[0];		
        socket_address.sll_addr[1]  = dst[1];		
        socket_address.sll_addr[2]  = dst[2];
        socket_address.sll_addr[3]  = dst[3];
        socket_address.sll_addr[4]  = dst[4];
        socket_address.sll_addr[5]  = dst[5];        
    }else
    {
        socket_address.sll_addr[0]  = dst_mac[0];		
        socket_address.sll_addr[1]  = dst_mac[1];		
        socket_address.sll_addr[2]  = dst_mac[2];
        socket_address.sll_addr[3]  = dst_mac[3];
        socket_address.sll_addr[4]  = dst_mac[4];
        socket_address.sll_addr[5]  = dst_mac[5];     
    }

/*MAC - end*/
    socket_address.sll_addr[6]  = 0x00;/*not used*/
    socket_address.sll_addr[7]  = 0x00;/*not used*/
    
    if(dst!=NULL)
    {
        memcpy(frame, dst, ETH_ALEN);        
    }else
    {
        memcpy(frame, dst_mac, ETH_ALEN);
    }
    if(src!=NULL)
    {
        memcpy((frame+ETH_ALEN), src, ETH_ALEN);
    }else
    {
        memcpy((frame+ETH_ALEN), src_mac, ETH_ALEN);
    }
    eh->h_proto = 0x00;
    
    for (i = 0; i < PAYLOAD_SZ; i++) 
    {
	eth_payload[i] = (unsigned char)i;
    }
            
    status = sendto(sock, frame, sz, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
    
    if (status == -1) {printf("ERROR: cannot send raw Eth frame.\n");return 0;}
    
    
    return 0;
}

void parse_args(int argc, char **argv)
{
	int status = 0;
	char c;
	char *num;
	char br = 0;

	static struct option long_options[] = {
		/* These options set a flag. */

		/* These options don't set a flag.
		  We distinguish them by their indices. */
                {"interface",     required_argument, 0, 'i'},
		{"src",           required_argument, 0, 's'},
		{"dst",	          required_argument, 0, 'd'},		
		{"payload-size",  required_argument, 0, 'l'},		
		{"help",          no_argument,       0, 'h'},
		{0, 0, 0, 0}
	};

	int option_index = 0;

	while(1)
	{
		if (br!=0){break;}
		c = getopt_long (argc, argv, "i:s:d:l:", long_options, &option_index);
		if (c==-1){break;}

		switch(c)
		{
			case 0:
				if (long_options[option_index].flag != 0) {break;}
				pr("option %s", long_options[option_index].name);
				if (optarg) {pr(" with arg %s", optarg);}
				pr("\n");
				break;
                        case 'i':
                                iface = (char *)malloc(sizeof(char));
                                if(iface==NULL){pr("ERROR: cannot allocate iface string.\n");}
                                
                                status = sscanf(optarg, "%s", iface);
                                pr("Eth interface: '%s'\n", iface);
                                
                                break;                                
			case 's':
                                src = (char *)malloc(6);
                                if(src==NULL){pr("ERROR: cannot allocate src mac.\n");}
				status = sscanf(optarg, "%x:%x:%x:%x:%x:%x", &src[0], &src[1], &src[2], &src[3], &src[4], &src[5]);
                                if (status<0){pr("Error: invalid source MAC address\n");exit(0);}   
                                pr("SRC MAC: '");                                
                                print_mac(src);
                                pr("'\n");                   
				break;
			case 'l':				
				payload_size = (unsigned int)strtol(optarg, &num, 0);
				break;
			case 'd':
                                dst = (char *)malloc(6);
                                if(dst==NULL){pr("ERROR: cannot allocate dst mac.\n");}
				status = sscanf(optarg, "%x:%x:%x:%x:%x:%x", &dst[0], &dst[1], &dst[2], &dst[3], &dst[4], &dst[5]);
                                if (status<0){pr("Error: invalid destination MAC address\n");exit(0);}                                
                                pr("DST MAC: '");                                
                                print_mac(dst);
                                pr("'\n");                                
				break;
			case 'h':
				exit(0);
				break;
			default:
				//pr(" dbg: default\n");
				br=1;
				break;
			}
	}
}

int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len)
{
    unsigned const char *pos = hexstring;
    char *endptr;
    size_t count = 0;

    if ((hexstring[0] == '\0') || (strlen(hexstring) % 2)) {
        //hexstring contains no data
        //or hexstring has an odd length
        return -1;
    }

    for(count = 0; count < len; count++) {
        char buf[5] = {'0', 'x', pos[0], pos[1], 0};
        data[count] = strtol(buf, &endptr, 0);
        pos += 2 * sizeof(char);

        if (endptr[0] != '\0') {
            //non-hexadecimal character encountered
            return -1;
        }
    }

    return 0;
}


void print_mac(unsigned char *mac)
{
    int i = 0;
    if(mac==NULL) {printf("EROOR: mac is NULL.\n");}
    
    for (i=0; i<6; i++)
    {
        printf("%02x ", mac[i]);
    }
}