#include <linux/string.h>
#include <linux/types.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/printk.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/suspicious.h>

#include "mount.h"

#define uid_matches() (getuid() >= 2000)

static const char* const suspicious_paths[] = {
	"/storage/emulated/0/TWRP",
	"/system/lib/libzygisk.so",
	"/system/lib64/libzygisk.so",
	"/dev/zygisk",
	"/system/addon.d",
	"/vendor/bin/install-recovery.sh",
	"/system/bin/install-recovery.sh",
	"/debug_ramdisk",
	"/data/adb/modules",
	"/data/adb/modules_update",
	"/data/adb/ksud",
	"/data/adb/kernelsu",
	"/data/adb/magisk",
	"/apex/com.android.art/bin/dex2oat32",
	"/apex/com.android.art/bin/dex2oat64",
	"/system/etc/preloaded-classes",
	"/system/bin/su",
	"/system/xbin/su",
	"/vendor/bin/su",
	"/sbin/su",
	"/proc/ksu",
	"/proc/magisk",
	"/sys/fs/selinux/policy",
	"/sys/fs/selinux/enforce",
	"/init.magisk.rc"
};

static const char* const suspicious_mount_types[] = {
	"overlay"  // Only overlays are suspicious in context
};

static const char* const suspicious_mount_paths[] = {
	"/data/adb/modules",           // Direct module storage
	"/data/adb/modules_update",    // Direct module storage
	"/debug_ramdisk",              // KSU ramdisk
	"/data/adb/ksu",               // KSU data
	"/data/adb/zygisksu",          // Zygisk data
	"/system/etc/preloaded-classes" // Common modification target
};

static const char* const suspicious_mount_devices[] = {
	"KSU"  // Only explicit KSU devices
};

static uid_t getuid(void) {

	const struct cred* const credentials = current_cred();

	if (credentials == NULL) {
		return 0;
	}

	return credentials->uid.val;

}

int is_suspicious_path(const struct path* const file)
{

	size_t index = 0, size = 4096;
	int res = -1, status = 0;
	char *path = NULL, *ptr = NULL, *end = NULL;

	if (!uid_matches() || file == NULL) {
		status = 0;
		goto out;
	}

	path = kmalloc(size, GFP_KERNEL);

	if (path == NULL) {
		status = -1;
		goto out;
	}

	ptr = d_path(file, path, size);

	if (IS_ERR(ptr)) {
		status = -1;
		goto out;
	}

	end = mangle_path(path, ptr, " \t\n\\");

	if (!end) {
		status = -1;
		goto out;
	}

	res = end - path;
	path[(size_t) res] = '\0';

	for (index = 0; index < ARRAY_SIZE(suspicious_paths); index++) {
		const char* const name = suspicious_paths[index];

		if (memcmp(name, path, strlen(name)) == 0) {
			printk(KERN_INFO "suspicious-fs: file or directory access to suspicious path '%s' won't be allowed to process with UID %i\n", name, getuid());

			status = 1;
			goto out;
		}
	}

out:
	kfree(path);

	return status;

}

int suspicious_path(const struct filename* const name)
{

	int status = 0, ret = 0;
	struct path path;

	if (IS_ERR(name)) {
		return -1;
	}

	if (!uid_matches() || name == NULL) {
		return 0;
	}

	ret = kern_path(name->name, LOOKUP_FOLLOW, &path);

	if (!ret) {
		status = is_suspicious_path(&path);
		path_put(&path);
	}

	return status;

}

int is_suspicious_mount(struct vfsmount* const mnt, const struct path* const root)
{
	size_t index = 0, size = 4096;
	int res = -1, status = 0;
	char* path = NULL, *ptr = NULL, *end = NULL;

	struct path mnt_path = {
		.dentry = mnt->mnt_root,
		.mnt = mnt
	};

	struct mount* real = real_mount(mnt);

	if (!uid_matches()) {
		status = 0;
		goto out;
	}

	// only check for KSU device name (most specific)
	if (real->mnt_devname != NULL && strstr(real->mnt_devname, "KSU")) {
		printk(KERN_INFO "suspicious-fs: hiding KSU device: %s\n", real->mnt_devname);
		status = 1;
		goto out;
	}

	path = kmalloc(size, GFP_KERNEL);
	if (path == NULL) {
		status = -1;
		goto out;
	}

	ptr = __d_path(&mnt_path, root, path, size);
	if (!ptr) {
		status = -1;
		goto out;
	}

	end = mangle_path(path, ptr, " \t\n\\");
	if (!end) {
		status = -1;
		goto out;
	}

	res = end - path;
	path[(size_t) res] = '\0';

	for (index = 0; index < ARRAY_SIZE(suspicious_mount_paths); index++) {
		const char* const name = suspicious_mount_paths[index];

		// Only hide if path matches exactly or is a subdirectory
		if (strstr(path, name)) {
			printk(KERN_INFO "suspicious-fs: hiding specific path: %s\n", path);
			status = 1;
			goto out;
		}
	}

	if (strcmp(mnt->mnt_root->d_sb->s_type->name, "overlay") == 0) {
		// For overlays, check if they're mounted on system partitions
		// This is less precise but much safer and more reliable
		if (strstr(path, "/system") || strstr(path, "/product") ||
		        strstr(path, "/vendor") || strstr(path, "/system_ext")) {
			printk(KERN_INFO "suspicious-fs: hiding system overlay: %s\n", path);
			status = 1;
			goto out;
		}
	}

out:
	kfree(path);
	return status;
}
