/*
 * app_sd_card.c
 *
 *  Created on: 21-Dec-2020
 *      Author: Rohan Dey
 */
#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <version.h>
#include <logging/log.h>
#include <stdlib.h>
#include <disk/disk_access.h>
#include <fs/fs.h>
#include <ff.h>

LOG_MODULE_REGISTER(app_sd_card);

static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = { .type = FS_FATFS, .fs_data = &fat_fs, };
/*
 *  Note the fatfs library is able to mount only strings inside _VOLUME_STRS in ffconf.h
 */
static const char *sd_mount_pt = "/SD:";

static int mount_sd_card() {
	int res = 0;
	do {
		static const char *disk_pdrv = "SD";
		uint64_t memory_size_mb;
		uint32_t block_count;
		uint32_t block_size;

		if (disk_access_init(disk_pdrv) != 0) {
			LOG_ERR("Storage init ERROR!");
			res = -1;
			break;
		}
		if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
			LOG_ERR("Unable to get sector count");
			res = -1;
			break;
		}
		LOG_DBG("Block count %u", block_count);

		if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
			LOG_ERR("Unable to get sector size");
			res = -1;
			break;
		}
		LOG_DBG("Sector size %u\n", block_size);

		memory_size_mb = (uint64_t) block_count * block_size;
		LOG_INF("Memory Size(MB) %u", (uint32_t )(memory_size_mb >> 20));
	} while (0);

	if (res != 0) {
		LOG_ERR("Error initializing disk.");
		return -1;
	}

	mp.mnt_point = sd_mount_pt;
	res = fs_mount(&mp);
	if (res == FR_OK) {
		LOG_INF("Disk mounted, mount point = %s", sd_mount_pt);
		return 0;
	} else {
		LOG_ERR("Error mounting disk.");
		return -1;
	}
}

static int unmount_sd_card() {

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

static int lsdir(const char *path) {
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
			printk("[FILE] %s (size = %zu)\n", entry.name, entry.size);
		}
	}

	/* Verify fs_closedir() */
	fs_closedir(&dirp);

	return res;
}

int list_files() {
	lsdir("/SD:");

	return 0;
}

int app_sd_init() {
	return mount_sd_card();
}

void app_sd_deinit() {
	unmount_sd_card();
}
