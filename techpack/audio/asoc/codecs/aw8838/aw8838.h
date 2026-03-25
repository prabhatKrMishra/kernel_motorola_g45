#ifndef _AW8838_H_
#define _AW8838_H_

#include "aw8838_monitor.h"
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 1)
#define AW_KERNEL_VER_OVER_4_19_1
#endif

/*
 * i2c transaction on Linux limited to 64k
 * (See Linux kernel documentation: Documentation/i2c/writing-clients)
*/
#define MAX_I2C_BUFFER_SIZE 65536

#define AW8838_FLAG_START_ON_MUTE   (1 << 0)
#define AW8838_FLAG_SKIP_INTERRUPTS     (1 << 1)
#define AW8838_FLAG_SAAM_AVAILABLE      (1 << 2)
#define AW8838_FLAG_STEREO_DEVICE       (1 << 3)
#define AW8838_FLAG_MULTI_MIC_INPUTS    (1 << 4)

#define AW8838_NUM_RATES                9

#define AW8838_MAX_REGISTER             0xff


enum aw8838_chipid{
    AW8838_ID,
};

enum aw8838_mode_spk_rcv{
    AW8838_SPEAKER_MODE = 0,
    AW8838_RECEIVER_MODE = 1,
};

/********************************************
 Compatible with codec and component
 *******************************************/

#ifdef AW_KERNEL_VER_OVER_4_19_1
typedef struct snd_soc_component aw_snd_soc_codec_t;
typedef struct snd_soc_component_driver aw_snd_soc_codec_driver_t;
#else
typedef struct snd_soc_codec aw_snd_soc_codec_t;
typedef struct snd_soc_codec_driver aw_snd_soc_codec_driver_t;
#endif

struct aw_componet_codec_ops {
	aw_snd_soc_codec_t *(*aw_snd_soc_kcontrol_codec)(struct snd_kcontrol *kcontrol);
	void *(*aw_snd_soc_codec_get_drvdata)(aw_snd_soc_codec_t *codec);
	int (*aw_snd_soc_add_codec_controls)(aw_snd_soc_codec_t *codec,
		const struct snd_kcontrol_new *controls, unsigned int num_controls);
	void (*aw_snd_soc_unregister_codec)(struct device *dev);
	int (*aw_snd_soc_register_codec)(struct device *dev,
			const aw_snd_soc_codec_driver_t *codec_drv,
			struct snd_soc_dai_driver *dai_drv,
			int num_dai);
};

struct aw8838 {
    struct regmap *regmap;
    struct i2c_client *i2c;
    aw_snd_soc_codec_t *codec;
    struct device *dev;
    struct mutex cfg_lock;
#ifdef AW8838_VBAT_MONITOR
    struct hrtimer vbat_monitor_timer;
    struct work_struct vbat_monitor_work;
#endif
    int sysclk;
    int rate;
    int pstream;
    int cstream;

    int reset_gpio;
    int irq_gpio;

#ifdef CONFIG_DEBUG_FS
    struct dentry *dbg_dir;
#endif
    u8 reg;

    unsigned int flags;
    unsigned int chipid;
    unsigned int init;
    unsigned int spk_rcv_mode;
    struct aw8838_monitor monitor;
    uint32_t db_offset;
    unsigned int cpmd_default;
};

struct aw8838_container{
    int len;
    unsigned char data[];
};

int aw8838_i2c_read(struct aw8838 *aw8838,
        unsigned char reg_addr, unsigned int *reg_data);
int aw8838_i2c_write(struct aw8838 *aw8838,
        unsigned char reg_addr, unsigned int reg_data);
int aw8838_get_volume(struct aw8838 *aw8838, uint32_t *value);
int aw8838_set_volume(struct aw8838 *aw8838, uint32_t value);
uint32_t aw8838_reg_val_to_db(uint32_t value);
int aw8838_get_hmute(struct aw8838 *aw8838);

#endif
