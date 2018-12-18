#ifndef KGEX_ETHTOOL_H
#define KGEX_ETHTOOL_H

extern int kgex_get_settings(struct net_device *ndev,
		struct ethtool_cmd *ecmd);

extern int kgex_set_settings(struct net_device *ndev,
		struct ethtool_cmd *ecmd);

extern void kgex_get_drvinfo(struct net_device *ndev,
		struct ethtool_drvinfo *drvinfo);

#endif /* KGEX_ETHTOOL_H */
