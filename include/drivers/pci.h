#ifndef _PCI_H
#define _PCI_H

int pci_is_available();
uint32_t pci_config_read(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
uint16_t pci_config_read_word(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
bool pci_has_functions(uint32_t bus, uint32_t device);

void pci_list_devices();

#endif
