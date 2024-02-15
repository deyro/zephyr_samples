/*
 * Copyright (c) 2019 Rohan Dey
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <shell/shell.h>
#include <version.h>
#include <logging/log.h>
#include <stdlib.h>
#include <disk/disk_access.h>
#include <fs/fs.h>
#include <ff.h>

LOG_MODULE_REGISTER(main);

static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = {
	.type = FS_FATFS,
	.fs_data = &fat_fs,
};
/*
*  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
*  in ffconf.h
*/
static const char *sd_mount_pt = "/SD:";

static int mount_sd_card(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argv);
//	shell_print(shell, "mount_sd_card\n");

	do {
		static const char *disk_pdrv = "SD";
		uint64_t memory_size_mb;
		uint32_t block_count;
		uint32_t block_size;

		if (disk_access_init(disk_pdrv) != 0) {
			LOG_ERR("Storage init ERROR!");
			break;
		}
		if (disk_access_ioctl(disk_pdrv,
				DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
			LOG_ERR("Unable to get sector count");
			break;
		}
		LOG_INF("Block count %u", block_count);

		if (disk_access_ioctl(disk_pdrv,
				DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
			LOG_ERR("Unable to get sector size");
			break;
		}
		LOG_INF("Sector size %u\n", block_size);

		memory_size_mb = (uint64_t)block_count * block_size;
		LOG_INF("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));
	} while (0);

	mp.mnt_point = sd_mount_pt;
	int res = fs_mount(&mp);
	if (res == FR_OK) {
		LOG_INF("Disk mounted, mount point = %s\n", sd_mount_pt);
	} else {
		LOG_ERR("Error mounting disk.\n");
	}

	return res;
}

static int unmount_sd_card(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argv);
	shell_print(shell, "unmount_sd_card\n");

	int res = fs_unmount(&mp);
	if (res == FR_OK) {
		LOG_INF("Disk unmounted\n");
	} else if (res == -EINVAL) {
		LOG_ERR("no system has been mounted at given mount point\n");
	} else {
		LOG_ERR("Error mounting disk.\n");
	}

	return 0;
}

static int lsdir(const char *path)
{
	int res;
	struct fs_dir_t dirp;
	static struct fs_dirent entry;

	/* Verify fs_opendir() */
	res = fs_opendir(&dirp, path);
	if (res) {
		printk("Error opening dir %s [%d]\n", path, res);
		return res;
	}

	printk("\nListing dir %s ...\n", path);
	for (;;) {
		/* Verify fs_readdir() */
		res = fs_readdir(&dirp, &entry);

		/* entry.name[0] == 0 means end-of-dir */
		if (res || entry.name[0] == 0) {
			break;
		}

		if (entry.type == FS_DIR_ENTRY_DIR) {
			printk("[DIR ] %s\n", entry.name);
		} else {
			printk("[FILE] %s (size = %zu)\n",
				entry.name, entry.size);
		}
	}

	/* Verify fs_closedir() */
	fs_closedir(&dirp);

	return res;
}

static int list_files(const struct shell *shell, size_t argc, char **argv)
{
	shell_print(shell, "list_files: argc=%d, argv[1] = %s\n", argc, argv[1]);

	lsdir(argv[1]);

	return 0;
}

static int cat_file(const struct shell *shell, size_t argc, char **argv)
{
//	ARG_UNUSED(argv);
	shell_print(shell, "cat_file: argc=%d, argv[1]=%s, argv[2]=%s\n", argc, argv[1], argv[2]);

	struct fs_file_t zfp;
	memset(&zfp, 0x00, sizeof(struct fs_file_t));

	/* Open the file */
	int res = fs_open(&zfp, argv[1], (FS_O_CREATE | FS_O_READ | FS_O_WRITE));
	if (res == 0) {
		LOG_INF("file created");
	} else if (res == -EINVAL) {
		LOG_ERR("bad file name");
	} else if (res == -EROFS) {
		LOG_ERR("read only file / file system");
	} else if (res == -ENOENT) {
		LOG_ERR("file path not possible");
	} else {
		LOG_ERR("failed to create file");
	}

	if (res < 0) {
		return res;
	}

	/* Do write operations here */
	int datalen = strlen(argv[2]);
	size_t wrbytes = fs_write(&zfp, argv[2], datalen);
	if (wrbytes == datalen) {
		LOG_INF("%d bytes written to file", wrbytes);
	} else if (wrbytes>0 && wrbytes<datalen) {
		LOG_INF("less bytes written, %d bytes", wrbytes);
	} else if(wrbytes == -ENOTSUP) {
		LOG_ERR("not implemented by underlying file system driver");
		res = -ENOTSUP;
	} else if(wrbytes < 0) {
		LOG_ERR("could not write to file");
		res = -1;
	}

	if (res < 0) {
		return res;
	}

	/* Close the file */
	res = fs_close(&zfp);
	if (res == 0) {
		LOG_INF("file closed");
	} else {
		LOG_ERR("could not close file");
	}

	return res;
}

static int read_file(const struct shell *shell, size_t argc, char **argv)
{
//	ARG_UNUSED(argv);
	shell_print(shell, "read_file: argc=%d, argv[1]=%s\n", argc, argv[1]);

	struct fs_file_t zfp;

	/* Open the file */
	int res = fs_open(&zfp, argv[1], (FS_O_CREATE | FS_O_READ));
	if (res == 0) {
		LOG_INF("file opened");
	} else if (res == -EINVAL) {
		LOG_ERR("bad file name");
	} else if (res == -EROFS) {
		LOG_ERR("read only file / file system");
	} else if (res == -ENOENT) {
		LOG_ERR("file path not possible");
	} else {
		LOG_ERR("failed to open file");
	}

	/* Do read operations here */
	char rdbuf = '\0';
	size_t rdbytes=0, totbytes=0;

	LOG_INF("Data read = ");
	do {
		rdbytes = fs_read(&zfp, &rdbuf, 1);
		printk("%c", rdbuf);
		totbytes++;
	} while (rdbytes > 0);
	LOG_INF("%d bytes read", totbytes);

	/* Close the file */
	res = fs_close(&zfp);
	if (res == 0) {
		LOG_INF("file closed");
	} else {
		LOG_ERR("could not close file");
	}

	return 0;
}

static int make_dir(const struct shell *shell, size_t argc, char **argv)
{
//	ARG_UNUSED(argv);
	shell_print(shell, "cat_file: argc=%d, argv[1]=%s\n", argc, argv[1]);

	int res = fs_mkdir(argv[1]);
	if (res == 0) {
		LOG_INF("directory created\n");
	} else if (res == -ENOTSUP) {
		LOG_ERR("not implemented by underlying file system driver\n");
	} else {
		LOG_ERR("could not create directory\n");
	}

	return 0;
}

static int delete_file(const struct shell *shell, size_t argc, char **argv)
{
//	ARG_UNUSED(argv);
	shell_print(shell, "cat_file: argc=%d, argv[1]=%s\n", argc, argv[1]);

	int res = fs_unlink(argv[1]);
	if (res == 0) {
		LOG_INF("delete successful\n");
	} else if (res == -EROFS) {
		LOG_ERR("file system is readonly\n");
	} else if (res == -ENOTSUP) {
		LOG_ERR("not implemented by underlying file system driver\n");
	} else {
		LOG_ERR("could not delete\n");
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(open_card,
		SHELL_CMD(mount, NULL, "mount sd card", mount_sd_card),
		SHELL_CMD(unmount, NULL, "unmount sd card", unmount_sd_card),
		SHELL_CMD_ARG(list, NULL, "list files", list_files, 2, 0),
		SHELL_CMD_ARG(mkdir, NULL, "create a directory", make_dir, 2, 0),
		SHELL_CMD_ARG(del, NULL, "deletes a directory or file", delete_file, 2, 0),
		SHELL_CMD_ARG(cat, NULL, "create a file and write data to it", cat_file, 2, 1),
		SHELL_CMD_ARG(rdfile, NULL, "read a file", read_file, 2, 0),
		SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_CMD_REGISTER(sd, &open_card, "Start the application", NULL);

void main(void)
{
	LOG_INF("Starting application ...");

#if 1	// may disable this if using shell commands
	char *argv[3] = {	"abc",
						"/SD:/hello",
						"hello! how are you."
					};

	int ret = mount_sd_card(NULL, 0 , NULL);
	if (ret == 0) {
		LOG_INF("SD card mounted successfully\n");
		k_sleep(K_MSEC(1000));
		cat_file(NULL, 3, argv);
	} else {
		LOG_ERR("SD card mount failed\n");
	}
#endif


	while (1) {
		k_sleep(K_MSEC(1000));
	}
}
