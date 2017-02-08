#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x568fba06, "module_layout" },
	{ 0x72aa82c6, "param_ops_charp" },
	{ 0x8e950d31, "remove_proc_entry" },
	{ 0x76b9aa18, "proc_create_data" },
	{ 0x71de9b3f, "_copy_to_user" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x59cd8371, "send_sig_info" },
	{ 0xf85dc997, "pid_task" },
	{ 0x9922b8f8, "find_pid_ns" },
	{ 0x17877e44, "init_pid_ns" },
	{ 0x42224298, "sscanf" },
	{ 0x77e2f33, "_copy_from_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x27e1a049, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

