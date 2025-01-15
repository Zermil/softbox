#ifndef USB_H
#define USB_H

#define USB_OTG_DEV ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_IN ((USB_OTG_INEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_IN_ENDPOINT_BASE))
#define USB_OTG_OUT ((USB_OTG_OUTEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_OUT_ENDPOINT_BASE))
#define USB_OTG_FIFO ((volatile u32 *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE))
#define USB_OTG_CLK ((volatile u32 *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE))

#define PKTSTS_OUT_RX 0x02
#define PKTSTS_OUT_COMPLETE 0x03
#define PKTSTS_SETUP_COMPLETE 0x04
#define PKTSTS_SETUP_RX 0x06

#define USB_GET_STATUS 0x00
#define USB_SET_ADDRESS 0x05
#define USB_GET_DESCRIPTOR 0x06

#define USB_DESCRIPTION_DEVICE 0x01
#define USB_DESCRIPTION_CONFIG 0x02

internal void usb_interrupt_routine(void);
internal void usb_init_periph(void);
internal void usb_init_device(void);
internal void usb_init(void);
internal void usb_start(void);
internal void usb_reset(void);
internal void usb_after_reset(void);
internal void usb_read(void);
internal void usb_write(void *data, u16 len);
internal void usb_endpoint_out(void);
internal void usb_endpoint_in(void);

typedef struct USB_Control USB_Control;
struct USB_Control
{
    u8 request_type;
    u8 request;
    u8 valuel;
    u8 valueh;
    u16 index;
    u16 length;
};

global USB_Control usb_control = {0};
global u16 usb_status_response = 0;

// @Note: Reponses from device to host.

global u8 usb_device_desc[] = {
    0x12, // bLength
    0x01, // bDescriptorType
    0x02, 0x00, // bcdUSB
    0x00, // bDeviceClass
    0x00, // bDeviceSubclass
    0x00, // bDeviceProtocol
    0x40, // bMaxPacketSize
    0x04, 0x83, // idVendor
    0x57, 0x2B, // idProduct
    0x02, 0x00, // bcdDevice
    0x01, // iManufacturer
    0x02, // iProduct
    0x03, // iSerialNumber
    0x01, // bNumConfigurations
};
global u8 usb_configuration[] = {
    0x09, // bLength
    0x02, // bDescriptorType
    0x34, 0x00, // wTotalLength
    0x01, // bNUmInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfigurationValue
    0x00, 0xE0, // bmAttributes
    0x32, // bMaxPower
};

#endif // USB_H
