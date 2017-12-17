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
	{ 0xc6c01fa, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x16abfc10, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0xf699984e, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0xf16343f3, __VMLINUX_SYMBOL_STR(sysfs_create_file_ns) },
	{ 0x9273028d, __VMLINUX_SYMBOL_STR(kobject_create_and_add) },
	{ 0x46ad5d5f, __VMLINUX_SYMBOL_STR(kernel_kobj) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xe2d5255a, __VMLINUX_SYMBOL_STR(strcmp) },
	{ 0x391afe42, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
    "depends=";


MODULE_INFO(srcversion, "CA8BAC7328FA6DF97D70F6E");
