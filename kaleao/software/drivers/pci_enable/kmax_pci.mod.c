#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa5b776, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xfba33a07, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x95fb1549, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x567f6224, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0x4cf00d8d, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0xb8ada85a, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0x92291f3, __VMLINUX_SYMBOL_STR(pci_enable_msix) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x2180f636, __VMLINUX_SYMBOL_STR(pci_set_mwi) },
	{ 0xa11f80b5, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0xe0487b4e, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x880e38a4, __VMLINUX_SYMBOL_STR(pci_clear_mwi) },
	{ 0x1a704dc2, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x3a482f75, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xc5be1a90, __VMLINUX_SYMBOL_STR(pci_enable_device) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "FBB32484B1D75B728CB9FF9");
