internal void usb_interrupt_routine(void)
{    
    u32 interrupt = USB_OTG_FS->GINTSTS;
    if (interrupt & USB_OTG_GINTSTS_USBRST) {
        USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_USBRST;
        usb_reset();
    } else if (interrupt & USB_OTG_GINTSTS_ENUMDNE) {
        USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;
        usb_after_reset();
    } else if (interrupt & USB_OTG_GINTSTS_RXFLVL) {
        USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_RXFLVL;
        usb_read();
    } else if (interrupt & USB_OTG_GINTSTS_OEPINT) {
        USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_OEPINT;
        usb_endpoint_out();
    } else if (interrupt & USB_OTG_GINTSTS_IEPINT) {
        USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_IEPINT;
        usb_endpoint_in();
    }
}

internal void usb_init_periph(void)
{
    volatile u32 dummy = 0;
    
    // @Note: GPIO setup, some of them need to function as D+ and D-
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    dummy = RCC->AHB1ENR;
    
    GPIOA->MODER |= (0x02 << GPIO_MODER_MODER11_Pos) | (0x02 << GPIO_MODER_MODER12_Pos);
    GPIOA->OSPEEDR |= (0x02 << GPIO_OSPEEDR_OSPEED11_Pos) | (0x02 << GPIO_OSPEEDR_OSPEED12_Pos);
    GPIOA->AFR[1] |= (0x0A << GPIO_AFRH_AFSEL11_Pos) | (0x0A << GPIO_AFRH_AFSEL12_Pos);

    // @Note: Disable vbus sensing
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBUSBSEN;
    USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBUSASEN;
    
    // @Note: Enable OTG FS clock
    RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
    dummy = RCC->AHB2ENR;
        
    while ((USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0);
}

internal void usb_init_device(void)
{
    // @Note: Disabling session request protcol and forcing device mode
    USB_OTG_FS->GUSBCFG &= ~(USB_OTG_GUSBCFG_SRPCAP | USB_OTG_GUSBCFG_TRDT);
    USB_OTG_FS->GUSBCFG |= (USB_OTG_GUSBCFG_FDMOD | (0x06 << USB_OTG_GUSBCFG_TRDT_Pos));
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    
    // @Note: Zero out clock and enable soft disconnect
    *USB_OTG_CLK = 0;
    USB_OTG_DEV->DCTL |= USB_OTG_DCTL_SDIS;

    // @Note: Zero out device address and set device speed
    USB_OTG_DEV->DCFG &= ~USB_OTG_DCFG_DAD;
    USB_OTG_DEV->DCFG |= (0x03 << USB_OTG_DCFG_DSPD_Pos) | USB_OTG_DCFG_NZLSOHSK;
    
    USB_OTG_FS->GINTMSK = USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM |
        USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_OEPINT |
        USB_OTG_GINTMSK_IEPINT;
    
    USB_OTG_FS->GINTSTS = 0xFFFFFFFF;
    USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

internal void usb_init(void)
{
    usb_init_periph();
    usb_init_device();
    NVIC_EnableIRQ(OTG_FS_IRQn);
}

internal void usb_start(void)
{
    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;
    USB_OTG_DEV->DCTL &= ~USB_OTG_DCTL_SDIS;
}

internal void usb_reset(void)
{
    USB_OTG_DEV->DCFG &= ~USB_OTG_DCFG_DAD;
    USB_OTG_FS->GRXFSIZ = 0x30;
    USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (16 << USB_OTG_TX0FD_Pos) | 0x30;
    USB_OTG_DEV->DCTL |= USB_OTG_DCTL_CGONAK;
}

internal void usb_after_reset(void)
{
    USB_OTG_DEV->DAINTMSK = ((1 << 16) | 1);
    USB_OTG_DEV->DOEPMSK = USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM |
        USB_OTG_DOEPMSK_OTEPDM | USB_OTG_DOEPMSK_OTEPSPRM;
    USB_OTG_DEV->DIEPMSK = USB_OTG_DIEPMSK_XFRCM;

    USB_OTG_IN->DIEPCTL &= (0x03 << USB_OTG_DIEPCTL_MPSIZ_Pos);

    USB_OTG_OUT->DOEPTSIZ = (1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
    USB_OTG_OUT->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
}

internal void usb_read(void)
{
    u32 status = USB_OTG_FS->GRXSTSP;
    u8 endpoint = status & 0xF;
    u16 bytes = (status & USB_OTG_GRXSTSP_BCNT) >> 4;

    switch ((status & USB_OTG_GRXSTSP_PKTSTS) >> 17) {
        case PKTSTS_OUT_RX: {
        } break;
            
        case PKTSTS_SETUP_RX: {
            if ((endpoint != 0) || (bytes != 8)) {
                return;
            }

            u32 word_len = (bytes >> 2);
            u32 *buffer = (u32 *) &usb_control;
            
            for (u32 i = 0; i < word_len; ++i, ++buffer) {
                UNALIGNED_U32_WRITE(buffer, *USB_OTG_FIFO);
            }
        } break;
            
        case PKTSTS_SETUP_COMPLETE: {
        } break;
    }
}

internal void usb_write(void *data, u16 len)
{
    USB_OTG_IN->DIEPTSIZ = (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos) | len;
    USB_OTG_IN->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
    
    u32 word_len = ((u32) len + 3) >> 2;
    u32 *buffer = (u32 *) data;
    
    for (u32 i = 0; i < word_len; ++i, ++buffer) {
        *USB_OTG_FIFO = UNALIGNED_U32_READ(buffer);
    }
}

internal void usb_endpoint_out(void) {
    if (USB_OTG_OUT->DOEPINT & USB_OTG_DOEPINT_STUP) {
        if (usb_control.request_type & 0x80) {
            // @Note: Device to Host
            switch (usb_control.request) {
                case USB_GET_STATUS: {
                    usb_write(&usb_status_response, usb_control.length);
                } break;

                // @ToDo: This does _NOT_ work! For some reason whenever we receive GET DESCRIPTOR REQUEST
                // usb_control.length varies in size (64, 64, 18, 64) and is received more than once. There's most
                // likely something with initialization(?) that I'm not seeing, feels like a simple fix but I can't get it to work.
                case USB_GET_DESCRIPTOR: {
                    switch (usb_control.valueh) {
                        case USB_DESCRIPTION_DEVICE: {
                            usb_write(usb_device_desc, usb_control.length);
                        } break;
                            
                        case USB_DESCRIPTION_CONFIG: {
                            usb_write(usb_configuration, usb_control.length);
                        } break;
                    }
                } break;

                default: {
                    // ...
                } break;
            }
        } else {
            // @Note: Host to Device
            u8 address = 0;
            switch (usb_control.request) {
                case USB_SET_ADDRESS: {
                    address = usb_control.valuel & 0x7F;
                    USB_OTG_DEV->DCFG |= address << 4;
                    usb_write(0, 0);
                } break;
            }
        }
    } else if (USB_OTG_OUT->DOEPINT & USB_OTG_DOEPINT_OTEPDIS) {
        (void) USB_OTG_OUT->DOEPINT;
    } else if (USB_OTG_OUT->DOEPINT & USB_OTG_DOEPINT_OTEPSPR) {
        USB_OTG_OUT->DOEPTSIZ |= (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        USB_OTG_OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
    } else if (USB_OTG_OUT->DOEPINT & USB_OTG_DOEPINT_XFRC) {
        USB_OTG_OUT->DOEPTSIZ |= (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        USB_OTG_OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
    }
    
    USB_OTG_OUT->DOEPINT = USB_OTG_OUT->DOEPINT;
}

internal void usb_endpoint_in(void)
{    
    if (USB_OTG_IN->DIEPINT & USB_OTG_DIEPINT_XFRC) {
        USB_OTG_OUT->DOEPTSIZ |= (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        USB_OTG_OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
    }
    
    USB_OTG_IN->DIEPINT = USB_OTG_IN->DIEPINT;
}
