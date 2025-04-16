#ifndef __AWINIC_MONITOR_H__
#define __AWINIC_MONITOR_H__

/*#define AW_DEBUG*/
/*#define AW_TEMP_EN*/
/*#define AW_QCOM_PLATFORM*/

#define AFE_PORT_ID_AWDSP_RX			(0x1006)	/*AFE_PORT_ID_QUATERNARY_MI2S_RX*/
#define AW_RX_TOPO_ID				(0x1000FF01)	/*RX topology id*/
#define AW_DSP_SLEEP_TIME (10)
#define AW_DSP_TRY_TIME (3)


struct aw_table;

#define AW_TABLE_SIZE	sizeof(struct aw_table)
#define AFE_PARAM_ID_AWDSP_RX_VMAX_L			(0X10013D17)
#define AFE_PARAM_ID_AWDSP_RX_VMAX_R			(0X10013D18)


#define IPEAK_NONE	(0xFF)
#define GAIN_NONE	(0xFF)
#define VMAX_NONE	(0xFFFFFFFF)
#define GET_DB_INT(x)	(x / 2)
#define GET_DB_DECIMAL(x)	((x % 2) * 5)
#define AW_LOAD_MON_FW_DELAY_TIME	(3000)
#define AW8838_MONITOR_DEFAULT_FLAG	(0)

#define GET_32_DATA(w, x, y, z) \
	((uint32_t)((((uint8_t)w) << 24) | (((uint8_t)x) << 16) | (((uint8_t)y) << 8) | ((uint8_t)z)))
#define GET_16_DATA(x, y) \
	((uint16_t)((((uint8_t)x) << 8) | (uint8_t)y))

#define MONITOR_EN_MASK  0x01

enum {
	AW_CHAN_LEFT = 0,
	AW_CHAN_RIGHT = 1,
};

enum {
	AW_MON_LOGIC_OR = 0,
	AW_MON_LOGIC_AND = 1,
};

enum {
	MONITOR_EN_BIT = 0,
	MONITOR_LOGIC_BIT = 1,
	MONITOR_IPEAK_EN_BIT =2,
	MONITOR_GAIN_EN_BIT = 3,
	MONITOR_VMAX_EN_BIT = 4,
	MONITOR_TEMP_EN_BIT = 5,
	MONITOR_VOL_EN_BIT = 6,
};

enum aw_monitor_hdr_ver {
	AW_MONITOR_HDR_VER_0_1_0 = 0x00010000,
	AW_MONITOR_HDR_VER_0_1_1 = 0x00010100,
};

enum {
	AW_FIRST_ENTRY = 0,
	AW_NOT_FIRST_ENTRY = 1,
};

struct aw_table {
	int16_t min_val;
	int16_t max_val;
	uint16_t ipeak;
	uint16_t gain;
	uint32_t vmax;
};

struct aw_table_info {
	uint8_t table_num;
	struct aw_table *aw_table;
};


enum aw_monitor_init {
	AW_MON_CFG_ST = 0,
	AW_MON_CFG_OK = 1,
};

struct aw_monitor_cfg {
	uint8_t monitor_status;
	uint32_t monitor_switch;
	uint32_t monitor_time;
	uint32_t monitor_count;
	uint32_t logic_switch;
	uint32_t temp_switch;
	uint32_t temp_aplha;
	uint32_t vol_switch;
	uint32_t vol_aplha;
	uint32_t ipeak_switch;
	uint32_t gain_switch;
	uint32_t vmax_switch;
	struct aw_table_info temp_info;
	struct aw_table_info vol_info;
};

struct aw_monitor_hdr {
	uint32_t check_sum;
	uint32_t monitor_ver;
	char chip_type[8];
	uint32_t ui_ver;
	uint32_t monitor_switch;
	uint32_t monitor_time;
	uint32_t monitor_count;
	uint32_t ipeak_switch;
	uint32_t gain_switch;
	uint32_t vmax_switch;
	uint32_t temp_switch;
	uint32_t temp_aplha;
	uint32_t temp_num;
	uint32_t single_temp_size;
	uint32_t temp_offset;
	uint32_t vol_switch;
	uint32_t vol_aplha;
	uint32_t vol_num;
	uint32_t single_vol_size;
	uint32_t vol_offset;
};

struct aw_monitor_hdr_v_0_1_1 {
	uint32_t check_sum;
	uint32_t monitor_ver;
	char chip_type[16];
	uint32_t ui_ver;
	uint32_t monitor_time;
	uint32_t monitor_count;
	uint32_t enable_flag;
		// [bit 31:7]
		// [bit 6: vol en]
		// [bit 5: temp en]
		// [bit 4: vmax en]
		// [bit 3: gain en]
		// [bit 2: ipeak en]
		// [bit 1: & or | flag]
		// [bit 0: monitor en]
	uint32_t temp_aplha;
	uint32_t temp_num;
	uint32_t single_temp_size;
	uint32_t temp_offset;
	uint32_t vol_aplha;
	uint32_t vol_num;
	uint32_t single_vol_size;
	uint32_t vol_offset;
	uint32_t reserver[3];
};

struct aw_monitor_trace {
	int32_t pre_val;
	int32_t sum_val;
	struct aw_table aw_table;
};

/******************************************************************
* struct aw882xx monitor
*******************************************************************/
struct aw8838_monitor {
	struct delayed_work delay_work;
	struct delayed_work load_fw_work;
	struct aw_monitor_cfg monitor_cfg;
	uint32_t is_enable;
	uint8_t first_entry;
	uint8_t samp_count;
	uint32_t pre_vmax;

	struct aw_monitor_trace temp_trace;
	struct aw_monitor_trace vol_trace;
#ifdef AW_DEBUG
	uint16_t test_vol;
	int16_t test_temp;
#endif
};

/******************************************************************
* aw8838 monitor functions
*******************************************************************/
void aw8838_monitor_start(struct aw8838_monitor *monitor);
void aw8838_monitor_stop(struct aw8838_monitor *monitor);
void aw8838_parse_monitor_dt(struct aw8838_monitor *monitor);
void aw8838_monitor_init(struct aw8838_monitor *monitor);
void aw8838_monitor_deinit(struct aw8838_monitor *monitor);

/********************************************
 * print information control
 *******************************************/
#define aw_dev_err(dev, format, ...) \
			pr_err("[%s]" format, dev_name(dev), ##__VA_ARGS__)

#define aw_dev_info(dev, format, ...) \
			pr_info("[%s]" format, dev_name(dev), ##__VA_ARGS__)

#define aw_dev_dbg(dev, format, ...) \
			pr_debug("[%s]" format, dev_name(dev), ##__VA_ARGS__)


#endif

