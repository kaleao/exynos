 Software for Exynos 7420

Bootloaders  
Recovery Linux kernel  
Recovery Device Tree  
Recovery RootFS 'buildroot'  
Recovery userspace services and programs  
  
Production Linux kernel  
Production Device Tree  
Production RootFS 'Ubuntu Xenial Server 16.04 LTS'  
Production userspace services and programs  
  
SD Emulation Linux kernel  
SD Emulation Device Tree  
SD Emulation RootFS 'builtroot' used as ramdisk embedded in SD Emu Linux kernel Image  
SD Emulation automated services  

DEB packages for updating stuff  


## Features
- New PCIe revive architecture using a direct FULL hard reset which resets both PCIe Endpoint, AXI DMA and surrounding components
- Use of low noise SD emulation links as GPIOs for the hard reset, instead of the default GPIOs that are very noisy
- PCIe Gen1->Gen2 retrain capability
- Modified PCIe Root Complex kernel module to reset the PCIe Root Complex at boto with retries until link is up.
- Selective WFI enable/disable workaround to avoid PMIC damage
- ASV (Adaptive Supply Voltage) support in the Linux kernel 4.4
- Extended cpufreq subsystem to include all frequency/voltage steps in accordance to ASV
- Thermal throttling subsystem for SoC protection
- Shared memory for Exynos configuration and status feedback to NPU A9
- Support of VLAN tagged Ethernet Frames
- Ubuntu Linux kernel headers
- Ubuntu Linux kernel modules
- Docker support
- KVM support
- QEMU support
- Support for FPGA SFLR (Small Frame Latency reduce) feature


## Services
-- powermng.py - handles power commands for both Production and Factory Test systems














