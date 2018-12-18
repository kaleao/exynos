#ifndef DNW_H
#define DNW_H

#include <common.h>
#include <command.h>

#include <asm/byteorder.h>
//#include <asm/arch/hardware.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>

#ifdef CONFIG_DNW
#define DNW_RESPONSE_PACKET_SIZE 64
enum{
	DNW_DETATCH = 0, 
	DNW_DNLOAD,
	DNW_UPLOAD,
	DNW_GETSTATUS,
	DNW_CLRSTATUS,
	DNW_GETSTATE,
	DNW_ABORT,
	DNW_DEVINFO,
	DNW_STORE,
	DNW_REBOOT,
	DNW_CMD_END,
};

enum{
	DNW_OK	=0,
	DNW_DISCONNECT,
	DNW_INACTIVE,
	ERR_TARGET,
	ERR_FILE,
	ERR_WRITE,
	ERR_ERASE,
	ERR_VERIFY,
	ERR_ADDRESS,
	ERR_USB,
	ERR_UNKNOWN,
};

enum{
	DNW_v01	= 1,
	DNW_v02	= 2,
	DNW_v05	= 5,
	DNW_v10	= 10,
};

struct dnw_devinfo{
	char chip_info[4];
	char dnw_version;
	char reserved[3];
}__attribute__ ((packed));

struct dnw_subcmd_dnload
{
	char	sub_cmd;
	char reserved[3];
	u32	start_address;
	u32	tot_size;
	u32 packet_size;
} __attribute__ ((packed));

struct dnw_subcmd_store
{
	u64 start_address;
	u64 tot_size;
	u32 attribute;
//	u32 reserved;

/*	u32 start_address;
	u32 tot_size;
	u32 packet_size;
	u32 reserved; */
} __attribute__ ((packed));

struct dnw_cmd
{
	unsigned char cmd;
	unsigned char reserved[3];
//	u32		length;
// temp	void*	data;
} __attribute__ ((packed));

struct dnw_response
{
	unsigned char cmd;
	unsigned char cmd_status;
	unsigned char reserved[2];
	u32		length;
	unsigned char	data[DNW_RESPONSE_PACKET_SIZE-8];
} __attribute__ ((packed));

/* state */
typedef enum
{
	dnwNOTINIT=0,
	dnwIDLE	= 2,
	dnwDOWNLOADSYNC,
	dnwDNBUSY,
	dnwERROR,
}USBDNW_STATE;

//static unsigned int download_size;
//static unsigned int download_bytes;
//static unsigned int download_bytes_unpadded;
///static unsigned int download_error;

struct dnw_dnload_ram_type{
	u64 	start_address;	
	u64 tot_size;
	u64 packet_size;
	u64 transfed_size;
	u64 remained_size;
	u64 downloaded_bytes;
	u64 dnload_error;
	unsigned char * transfer_buffer;
	u64 transfer_buffer_size;
};

struct dnw_dnload_str_type{
	u32 start_address;
	unsigned int tot_size;
};


typedef struct{
	USBDNW_STATE state;	
	struct dnw_cmd		cmd;
	struct dnw_dnload_ram_type dnload;
	struct dnw_dnload_str_type dn_stor;
}USBDNW;


extern int dnw_init(void);
extern void dnw_shutdown(void);
extern int dnw_poll(void);
extern void dnw_usb_handle_ep_in_xfer_complete(void);
extern int dnw_usb_handle_ep_out_xfer_complete(void);
#else

#endif /* CONFIG_DNW */

#endif /* DNW_H */
