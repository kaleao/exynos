#ifndef KGEX_PCI_H
#define KGEX_PCI_H

#include "kgex.h"

#define PCI_VENDOR_ID_PLDA      0x1556

///PCI information for the device
static const struct pci_device_id kgex_pci_tbl[] = {
        { PCI_DEVICE(PCI_VENDOR_ID_XILINX, 0x8024), 0, 0, 0x0 },
        { PCI_DEVICE(PCI_VENDOR_ID_XILINX, 0x7021), 0, 0, 0x0 },
        { PCI_DEVICE(PCI_VENDOR_ID_XILINX, 0x7022), 0, 0, 0x0 },
        { PCI_DEVICE(PCI_VENDOR_ID_PLDA,   0x1100), 0, 0, 0x0 },
        { PCI_DEVICE(PCI_VENDOR_ID_PLDA,   0x11aa), 0, 0, 0x0 },
        { PCI_DEVICE(PCI_VENDOR_ID_PLDA,  0x1111), 0, 0, 0x0 },
        {0,},
};

/* exynos-pcie functions */
extern int exynos_pcie_reset_rc(struct pci_dev *rc);
extern unsigned long exynos_pcie_get_msi_addr(struct pci_dev *rc);
extern void exynos_pcie_link_restored(struct pci_dev *pdev);
extern void exynos_pcie_link_retrain(struct pci_dev *pdev);

extern void kgex_pci_restore_msi(struct net_device *ndev);
extern int kgex_pci_get_gen(struct net_device *ndev);
extern int kgex_pci_set_target_gen(struct net_device *ndev, int gen);
extern int kgex_pci_wait_gen2(struct net_device *ndev);
extern int kgex_pci_retrain(struct net_device *ndev);
extern int kgex_pci_endpoint_online(struct net_device *ndev);
extern void kgex_pci_restore_state(struct net_device *ndev);
extern int kgex_pci_probe(struct net_device *ndev);
extern void kgex_pci_remove(struct net_device *ndev);

#endif /* KGEX_PCI_H */
