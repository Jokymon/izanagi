#include <io.h>
#include "drivers/pci.h"

#define PCI_CONFIG_DATA 0xcfc
#define PCI_CONFIG_ADDR 0xcf8

#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION 0x08
#define PCI_CLASS 0x0B
#define PCI_SUBCLASS 0x0A
#define PCI_INTERFACE 0x09
#define PCI_HEADERTYPE 0x0E
#define PCI_BAR0 0x10
#define PCI_INTERRUPT 0x3C

int pci_is_available()
{
    unsigned int temp;
    temp = inl(PCI_CONFIG_DATA);
    return (temp==0xffffffff);
}

uint32_t pci_read_dword(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg)
{
    uint32_t offset = reg % 0x04;

    outl(PCI_CONFIG_ADDR, 
        0x1 << 31
        | ((bus     & 0xFF) << 16) 
        | ((device  & 0x1F) << 11) 
        | ((func    & 0x07) <<  8) 
        | ((reg     & 0xFC)));

    return inl(PCI_CONFIG_DATA) >> (8 * offset);
}

uint16_t pci_config_read_word(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg)
{
    return pci_read_dword(bus, device, func, reg) & 0xffff;
}

bool pci_has_functions(uint32_t bus, uint32_t device)
{
    return pci_read_dword(bus, device, 0, PCI_HEADERTYPE) & (1 << 7);
}

void pci_list_devices()
{
    int bus;
    unsigned int device, k;
    for (bus=0; bus<8; bus++)
    {
        for (device=0x0; device<32; device++)
        {
            for (k=0; k<8; k++)
            {
                uint16_t vendor_id = pci_config_read_word(bus, device, k, PCI_VENDOR_ID);
                uint16_t device_id = pci_config_read_word(bus, device, k, PCI_DEVICE_ID);

                //if (!pci_has_functions(bus, device)) continue;

                if ((vendor_id != 0xffff) && vendor_id)
                {
                    printf("On bus: %u, device: %u, function: %u VendorId/DeviceId: %x/%x\n", bus, device, k, vendor_id, device_id);
                }
            }
        }
    }
}
