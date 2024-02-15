/*
 * Copyright (c) 2019 Tavish Naruka <tavishnaruka@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Sample which uses the filesystem API and SDHC driver */

#include <zephyr.h>
#include <device.h>
#include <storage/disk_access.h>
#include <logging/log.h>
#include <fs/fs.h>
#include <ff.h>

LOG_MODULE_REGISTER(main);

static int lsdir(const char *path);
size_t bsp_fs_create_append_file(const char* file_name, const void *data, size_t len);

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
static const char *disk_mount_pt = "/SD:";

void main(void)
{
	/* raw disk i/o */
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
		printk("Sector size %u\n", block_size);

		memory_size_mb = (uint64_t)block_count * block_size;
		printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));
	} while (0);

	mp.mnt_point = disk_mount_pt;

	int res = fs_mount(&mp);

	if (res == FR_OK) {
		printk("Disk mounted.\n");
		lsdir(disk_mount_pt);
	} else {
		printk("Error mounting disk.\n");
	}

	char * str = "hello, hello, hello ...";
	while (1) {
		bsp_fs_create_append_file("/SD:/test.log", str, strlen(str));
		k_sleep(K_MSEC(1000));
	}
}

static int lsdir(const char *path)
{
	int res;
	struct fs_dir_t dirp;
	static struct fs_dirent entry;

	fs_dir_t_init(&dirp);

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

size_t bsp_fs_create_append_file(const char* file_name, const void *data, size_t len) {
	printk("bsp_fs_create_append_file: file_name=%s, length=%d\n", file_name, len);

	int res=0;
	size_t wrbytes=0;

/*
	if (!bsp_fs_sd_card_is_mounted()) {
		res = -ENXIO;
		goto err;
	}
*/

	struct fs_file_t zfp;
	fs_file_t_init(&zfp);
//	memset(&zfp, 0x00, sizeof(struct fs_file_t));

	/* Open the file */
	res = fs_open(&zfp, file_name, (FS_O_CREATE | FS_O_READ | FS_O_WRITE | FS_O_APPEND));
	if (res == 0) {
		printk("file created");
	} else {
		wrbytes = res;
		goto err;
	}

	/* Do write operations here */
	size_t datalen = len;
	wrbytes = fs_write(&zfp, data, datalen);
	if (wrbytes == datalen) {
		printk("%d bytes written to file", wrbytes);
	} else if (wrbytes > 0 && wrbytes < datalen) {
		printk("less bytes written, %d bytes", wrbytes);
	} else if (wrbytes == -ENOTSUP) {
		printk("not implemented by underlying file system driver");
		res = -ENOTSUP;
	} else if (wrbytes < 0) {
		printk("could not write to file");
		res = -1;
	}

	if (res < 0) {
		wrbytes = res;
		goto err;
	}

	/* Close the file */
	res = fs_close(&zfp);
	if (res == 0) {
		printk("file closed");
	} else {
		printk("could not close file");
	}

err:
	return wrbytes;
}
