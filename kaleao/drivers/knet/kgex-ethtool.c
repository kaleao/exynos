#include "kgex.h"
#include "kgex-ethtool.h"
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/pci.h>

int kgex_get_settings(struct net_device *ndev, struct ethtool_cmd *ecmd) {
	return 0;
}

int kgex_set_settings(struct net_device *ndev, struct ethtool_cmd *ecmd) {
	return 0;
}

void kgex_get_drvinfo(struct net_device *ndev,
		struct ethtool_drvinfo *drvinfo) {
	struct net_private *lp = NULL;
	lp = netdev_priv(ndev);

	strlcpy(drvinfo->driver, DRIVER_NAME, sizeof(drvinfo->driver));
	strlcpy(drvinfo->version, DRIVER_VERSION, sizeof(drvinfo->version));

	strlcpy(drvinfo->bus_info, pci_name(lp->pdev), sizeof(drvinfo->bus_info));
}
