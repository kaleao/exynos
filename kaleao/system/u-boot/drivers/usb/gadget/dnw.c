
#include <dnw.h>

#if (CONFIG_DNW_VERSION > 0x09)

/* Global variables */
static struct dnw_devinfo g_devinfo={
#if (CONFIG_DNW_VERSION == 0x0a)
	.dnw_version = DNW_v10,
#else
#error dnw version mismatch
#endif 
};

USBDNW g_usbdnw;

#define DEVICE_STRING_CONFIG_INDEX        4
#define DEVICE_STRING_INTERFACE_INDEX     5
#define DEVICE_STRING_MANUFACTURER_INDEX  1

static char *device_strings[DEVICE_STRING_MANUFACTURER_INDEX+1];
char* reply_msg;

void dnw_usbctl_init(void)
{
	exynos_usbctl_init();
}

static void dnw_set_serial_number(void){

}

int dnw_init(void){
	USBDNW *dnw = 	&g_usbdnw;
	int ret = 1;

	memset(dnw, 0, sizeof(dnw));
	dnw->state = dnwNOTINIT;
//	dnw->cmd.data = NULL;

	dnw->dnload.transfer_buffer = (unsigned char *)CFG_DNW_TRANSFER_BUFFER; 
	dnw->dnload.transfer_buffer_size = CFG_DNW_TRANSFER_BUFFER_SIZE;

	// usbd init
	dnw_usbctl_init();
	exynos_usbc_activate();

	device_strings[DEVICE_STRING_MANUFACTURER_INDEX]  = "Samsung S.LSI";
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "exynos reference board";
	dnw_set_serial_number();
	/* These are just made up */
	device_strings[DEVICE_STRING_CONFIG_INDEX]        = "DNW";
	device_strings[DEVICE_STRING_INTERFACE_INDEX]     = "DNW";

	reply_msg = (char *)exynos_usb_malloc(512, USBDEV3_MDWIDTH/8);

	ret = 0;	// This is a fake return value, because we do not initialize USB yet!

	dnw->state = dnwIDLE;

	return ret;


}

#define DNW_USBD_IS_CONNECTED() (1)
#define DNW_USBD_DETECT_IRQ() EXYNOS_USBD_DETECT_IRQ()
#define DNW_USBD_CLEAR_IRQ()  EXYNOS_USBD_CLEAR_IRQ()

int dnw_usb_int_hndlr(void)
{
	return exynos_udc_int_hndlr();
}

int dnw_usb_int_bulkin(const char *buffer, unsigned int buffer_size)
{
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;
	usbdev3_trb_ptr_t pBulkInTrb;
	u32 usCapTrbBufSiz, uLastBufSize;
	u32 i;

	// Set TRB for multiple Bulk IN Packet
	usCapTrbBufSiz = TRB_BUF_SIZ_LIMIT/oUsbDev3.m_uBulkEPMaxPktSize*oUsbDev3.m_uBulkEPMaxPktSize;
	g_uCntOfBulkInTrb = buffer_size/usCapTrbBufSiz;

	if ((buffer_size%usCapTrbBufSiz) != 0)
		g_uCntOfBulkInTrb++;

	g_pBulkInTrb = (usbdev3_trb_ptr_t)exynos_usb_malloc(g_uCntOfBulkInTrb*sizeof(usbdev3_trb_t), USBDEV3_MDWIDTH/8);

	if (g_pBulkInTrb == NULL)
		Assert(0);

	pBulkInTrb = g_pBulkInTrb;
	// Fill the Trbs
	usbdev3_trb_ctrl.data = 0;
	usbdev3_trb_ctrl.b.lst = 0;
	usbdev3_trb_ctrl.b.chn = 1;
	usbdev3_trb_ctrl.b.csp = 0;
	usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_NORMAL;
	usbdev3_trb_ctrl.b.isp_imi = 1;
	usbdev3_trb_ctrl.b.ioc = 0;
	usbdev3_trb_ctrl.b.strmid_sofn = 0;

	for(i=0;i<(g_uCntOfBulkInTrb-1);i++, pBulkInTrb++)
		exynos_usb_fill_trb(pBulkInTrb, (u32)(buffer+usCapTrbBufSiz*i), usCapTrbBufSiz, usbdev3_trb_ctrl.data, 1);

	usbdev3_trb_ctrl.b.lst = 1;
	usbdev3_trb_ctrl.b.chn = 0;
	usbdev3_trb_ctrl.b.ioc = 1;
	uLastBufSize = buffer_size-usCapTrbBufSiz*i;
	exynos_usb_fill_trb(pBulkInTrb, (u32)(buffer+usCapTrbBufSiz*i), uLastBufSize, usbdev3_trb_ctrl.data, 1);

	// . Issue Start Xfer for multiple Bulk IN Packet
	//------------------------------------
	if (!exynos_usb_start_ep_xfer(USBDEV3_EP_DIR_IN, BULK_IN_EP, (u32)g_pBulkInTrb, 0, &oUsbDev3.m_uTriIn[BULK_IN_EP]))
	{
		return 0;
	}

	return 1;
}

u32 transfer_size=0;
u32 dnw_response_flag=0;

int dnw_tx_status(const char *buffer, unsigned int buffer_size, const u32 need_sync_flag)
{
	/* dnw resp buffer */
	transfer_size = MIN(512, buffer_size);

	if (dnw_response_flag)
		printf(" Response tx Warnning\n");

#if 0
	printf("%s, transfer_size: %d, msg size: %d\n", 
		__func__, transfer_size, sizeof(reply_msg)); //test
#endif
	memcpy(reply_msg, buffer, transfer_size);
	dnw_response_flag=1;
	dnw_usb_int_bulkin(reply_msg, transfer_size);

	if (need_sync_flag)
	{
		while(dnw_response_flag)
			dnw_poll();
	}
	return 1;
}

static int dnw_receive_data (const unsigned char *buffer, unsigned int buffer_size)
{
	USBDNW *dnw = &g_usbdnw;
	int ret = 1;
//	if (download_size)
	{
		/* Something to download */
	
		if (buffer_size)
		{
			/* Handle possible overflow */
			unsigned int transfer_size = dnw->dnload.tot_size - dnw->dnload.downloaded_bytes;

			if (buffer_size < transfer_size)
				transfer_size = buffer_size;

			/* Save the data to the transfer buffer */
			memcpy ((u32)(dnw->dnload.start_address + dnw->dnload.downloaded_bytes),
				buffer, transfer_size);

			dnw->dnload.downloaded_bytes += transfer_size;

			/* Check if transfer is done */
			if (dnw->dnload.downloaded_bytes >= dnw->dnload.tot_size)
			{
				/* Reset global transfer variable,
				   Keep download_bytes because it will be
				   used in the next possible flashing command */
				dnw->dnload.tot_size = 0;

				if (dnw->dnload.dnload_error)
				{
					/* There was an earlier error */
//					sprintf(response, "ERROR");
				}
				else
				{
					/* Everything has transferred,
					   send the OK response */
//					sprintf(response, "OKAY");
					ret = DNW_OK;
					dnw->state = dnwIDLE;
				}

				printf("\ndownloading of %ld bytes finished\n", dnw->dnload.downloaded_bytes);
//				LCD_setprogress(0);

			}

			/* Provide some feedback */
			if (dnw->dnload.downloaded_bytes && dnw->dnload.tot_size &&
			    0 == (dnw->dnload.downloaded_bytes & (0x100000 - 1)))
			{
				/* Some feeback that the download is happening */
				if (dnw->dnload.dnload_error)
					printf("X");
				else
					printf(".");
				if (0 == (dnw->dnload.downloaded_bytes %
					  (80 * 0x100000)))
					printf("\n");

//				LCD_setfgcolor(0x2E8B57);
//				LCD_setprogress(download_bytes / (dnw->dnload.tot_size/100));
			}
		}
		else
		{
			/* Ignore empty buffers */
			printf("Warning empty download buffer\n");
			printf("Ignoring\n");
		}
		ret = 0;
	}
	return ret;
}


//const char hex_asc[] = "0123456789abcdef";
extern const char hex_asc[]; 
#define hex_asc_lo(x)   hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)   hex_asc[((x) & 0xf0) >> 4]

static inline char *pack_hex_byte(char *buf, u8 byte)
{
	*buf++ = hex_asc_hi(byte);
	*buf++ = hex_asc_lo(byte);
	return buf;
}


static int dnw_rx_handler (const unsigned char *buffer, unsigned int buffer_size)
{
	int ret = 1;
	struct dnw_response *response=NULL;
	const struct dnw_cmd *cmdbuf = (struct dnw_cmd*)buffer;
	USBDNW *dnw = &g_usbdnw;
	char hex_data[32];
	char	*pbuf=NULL; //= *buffer; 
		/* Generic failed response */

//	printf("%s,  size: %d\n", __func__, buffer_size); 

	response = malloc(sizeof(struct dnw_response));
		
	response->cmd_status = ERR_UNKNOWN;
	response->cmd = cmdbuf->cmd;

	if( cmdbuf->cmd < DNW_CMD_END){
		memcpy( &dnw->cmd, cmdbuf, sizeof(struct dnw_cmd));
	}		

	if(dnw->dnload.tot_size){
		response->cmd_status = dnw_receive_data(buffer, buffer_size);
		if(DNW_OK != response->cmd_status){
			return -1; // ret = -1;			
		}else if(dnwIDLE == dnw->state){
			goto send_tx_status;	
		}else{
			goto err_exit;
		}		
	}

	printf("%s, cmd:%d,   size: %d\n", __func__, cmdbuf->cmd, buffer_size); 
	printf("%s, state: %d\n", __func__, dnw->state); 

	{
		int i=0;
		pbuf = hex_data; // buffer;
		while(i < MIN(16, buffer_size)){
			pbuf = pack_hex_byte(pbuf, *(buffer + i));
			i++; 
			*pbuf++ = ' '; 
//			pbuf = pack_hex_byte(pbuf, *(buffer + (pbuf -hex_data));
			//snprintf(hex_data, 3, "%02x ", *pbuf); //  buf, size, fmt, args...)
		};
	}
	printf("%s\n", hex_data); 

	switch(cmdbuf->cmd){
		case DNW_GETSTATE:
			response->data[0] = g_usbdnw.state;
			response->cmd_status = DNW_OK;
			response->length = 1;
			ret = 0;
			goto send_tx_status;	
			break;
		case DNW_DEVINFO:
			memcpy(response->data, &g_devinfo, sizeof(struct dnw_devinfo));
			response->cmd_status = DNW_OK;
			response->length = sizeof(struct dnw_devinfo);
			ret = 0;
			goto send_tx_status;	
			break;			
		case DNW_DNLOAD:
		{
			struct dnw_subcmd_dnload *subcmd_dnload = NULL;

			subcmd_dnload = buffer + sizeof(struct dnw_cmd);
			dnw->state = dnwDOWNLOADSYNC; 
			dnw->dn_stor.start_address = subcmd_dnload->start_address;
			dnw->dn_stor.tot_size	= subcmd_dnload->tot_size;

			dnw->dnload.start_address = CFG_DNW_TRANSFER_BUFFER;
			dnw->dnload.tot_size = subcmd_dnload->tot_size;
			dnw->dnload.packet_size = subcmd_dnload->packet_size;
			
			response->cmd_status = DNW_OK;
			ret = 0;
			goto send_tx_status;	
		}
			break;
			
		case DNW_STORE:
		{
			struct dnw_subcmd_store *subcmd_store = NULL;
			if(buffer_size < (sizeof(struct dnw_cmd) + sizeof(struct dnw_subcmd_store))){ // error
				break;
			}
			subcmd_store = buffer + sizeof(struct dnw_cmd);
			dnw->state = dnwDOWNLOADSYNC; 
			dnw->dnload.start_address = subcmd_store->start_address;
			dnw->dnload.tot_size = subcmd_store->tot_size;
//			dnw->dnload.packet_size = subcmd_store->packet_size;			
				
			/* Reset the bytes count, now it is safe */
			dnw->dnload.downloaded_bytes = 0;
			/* Reset error */
			dnw->dnload.dnload_error = 0;

			response->cmd_status = DNW_OK;
			ret = 0;
			printf("Storing data start:0x%08x, size:%08d bytes\n", 
			 	dnw->dnload.start_address, dnw->dnload.tot_size);
			
			goto send_tx_status;	
		}
			break;
		case DNW_REBOOT: 	
			response->cmd_status = DNW_OK;
			dnw_tx_status((char *)response, sizeof(struct dnw_response), 0);
			do_reset (NULL, 0, 0, NULL);
			break;
		default:
			response->cmd_status = ERR_UNKNOWN;
			ret = -1;
			break;
	}
	
send_tx_status:
	printf("%s, send_tx_status, size: %d\n", __func__, sizeof(struct dnw_response));
	dnw_tx_status((char *)response, sizeof(struct dnw_response), 0);
	if(response){
		free(response); 
		response = NULL;
	}

err_exit:
	if(response){
		free(response); 
		response = NULL;
	}
}

void dnw_usb_handle_ep_in_xfer_complete(void)
{
	printf("%s, dnw_response_flag: %d\n", __func__, dnw_response_flag);

	if (dnw_response_flag) {
		exynos_usb_free((u32)g_pBulkInTrb);
		dnw_response_flag=0;
	}

	return;
}

int dnw_usb_handle_ep_out_xfer_complete(void)
{
	u32 usRxCnt;
	usbdev3_trb_ctrl_t usbdev3_trb_ctrl;

	// Check whether TRB was finished successfully or not
	if ((g_pBulkOutTrb0->control.b.hwo != 0)||(g_pBulkOutTrb0->status.b.trb_sts != 0))
	{
		Assert(0);
	}

	usRxCnt = oUsbDev3.m_uBulkEPMaxPktSize - g_pBulkOutTrb0->status.b.buf_siz;

	if (usRxCnt < oUsbDev3.m_uBulkEPMaxPktSize)
		g_ucTempDownBuf[usRxCnt] = 0;

	/* Pass this up to the interface's handler */
	dnw_rx_handler(g_ucTempDownBuf, usRxCnt);

	// . Set TRB for 1st Bulk Out Packet
	//-----------------------------
	usbdev3_trb_ctrl.data = 0;
	usbdev3_trb_ctrl.b.lst = 1;
	usbdev3_trb_ctrl.b.trb_ctrl = (u32)TRB_CTRL_NORMAL;
	usbdev3_trb_ctrl.b.isp_imi = 1;
	usbdev3_trb_ctrl.b.ioc = 1;
	usbdev3_trb_ctrl.b.strmid_sofn = 0;

	exynos_usb_fill_trb(g_pBulkOutTrb0, (u32)g_ucTempDownBuf, oUsbDev3.m_uBulkEPMaxPktSize, usbdev3_trb_ctrl.data, 1);

	// . Issue Start Xfer for 1st Bulk Out Packet
	//------------------------------------
	if (!exynos_usb_start_ep_xfer(USBDEV3_EP_DIR_OUT, BULK_OUT_EP, (u32)g_pBulkOutTrb0, 0, &oUsbDev3.m_uTriOut[BULK_OUT_EP]))
	{
		return 0;
	}

	return 1;
}



int dnw_poll(void)
{
	/* No activity */
	int ret = DNW_INACTIVE;

	if (!exynos_usb_wait_cable_insert() ) {
		exynos_usbctl_init();
		exynos_usbc_activate();
		printf("%s,  \n", __func__);
	}

	/* A disconnect happended, this signals that the cable
	   has been disconnected, return immediately */
	if (!DNW_USBD_IS_CONNECTED()) {
		return DNW_DISCONNECT;
	} else if (DNW_USBD_DETECT_IRQ()) {
		if (!dnw_usb_int_hndlr())
			ret = DNW_OK;
		else
			ret = ERR_USB;
		DNW_USBD_CLEAR_IRQ();
	}

	return ret;
}

void dnw_shutdown(void)
{
	/* when operation is done, usbd must be stopped */
	exynos_usb_stop();
}

#endif
