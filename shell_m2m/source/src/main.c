#include <zephyr.h>
#include <sys/printk.h>
#include <shell/shell.h>
#include <version.h>
#include <logging/log.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(app);

static int cmd_version(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	/* Log messages to be printed on the console */
	LOG_INF("cmd_version received");
	LOG_INF("responding: [Zephyr version %s]", KERNEL_VERSION_STRING);
//	printk("1.2.3.4");

	/* Response of the command, to be printed on the shell */
	shell_print(shell, "Zephyr version %s", KERNEL_VERSION_STRING);
//	shell_print(shell, "1.2.3.4");

	return 0;
}

SHELL_CMD_ARG_REGISTER(version, NULL, "Show kernel version", cmd_version, 1, 0);

void main(void)
{
	/* Log messages to be printed on the console */
	LOG_INF("Starting shell m2m application");

	while (1) {
		// Do nothing ...
		k_sleep(K_MSEC(2000));
	}
}
