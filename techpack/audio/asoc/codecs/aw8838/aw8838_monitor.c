/*
 * awinic_monitor.c monitor_module
 *
 *
 * Copyright (c) 2019 AWINIC Technology CO., LTD
 *
 *  Author: Nick Li <liweilei@awinic.com.cn>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#include <linux/module.h>
#include <linux/i2c.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/of.h>
#include <linux/version.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>
#include <linux/power_supply.h>
#include "aw8838.h"
#include "aw8838_reg.h"
#include "aw8838_monitor.h"


#define AW_MONITOR_FILE "aw8838_monitor.bin"

/*DSP communication*/
#ifdef AW_QCOM_PLATFORM
extern int afe_get_topology(int port_id);
extern int aw_send_afe_cal_apr(uint32_t param_id,
	void *buf, int cmd_size, bool write);
#else
static int afe_get_topology(int port_id)
{
	return -EPERM;
}
static int aw_send_afe_cal_apr(uint32_t param_id,
	void *buf, int cmd_size, bool write)
{
	return 0;
}
#endif

static int aw_check_dsp_ready(void)
{
	int topo_id;

	topo_id = afe_get_topology(AFE_PORT_ID_AWDSP_RX);
	pr_debug("%s: topo_id 0x%x\n", __func__, topo_id);

	if (topo_id == AW_RX_TOPO_ID)
		return true;
	else
		return false;
}

static int aw_qcom_write_data_to_dsp(uint32_t param_id, void *data, int data_size)
{
	int try = 0;

	while (try < AW_DSP_TRY_TIME) {
		if (aw_check_dsp_ready()) {
			return aw_send_afe_cal_apr(param_id, data,
					data_size, true);
		} else {
			try++;
			msleep(AW_DSP_SLEEP_TIME);
			pr_err("%s: afe not ready try again\n", __func__);
		}
	}

	return -EINVAL;
}

static int aw_qcom_set_vmax_to_dsp(uint32_t vmax, int channel)
{
	uint32_t param_id;

	if (channel == AW_CHAN_LEFT)
		param_id = AFE_PARAM_ID_AWDSP_RX_VMAX_L;
	else
		param_id = AFE_PARAM_ID_AWDSP_RX_VMAX_R;

	return aw_qcom_write_data_to_dsp(param_id, &vmax, sizeof(vmax));
}

static int aw8838_monitor_get_voltage(struct aw8838 *aw8838,
						int *vbat)
{
	char name[] = "battery";
	int ret;
	union power_supply_propval prop = { 0 };
	struct power_supply *psy = NULL;

	aw_dev_info(aw8838->dev, "%s enter\n", __func__);

	psy = power_supply_get_by_name(name);
	if (psy) {
		ret = power_supply_get_property(psy,
						POWER_SUPPLY_PROP_VOLTAGE_NOW,
						&prop);
		if (ret < 0) {
			aw_dev_err(aw8838->dev,
				"%s: get voltage failed\n", __func__);
			return -EINVAL;
		}
		*vbat = prop.intval / 1000;
		aw_dev_info(aw8838->dev, "%s: get vbat is %d\n",
			__func__, *vbat);
	} else {
		aw_dev_err(aw8838->dev, "%s: no struct power supply name : %s\n",
			__func__, name);
		return -EINVAL;
	}

	return 0;
}

static int aw8838_monitor_get_temperature(struct aw8838 *aw8838,  int *temp)
{
#ifdef AW_TEMP_EN
	char name[] = "battery";
	int ret;
	union power_supply_propval sys_temp = { 0 };
	struct power_supply *psy = NULL;

	aw_dev_info(aw8838->dev, "%s enter\n", __func__);

	psy = power_supply_get_by_name(name);
	if (psy) {
		ret = power_supply_get_property(psy,
						POWER_SUPPLY_PROP_TEMP,
						&sys_temp);
		if (ret < 0) {
			aw_dev_err(aw8838->dev,
				"%s: get temperature failed\n", __func__);
			return -EINVAL;
		}
		*temp = sys_temp.intval / 10;
		aw_dev_info(aw8838->dev, "%s: get temperature is %d\n",
			__func__, *temp);
	} else {
		aw_dev_err(aw8838->dev, "%s: no struct power supply name : %s\n",
			__func__, name);
		return -EINVAL;
	}
#else
	aw_dev_info(aw8838->dev, "%s colse temp switch\n", __func__);
	aw8838->monitor.monitor_cfg.temp_switch = 0;
#endif
	return 0;
}

static int aw8838_monitor_get_temp_and_vol(struct aw8838 *aw8838)
{
	struct aw8838_monitor *monitor = &aw8838->monitor;
	unsigned int voltage = 0;
	int current_temp = 0;
	int ret = -1;

#ifdef AW_DEBUG
	if (monitor->test_vol == 0) {
		ret = aw8838_monitor_get_voltage(aw8838, &voltage);
		if (ret < 0)
			return ret;
	} else {
		voltage = monitor->test_vol;
	}

	if (monitor->test_temp == 0) {
		ret = aw8838_monitor_get_temperature(aw8838, &current_temp);
		if (ret)
			return ret;
	} else {
		current_temp = monitor->test_temp;
	}
#else
	ret = aw8838_monitor_get_voltage(aw8838, &voltage);
	if (ret < 0)
		return ret;

	ret = aw8838_monitor_get_temperature(aw8838, &current_temp);
	if (ret < 0)
		return ret;
#endif

	monitor->vol_trace.sum_val += voltage;
	monitor->temp_trace.sum_val += current_temp;
	monitor->samp_count++;

	return 0;
}

static int aw8838_monitor_trace_data_from_table(struct aw8838 *aw8838,
			struct aw_table_info table_info,
			struct aw_monitor_trace *data_trace)
{
	int i;

	if (table_info.aw_table == NULL) {
		aw_dev_err(aw8838->dev, "%s: table_info.aw_table is null\n",
			__func__);
		return -EINVAL;
	}

	for (i = 0; i < table_info.table_num; i++) {
		if (data_trace->sum_val >= table_info.aw_table[i].min_val &&
			data_trace->sum_val <= table_info.aw_table[i].max_val) {
			memcpy(&data_trace->aw_table, &table_info.aw_table[i],
				sizeof(struct aw_table));
			break;
		}
	}

	data_trace->pre_val = data_trace->sum_val;
	data_trace->sum_val = 0;
	return 0;
}

static int aw8838_monitor_first_get_data_form_table(struct aw8838 *aw8838,
				struct aw_table_info table_info,
			struct aw_monitor_trace *data_trace)
{
	int i;

	if (table_info.aw_table == NULL) {
		aw_dev_err(aw8838->dev, "%s: table_info.aw_table is null\n",
			__func__);
		return -EINVAL;
	}

	for (i = 0; i < table_info.table_num; i++) {
		if (data_trace->sum_val >= table_info.aw_table[i].min_val) {
			memcpy(&data_trace->aw_table, &table_info.aw_table[i],
				sizeof(struct aw_table));
			break;
		}
	}
	data_trace->pre_val = data_trace->sum_val;
	data_trace->sum_val = 0;
	return 0;
}

static int aw8838_monitor_get_data_from_table(struct aw8838 *aw8838,
					struct aw_table_info table_info,
					struct aw_monitor_trace *data_trace,
					uint32_t aplha)
{
	struct aw8838_monitor *monitor = &aw8838->monitor;

	if (monitor->first_entry == AW_FIRST_ENTRY) {
		return aw8838_monitor_first_get_data_form_table(aw8838,
						table_info, data_trace);
	} else {
		data_trace->sum_val = data_trace->sum_val / monitor->samp_count;
		data_trace->sum_val = ((int32_t)aplha * data_trace->sum_val +
			(1000 - (int32_t)aplha) * data_trace->pre_val) / 1000;
		return aw8838_monitor_trace_data_from_table(aw8838,
						table_info, data_trace);
	}

	return 0;
}

static int aw8838_monitor_get_data(struct aw8838 *aw8838)
{
	struct aw8838_monitor *monitor = &aw8838->monitor;
	struct aw_monitor_cfg *monitor_cfg = &monitor->monitor_cfg;
	struct aw_monitor_trace *vol_trace = &monitor->vol_trace;
	struct aw_monitor_trace *temp_trace = &monitor->temp_trace;
	int ret;

	if (monitor_cfg->vol_switch) {
		ret = aw8838_monitor_get_data_from_table(aw8838,
			monitor_cfg->vol_info, vol_trace,
			monitor_cfg->vol_aplha);
		if (ret < 0)
			return ret;
	} else {
		vol_trace->aw_table.ipeak = IPEAK_NONE;
		vol_trace->aw_table.gain = GAIN_NONE;
		vol_trace->aw_table.vmax = VMAX_NONE;
	}

	if (monitor_cfg->temp_switch) {
		ret = aw8838_monitor_get_data_from_table(aw8838,
			monitor_cfg->temp_info, temp_trace,
			monitor_cfg->temp_aplha);
		if (ret < 0)
			return ret;
	} else {
		temp_trace->aw_table.ipeak = IPEAK_NONE;
		temp_trace->aw_table.gain = GAIN_NONE;
		temp_trace->aw_table.vmax = VMAX_NONE;
	}

	aw_dev_info(aw8838->dev, "%s: vol: ipeak = 0x%x, gain = 0x%x, vmax = 0x%x\n",
			__func__, vol_trace->aw_table.ipeak,
			vol_trace->aw_table.gain,
			vol_trace->aw_table.vmax);

	aw_dev_info(aw8838->dev, "%s: temp: ipeak = 0x%x, gain = 0x%x, vmax = 0x%x\n",
			__func__, temp_trace->aw_table.ipeak,
			temp_trace->aw_table.gain,
			temp_trace->aw_table.vmax);

	return 0;
}

static void aw8838_monitor_get_cfg(struct aw8838 *aw8838,
					struct aw_table *set_table)
{
	struct aw8838_monitor *monitor = &aw8838->monitor;
	struct aw_table *temp_data = &monitor->temp_trace.aw_table;
	struct aw_table *vol_data = &monitor->vol_trace.aw_table;

	if (temp_data->ipeak == IPEAK_NONE && vol_data->ipeak == IPEAK_NONE) {
		memcpy(set_table, temp_data, sizeof(struct aw_table));
	} else if (temp_data->ipeak == IPEAK_NONE) {
		memcpy(set_table, vol_data, sizeof(struct aw_table));
	} else if (vol_data->ipeak == IPEAK_NONE) {
		memcpy(set_table, temp_data, sizeof(struct aw_table));
	} else {
		if (monitor->monitor_cfg.logic_switch == AW_MON_LOGIC_OR) {
			set_table->ipeak = (temp_data->ipeak < vol_data->ipeak ?
					temp_data->ipeak : vol_data->ipeak);
			set_table->gain = (temp_data->gain < vol_data->gain ?
					vol_data->gain : temp_data->gain);
			set_table->vmax = (temp_data->vmax < vol_data->vmax ?
					vol_data->vmax : temp_data->vmax);
		} else {
			set_table->ipeak = (temp_data->ipeak < vol_data->ipeak ?
					vol_data->ipeak : temp_data->ipeak);
			set_table->gain = (temp_data->gain < vol_data->gain ?
					temp_data->gain : vol_data->gain);
			set_table->vmax = (temp_data->vmax < vol_data->vmax ?
					temp_data->vmax : vol_data->vmax);
		}
	}
}

static void aw8838_monitor_set_ipeak(struct aw8838 *aw8838,
				uint16_t ipeak)
{
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;
	unsigned int reg_val = 0;
	unsigned int read_reg_val;
	int ret;

	if (ipeak == IPEAK_NONE || (!monitor_cfg->ipeak_switch))
		return;

	ret = aw8838_i2c_read(aw8838, AW8838_REG_CPCTRL, &reg_val);
	if (ret < 0) {
		aw_dev_err(aw8838->dev, "%s: read ipeak failed\n", __func__);
		return;
	}

	read_reg_val = reg_val;
	read_reg_val &= (~AW8838_BIT_CPCTRL_CP_IPEAK_MASK);

	if (read_reg_val == ipeak) {
		aw_dev_info(aw8838->dev, "%s: ipeak = 0x%x, no change\n",
					__func__, read_reg_val);
		return;
	}

	reg_val &= AW8838_BIT_CPCTRL_CP_IPEAK_MASK;
	read_reg_val = ipeak;
	reg_val |= read_reg_val;

	ret = aw8838_i2c_write(aw8838, AW8838_REG_CPCTRL, reg_val);
	if (ret < 0) {
		aw_dev_err(aw8838->dev, "%s: write ipeak failed\n", __func__);
		return;
	}

	aw_dev_info(aw8838->dev, "%s: set reg val = 0x%x, ipeak = 0x%x\n",
					__func__, reg_val, ipeak);
}

static void aw8838_monitor_set_gain(struct aw8838 *aw8838,
				uint16_t gain)
{
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;
	uint32_t read_volume;
	uint32_t set_volume;
	uint32_t gain_db;
	int ret;

	if (gain == GAIN_NONE || (!monitor_cfg->gain_switch))
		return;

	ret = aw8838_get_volume(aw8838, &read_volume);
	if (ret < 0) {
		aw_dev_err(aw8838->dev, "%s: read volume failed\n", __func__);
		return;
	}

	gain_db = aw8838_reg_val_to_db(gain);

	/*add offset*/
	set_volume = gain_db + aw8838->db_offset;

	if (read_volume == set_volume) {
		aw_dev_info(aw8838->dev, "%s: set db = %d.%d dB, no change\n",
				__func__, GET_DB_INT(read_volume),
				GET_DB_DECIMAL(read_volume));
		return;
	}

	ret = aw8838_set_volume(aw8838, set_volume);
	if (ret < 0) {
		aw_dev_err(aw8838->dev, "%s: set volume failed\n", __func__);
		return;
	}

	aw_dev_info(aw8838->dev, "%s: set_volume = %d.%d dB\n",
			__func__, GET_DB_INT(set_volume),
			GET_DB_DECIMAL(set_volume));

}

static void aw8838_monitor_set_vmax(struct aw8838 *aw8838,
						uint32_t vmax)
{
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;
	int ret;

	if (vmax == VMAX_NONE || (!monitor_cfg->vmax_switch))
		return;

	if ((aw8838->monitor.pre_vmax == vmax) &&
		(aw8838->monitor.first_entry != AW_FIRST_ENTRY)) {
		aw_dev_info(aw8838->dev, "%s: vmax no change\n",
			__func__);
		return;
	}

	/*left sound channel*/
	ret = aw_qcom_set_vmax_to_dsp(vmax, AW_CHAN_LEFT);
	if (ret) {
		aw_dev_err(aw8838->dev, "%s: dsp_msg_write error\n",
			__func__);
		return;
	}
	aw8838->monitor.pre_vmax = vmax;
	aw_dev_info(aw8838->dev, "%s: set vmax = 0x%x\n", __func__, vmax);
}

static int aw8838_monitor_work(struct aw8838 *aw8838)
{
	struct aw8838_monitor *monitor = &aw8838->monitor;
	struct aw_monitor_cfg *monitor_cfg = &monitor->monitor_cfg;
	struct aw_table set_table;
	int ret = -1;

	ret = aw8838_monitor_get_temp_and_vol(aw8838);
	if (ret < 0)
		return ret;

	if (monitor->samp_count < monitor_cfg->monitor_count &&
		(monitor->first_entry == AW_NOT_FIRST_ENTRY))
		return 0;

	ret = aw8838_monitor_get_data(aw8838);
	if (ret < 0)
		return ret;

	aw8838_monitor_get_cfg(aw8838, &set_table);

	aw_dev_dbg(aw8838->dev, "%s: set_ipeak = 0x%x, set_gain = 0x%x, set_vmax = 0x%x\n",
		__func__, set_table.ipeak, set_table.gain, set_table.vmax);

	aw8838_monitor_set_ipeak(aw8838, set_table.ipeak);

	aw8838_monitor_set_gain(aw8838, set_table.gain);

	aw8838_monitor_set_vmax(aw8838, set_table.vmax);

	monitor->samp_count = 0;

	if (monitor->first_entry == AW_FIRST_ENTRY)
		monitor->first_entry = AW_NOT_FIRST_ENTRY;

	return 0;
}

static void aw8838_monitor_work_func(struct work_struct *work)
{
	struct aw8838 *aw8838 = container_of(work,
			struct aw8838, monitor.delay_work.work);
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;

	aw_dev_dbg(aw8838->dev, "%s: monitor is_enable %d,scene_mode %d,monitor_status:%d, monitor_switch:%d\n",
		__func__, aw8838->monitor.is_enable, aw8838->spk_rcv_mode,
		monitor_cfg->monitor_status, monitor_cfg->monitor_switch);

	if (aw8838->monitor.is_enable &&
		(aw8838->spk_rcv_mode == AW8838_SPEAKER_MODE) &&
		(monitor_cfg->monitor_status == AW_MON_CFG_OK) &&
		monitor_cfg->monitor_switch) {
		if (!aw8838_get_hmute(aw8838)) {
			aw8838_monitor_work(aw8838);
			schedule_delayed_work(&aw8838->monitor.delay_work,
				msecs_to_jiffies(monitor_cfg->monitor_time));
		}
	}
}

void aw8838_monitor_start(struct aw8838_monitor *monitor)
{
	struct aw8838 *aw8838 = container_of(monitor,
			struct aw8838, monitor);

	aw_dev_info(aw8838->dev, "%s: enter\n", __func__);
	monitor->first_entry = AW_FIRST_ENTRY;
	monitor->samp_count = 0;
	monitor->vol_trace.sum_val = 0;
	monitor->temp_trace.sum_val = 0;
	aw8838->monitor.pre_vmax = 0;

	schedule_delayed_work(&aw8838->monitor.delay_work,
				msecs_to_jiffies(0));
}

void aw8838_monitor_stop(struct aw8838_monitor *monitor)
{
	struct aw8838 *aw8838 = container_of(monitor,
				struct aw8838, monitor);

	aw_dev_info(aw8838->dev, "%s: enter\n", __func__);
	cancel_delayed_work_sync(&aw8838->monitor.delay_work);
}


/*****************************************************
 * load monitor config
 *****************************************************/
static int aw_check_monitor_profile(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	struct aw_monitor_hdr *monitor_hdr =
		(struct aw_monitor_hdr *)cont->data;
	int temp_size, vol_size;

	if (cont->len < sizeof(struct aw_monitor_hdr)) {
		aw_dev_err(aw8838->dev,
			"%s:params size[%d] < struct aw_monitor_hdr size[%d]!\n",
			__func__, cont->len,
			(int)sizeof(struct aw_monitor_hdr));
		return -ENOMEM;
	}

	if (monitor_hdr->temp_offset > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:temp_offset[%d] overflow file size[%d]!\n",
			__func__, monitor_hdr->temp_offset, cont->len);
		return -ENOMEM;
	}

	if (monitor_hdr->vol_offset > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:vol_offset[%d] overflow file size[%d]!\n",
			__func__, monitor_hdr->vol_offset, cont->len);
		return -ENOMEM;
	}

	temp_size = monitor_hdr->temp_num * monitor_hdr->single_temp_size;
	if (temp_size > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:temp_size:[%d] overflow file size[%d]!!\n",
			__func__, temp_size, cont->len);
		return -ENOMEM;
	}

	vol_size = monitor_hdr->vol_num * monitor_hdr->single_vol_size;
	if (vol_size > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:vol_size:[%d] overflow file size[%d]!\n",
			__func__, vol_size, cont->len);
		return -ENOMEM;
	}

	return 0;
}

static int aw_check_monitor_profile_v_0_1_1(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	struct aw_monitor_hdr_v_0_1_1 *monitor_hdr =
		(struct aw_monitor_hdr_v_0_1_1 *)cont->data;
	int temp_size, vol_size;

	if (cont->len < sizeof(struct aw_monitor_hdr_v_0_1_1)) {
		aw_dev_err(aw8838->dev,
			"%s:params size[%d] < struct aw_monitor_hdr_v_0_1_1 size[%d]!\n",
			__func__, cont->len,
			(int)sizeof(struct aw_monitor_hdr_v_0_1_1));
		return -ENOMEM;
	}

	if (monitor_hdr->temp_offset > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:temp_offset[%d] overflow file size[%d]!\n",
			__func__, monitor_hdr->temp_offset, cont->len);
		return -ENOMEM;
	}

	if (monitor_hdr->vol_offset > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:vol_offset[%d] overflow file size[%d]!\n",
			__func__, monitor_hdr->vol_offset, cont->len);
		return -ENOMEM;
	}

	temp_size = monitor_hdr->temp_num * monitor_hdr->single_temp_size;
	if (temp_size > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:temp_size:[%d] overflow file size[%d]!!\n",
			__func__, temp_size, cont->len);
		return -ENOMEM;
	}

	vol_size = monitor_hdr->vol_num * monitor_hdr->single_vol_size;
	if (vol_size > cont->len) {
		aw_dev_err(aw8838->dev,
			"%s:vol_size:[%d] overflow file size[%d]!\n",
			__func__, vol_size, cont->len);
		return -ENOMEM;
	}

	return 0;
}


static void aw_parse_monitor_hdr(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	struct aw_monitor_hdr *monitor_hdr =
			(struct aw_monitor_hdr *)cont->data;
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;

	monitor_cfg->monitor_switch = monitor_hdr->monitor_switch;
	monitor_cfg->monitor_time = monitor_hdr->monitor_time;
	monitor_cfg->monitor_count = monitor_hdr->monitor_count;
	monitor_cfg->ipeak_switch = monitor_hdr->ipeak_switch;
	monitor_cfg->gain_switch = monitor_hdr->gain_switch;
	monitor_cfg->vmax_switch = monitor_hdr->vmax_switch;
	monitor_cfg->temp_switch = monitor_hdr->temp_switch;
	monitor_cfg->temp_aplha = monitor_hdr->temp_aplha;
	monitor_cfg->vol_switch = monitor_hdr->vol_switch;
	monitor_cfg->vol_aplha = monitor_hdr->vol_aplha;

	aw_dev_info(aw8838->dev, "%s: chip name:%s\n",
		__func__, monitor_hdr->chip_type);
	aw_dev_info(aw8838->dev, "%s: ui ver:0x%x\n",
		__func__, monitor_hdr->ui_ver);

	aw_dev_info(aw8838->dev,
		"%s:monitor_switch:%d, monitor_time:%d (ms), monitor_count:%d\n",
		__func__, monitor_cfg->monitor_switch,
		monitor_cfg->monitor_time, monitor_cfg->monitor_count);

	aw_dev_info(aw8838->dev,
		"%s:ipeak_switch:%d, gain_switch:%d, vmax_switch:%d\n",
		__func__, monitor_cfg->ipeak_switch,
		monitor_cfg->gain_switch, monitor_cfg->vmax_switch);

	aw_dev_info(aw8838->dev,
		"%s:temp_switch:%d, temp_aplha:%d, vol_switch:%d, vol_aplha:%d\n",
		__func__, monitor_cfg->temp_switch,
		monitor_cfg->temp_aplha, monitor_cfg->vol_switch,
		monitor_cfg->vol_aplha);
}

static void aw_parse_monitor_hdr_v_0_1_1(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	struct aw_monitor_hdr_v_0_1_1 *monitor_hdr =
			(struct aw_monitor_hdr_v_0_1_1 *)cont->data;
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;

	monitor_cfg->monitor_switch = (monitor_hdr->enable_flag >> MONITOR_EN_BIT) & MONITOR_EN_MASK;
	monitor_cfg->monitor_time = monitor_hdr->monitor_time;
	monitor_cfg->monitor_count = monitor_hdr->monitor_count;
	monitor_cfg->ipeak_switch = (monitor_hdr->enable_flag >> MONITOR_IPEAK_EN_BIT) & MONITOR_EN_MASK;
	monitor_cfg->logic_switch = (monitor_hdr->enable_flag >> MONITOR_LOGIC_BIT) & MONITOR_EN_MASK;
	monitor_cfg->gain_switch = (monitor_hdr->enable_flag >> MONITOR_GAIN_EN_BIT) & MONITOR_EN_MASK;
	monitor_cfg->vmax_switch = (monitor_hdr->enable_flag >> MONITOR_VMAX_EN_BIT) & MONITOR_EN_MASK;
	monitor_cfg->temp_switch = (monitor_hdr->enable_flag >> MONITOR_TEMP_EN_BIT) & MONITOR_EN_MASK;
	monitor_cfg->temp_aplha = monitor_hdr->temp_aplha;
	monitor_cfg->vol_switch = (monitor_hdr->enable_flag >> MONITOR_VOL_EN_BIT) & MONITOR_EN_MASK;
	monitor_cfg->vol_aplha = monitor_hdr->vol_aplha;

	aw_dev_info(aw8838->dev, "%s:chip name:%s\n",
		__func__, monitor_hdr->chip_type);
	aw_dev_info(aw8838->dev, "%s:ui ver:0x%x\n",
		__func__, monitor_hdr->ui_ver);

	aw_dev_info(aw8838->dev, "%s:monitor_switch:%d, monitor_time:%d (ms), monitor_count:%d\n",
		__func__, monitor_cfg->monitor_switch, monitor_cfg->monitor_time,
		monitor_cfg->monitor_count);

	aw_dev_info(aw8838->dev, "%s: logic_switch:%d, ipeak_switch:%d, gain_switch:%d, vmax_switch:%d\n",
		__func__, monitor_cfg->logic_switch, monitor_cfg->ipeak_switch, monitor_cfg->gain_switch,
		monitor_cfg->vmax_switch);

	aw_dev_info(aw8838->dev, "%s:temp_switch:%d, temp_aplha:%d, vol_switch:%d, vol_aplha:%d\n",
		__func__, monitor_cfg->temp_switch, monitor_cfg->temp_aplha,
		monitor_cfg->vol_switch, monitor_cfg->vol_aplha);

}

static void aw_populate_data_to_table(struct aw8838 *aw8838,
		struct aw_table_info *table_info, const char *offset_ptr)
{
	int i;

	for (i = 0; i < table_info->table_num * AW_TABLE_SIZE; i += AW_TABLE_SIZE) {
		table_info->aw_table[i / AW_TABLE_SIZE].min_val =
			GET_16_DATA(offset_ptr[1 + i], offset_ptr[i]);
		table_info->aw_table[i / AW_TABLE_SIZE].max_val =
			GET_16_DATA(offset_ptr[3 + i], offset_ptr[2 + i]);
		table_info->aw_table[i / AW_TABLE_SIZE].ipeak =
			GET_16_DATA(offset_ptr[5 + i], offset_ptr[4 + i]);
		table_info->aw_table[i / AW_TABLE_SIZE].gain =
			GET_16_DATA(offset_ptr[7 + i], offset_ptr[6 + i]);
		table_info->aw_table[i / AW_TABLE_SIZE].vmax =
			GET_32_DATA(offset_ptr[11 + i], offset_ptr[10 + i],
				offset_ptr[9 + i], offset_ptr[8 + i]);
	}

	for (i = 0; i < table_info->table_num; i++)
		aw_dev_info(aw8838->dev,
			"min_val:%d, max_val:%d, ipeak:0x%x, gain:0x%x, vmax:0x%x\n",
			table_info->aw_table[i].min_val,
			table_info->aw_table[i].max_val,
			table_info->aw_table[i].ipeak,
			table_info->aw_table[i].gain,
			table_info->aw_table[i].vmax);

}

static int aw_parse_temp_data(struct aw8838 *aw8838,
			struct aw8838_container *cont)
{
	struct aw_monitor_hdr *monitor_hdr =
			(struct aw_monitor_hdr *)cont->data;
	struct aw_table_info *temp_info =
		&aw8838->monitor.monitor_cfg.temp_info;

	aw_dev_info(aw8838->dev, "%s: ===parse temp start ===\n",
		__func__);

	if (temp_info->aw_table != NULL) {
		kfree(temp_info->aw_table);
		temp_info->aw_table = NULL;
	}

	temp_info->aw_table = kzalloc((monitor_hdr->temp_num * AW_TABLE_SIZE),
			GFP_KERNEL);
	if (!temp_info->aw_table)
		return -ENOMEM;

	temp_info->table_num = monitor_hdr->temp_num;
	aw_populate_data_to_table(aw8838, temp_info,
		&cont->data[monitor_hdr->temp_offset]);
	aw_dev_info(aw8838->dev, "%s: ===parse temp end ===\n",
		__func__);
	return 0;
}

static int aw_parse_temp_data_v_0_1_1(struct aw8838 *aw8838,
			struct aw8838_container *cont)
{
	struct aw_monitor_hdr_v_0_1_1 *monitor_hdr =
			(struct aw_monitor_hdr_v_0_1_1 *)cont->data;
	struct aw_table_info *temp_info =
		&aw8838->monitor.monitor_cfg.temp_info;

	aw_dev_info(aw8838->dev, "%s: ===parse temp start ===\n",
		__func__);

	if (temp_info->aw_table != NULL) {
		kfree(temp_info->aw_table);
		temp_info->aw_table = NULL;
	}

	temp_info->aw_table = kzalloc((monitor_hdr->temp_num * AW_TABLE_SIZE),
			GFP_KERNEL);
	if (!temp_info->aw_table)
		return -ENOMEM;

	temp_info->table_num = monitor_hdr->temp_num;
	aw_populate_data_to_table(aw8838, temp_info,
		&cont->data[monitor_hdr->temp_offset]);
	aw_dev_info(aw8838->dev, "%s: ===parse temp end ===\n",
		__func__);
	return 0;
}

static int aw_parse_vol_data(struct aw8838 *aw8838,
			struct aw8838_container *cont)
{
	struct aw_monitor_hdr *monitor_hdr =
			(struct aw_monitor_hdr *)cont->data;
	struct aw_table_info *vol_info =
		&aw8838->monitor.monitor_cfg.vol_info;

	aw_dev_info(aw8838->dev, "%s: ===parse vol start ===\n",
		__func__);

	if (vol_info->aw_table != NULL) {
		kfree(vol_info->aw_table);
		vol_info->aw_table = NULL;
	}

	vol_info->aw_table = kzalloc((monitor_hdr->vol_num * AW_TABLE_SIZE),
			GFP_KERNEL);
	if (!vol_info->aw_table)
		return -ENOMEM;

	vol_info->table_num = monitor_hdr->vol_num;
	aw_populate_data_to_table(aw8838, vol_info,
		&cont->data[monitor_hdr->vol_offset]);
	aw_dev_info(aw8838->dev, "%s: ===parse vol end ===\n",
		__func__);
	return 0;
}

static int aw_parse_vol_data_v_0_1_1(struct aw8838 *aw8838,
			struct aw8838_container *cont)
{
	struct aw_monitor_hdr_v_0_1_1 *monitor_hdr =
			(struct aw_monitor_hdr_v_0_1_1 *)cont->data;
	struct aw_table_info *vol_info =
		&aw8838->monitor.monitor_cfg.vol_info;

	aw_dev_info(aw8838->dev, "%s: ===parse vol start ===\n",
		__func__);

	if (vol_info->aw_table != NULL) {
		kfree(vol_info->aw_table);
		vol_info->aw_table = NULL;
	}

	vol_info->aw_table = kzalloc((monitor_hdr->vol_num * AW_TABLE_SIZE),
			GFP_KERNEL);
	if (!vol_info->aw_table)
		return -ENOMEM;

	vol_info->table_num = monitor_hdr->vol_num;
	aw_populate_data_to_table(aw8838, vol_info,
		&cont->data[monitor_hdr->vol_offset]);
	aw_dev_info(aw8838->dev, "%s: ===parse vol end ===\n",
		__func__);
	return 0;
}


static int aw_parse_monitor_data(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	int ret;
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;

	ret = aw_check_monitor_profile(aw8838, cont);
	if (ret < 0) {
		aw_dev_err(aw8838->dev, "%s:check %s failed\n",
				__func__, AW_MONITOR_FILE);
		return ret;
	}

	aw_parse_monitor_hdr(aw8838, cont);

	ret = aw_parse_temp_data(aw8838, cont);
	if (ret < 0)
		return ret;

	ret = aw_parse_vol_data(aw8838, cont);
	if (ret < 0) {
		if (monitor_cfg->temp_info.aw_table != NULL) {
			kfree(monitor_cfg->temp_info.aw_table);
			monitor_cfg->temp_info.aw_table = NULL;
			monitor_cfg->temp_info.table_num = 0;
		}
		return ret;
	}

	monitor_cfg->monitor_status = AW_MON_CFG_OK;
	return 0;
}

static int aw_parse_monitor_data_v_0_1_1(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	int ret;
	struct aw_monitor_cfg *monitor_cfg = &aw8838->monitor.monitor_cfg;

	ret = aw_check_monitor_profile_v_0_1_1(aw8838, cont);
	if (ret < 0) {
		aw_dev_err(aw8838->dev, "%s:check %s failed\n",
				__func__, AW_MONITOR_FILE);
		return ret;
	}

	aw_parse_monitor_hdr_v_0_1_1(aw8838, cont);

	ret = aw_parse_temp_data_v_0_1_1(aw8838, cont);
	if (ret < 0)
		return ret;

	ret = aw_parse_vol_data_v_0_1_1(aw8838, cont);
	if (ret < 0) {
		if (monitor_cfg->temp_info.aw_table != NULL) {
			kfree(monitor_cfg->temp_info.aw_table);
			monitor_cfg->temp_info.aw_table = NULL;
			monitor_cfg->temp_info.table_num = 0;
		}
		return ret;
	}

	monitor_cfg->monitor_status = AW_MON_CFG_OK;
	return 0;
}


static int aw_monitor_param_check_sum(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	int i, check_sum = 0;
	struct aw_monitor_hdr *monitor_hdr =
		(struct aw_monitor_hdr *)cont->data;

	if (cont->len < sizeof(struct aw_monitor_hdr)) {
		aw_dev_err(aw8838->dev, "%s:data size smaller than hdr , please check monitor bin\n",
					__func__);
		return -ENOMEM;
	}

	for (i = 4; i < cont->len; i++)
		check_sum += (uint8_t)cont->data[i];

	if (monitor_hdr->check_sum != check_sum) {
		aw_dev_err(aw8838->dev, "%s:check_sum[%d] is not equal to actual check_sum[%d]\n",
			__func__, monitor_hdr->check_sum, check_sum);
		return -ENOMEM;
	}

	return 0;
}

static int aw_parse_monitor_profile(struct aw8838 *aw8838,
					struct aw8838_container *cont)
{
	struct aw_monitor_hdr *monitor_hdr =
			(struct aw_monitor_hdr *)cont->data;
	int ret;

	ret = aw_monitor_param_check_sum(aw8838, cont);
	if (ret < 0)
		return ret;

	switch (monitor_hdr->monitor_ver) {
	case AW_MONITOR_HDR_VER_0_1_0:
		return aw_parse_monitor_data(aw8838, cont);
	case AW_MONITOR_HDR_VER_0_1_1:
		return aw_parse_monitor_data_v_0_1_1(aw8838, cont);
	default:
		aw_dev_err(aw8838->dev, "%s:cfg version:0x%x unsupported\n",
				__func__, monitor_hdr->monitor_ver);
		return -EINVAL;
	}
}

static void aw_monitor_profile_loaded(const struct firmware *cont,
					void *context)
{
	struct aw8838 *aw8838 = context;
	struct aw8838_container *monitor_cfg = NULL;
	int ret;

	if (aw8838->monitor.monitor_cfg.monitor_status == AW_MON_CFG_ST) {
		if (!cont) {
			aw_dev_err(aw8838->dev, "%s:failed to read %s\n",
					__func__, AW_MONITOR_FILE);
			goto exit;
		}

		aw_dev_info(aw8838->dev, "%s: loaded %s - size: %zu\n",
			__func__, AW_MONITOR_FILE, cont ? cont->size : 0);

		monitor_cfg = kzalloc(cont->size + sizeof(int), GFP_KERNEL);
		if (!monitor_cfg) {
			aw_dev_err(aw8838->dev, "%s: error allocating memory\n", __func__);
			goto exit;
		}
		monitor_cfg->len = cont->size;
		memcpy(monitor_cfg->data, cont->data, cont->size);
		ret = aw_parse_monitor_profile(aw8838, monitor_cfg);
		if (ret < 0)
			aw_dev_err(aw8838->dev, "%s:parse monitor cfg failed\n",
				__func__);
		kfree(monitor_cfg);
		monitor_cfg = NULL;
	}

exit:
	release_firmware(cont);
}

int aw8838_load_monitor_profile(struct aw8838_monitor *monitor)
{
	int ret;
	struct aw8838 *aw8838 = container_of(monitor,
					struct aw8838, monitor);

	if (!monitor->is_enable) {
		aw_dev_info(aw8838->dev, "%s: monitor flag:%d, monitor bin noload\n",
			__func__, monitor->is_enable);
		ret = 0;
	} else {
		ret = request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
				AW_MONITOR_FILE,
				aw8838->dev, GFP_KERNEL, aw8838,
				aw_monitor_profile_loaded);
	}

	return ret;
}

static void aw8838_monitor_load_fw_work_func(struct work_struct *work)
{
	struct aw8838_monitor *monitor = container_of(work,
				struct aw8838_monitor, load_fw_work.work);

	aw8838_load_monitor_profile(monitor);
}

void aw8838_deinit_monitor_profile(struct aw8838_monitor *monitor)
{
	struct aw_monitor_cfg *monitor_cfg = &monitor->monitor_cfg;

	monitor_cfg->monitor_status = AW_MON_CFG_ST;

	if (monitor_cfg->temp_info.aw_table != NULL) {
		kfree(monitor_cfg->temp_info.aw_table);
		monitor_cfg->temp_info.aw_table = NULL;
	}

	if (monitor_cfg->vol_info.aw_table != NULL) {
		kfree(monitor_cfg->vol_info.aw_table);
		monitor_cfg->vol_info.aw_table = NULL;
	}
	memset(monitor_cfg, 0, sizeof(struct aw_monitor_cfg));
}

#ifdef AW_DEBUG
static ssize_t aw8838_vol_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	uint32_t vol = 0;
	int ret = -1;

	if (count == 0)
		return 0;

	ret = kstrtouint(buf, 0, &vol);
	if (ret < 0)
		return ret;

	aw_dev_info(aw8838->dev, "%s: vol set =%d\n", __func__, vol);
	aw8838->monitor.test_vol = vol;

	return count;
}

static ssize_t aw8838_vol_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	ssize_t len = 0;
	uint32_t local_vol = aw8838->monitor.test_vol;

	len += snprintf(buf+len, PAGE_SIZE-len,
		"aw8838 vol: %d\n", local_vol);
	return len;
}

static ssize_t aw8838_temp_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	int32_t temp = 0;
	int ret = -1;

	if (count == 0)
		return 0;

	ret = kstrtoint(buf, 0, &temp);
	if (ret < 0)
		return ret;

	aw_dev_info(aw8838->dev, "%s: temp set =%d\n", __func__, temp);
	aw8838->monitor.test_temp = temp;

	return count;
}

static ssize_t aw8838_temp_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	ssize_t len = 0;
	int32_t local_temp = aw8838->monitor.test_temp;

	len += snprintf(buf+len, PAGE_SIZE-len,
		"aw8838 temp: %d\n", local_temp);
	return len;
}

static DEVICE_ATTR(vol, S_IWUSR | S_IRUGO,
	aw8838_vol_show, aw8838_vol_store);
static DEVICE_ATTR(temp, S_IWUSR | S_IRUGO,
	aw8838_temp_show, aw8838_temp_store);
#endif

static ssize_t aw8838_monitor_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	uint32_t enable = 0;
	int ret = -1;

	if (count == 0)
		return 0;

	ret = kstrtouint(buf, 0, &enable);
	if (ret < 0)
		return ret;

	aw_dev_info(aw8838->dev, "%s:monitor  enable set =%d\n",
		__func__, enable);
	aw8838->monitor.is_enable = enable;
	if (enable)
		schedule_delayed_work(&aw8838->monitor.delay_work,
					msecs_to_jiffies(0));

	return count;
}

static ssize_t aw8838_monitor_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	ssize_t len = 0;
	uint32_t local_enable;

	local_enable = aw8838->monitor.is_enable;
	len += snprintf(buf+len, PAGE_SIZE-len,
		"aw8838 monitor enable: %d\n", local_enable);
	return len;
}

static ssize_t aw_monitor_update_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct aw8838 *aw8838 = dev_get_drvdata(dev);
	uint32_t update = 0;
	int ret;

	if (count == 0)
		return 0;

	ret = kstrtouint(buf, 0, &update);
	if (ret < 0)
		return ret;

	aw_dev_info(aw8838->dev, "%s:monitor update = %d\n",
		__func__, update);

	aw8838_monitor_stop(&aw8838->monitor);

	aw8838_deinit_monitor_profile(&aw8838->monitor);
	aw8838_load_monitor_profile(&aw8838->monitor);
	msleep(100);
	aw8838_monitor_start(&aw8838->monitor);

	return count;
}

static DEVICE_ATTR(monitor, S_IWUSR | S_IRUGO,
	aw8838_monitor_show, aw8838_monitor_store);
static DEVICE_ATTR(monitor_update, S_IWUSR,
	NULL, aw_monitor_update_store);


static struct attribute *aw8838_monitor_attr[] = {
	&dev_attr_monitor.attr,
	&dev_attr_monitor_update.attr,
#ifdef AW_DEBUG
	&dev_attr_vol.attr,
	&dev_attr_temp.attr,
#endif
	NULL
};

static struct attribute_group aw8838_monitor_attr_group = {
	.attrs = aw8838_monitor_attr,
};

void aw8838_monitor_init(struct aw8838_monitor *monitor)
{
	int ret;
	struct aw8838 *aw8838 = container_of(monitor,
				struct aw8838, monitor);

	aw_dev_info(aw8838->dev, "%s: enter\n", __func__);

#ifdef AW_DEBUG
	monitor->test_vol = 0;
	monitor->test_temp = 0;
#endif

	INIT_DELAYED_WORK(&monitor->delay_work, aw8838_monitor_work_func);
	INIT_DELAYED_WORK(&monitor->load_fw_work, aw8838_monitor_load_fw_work_func);

	ret = sysfs_create_group(&aw8838->dev->kobj,
				&aw8838_monitor_attr_group);
	if (ret < 0)
		aw_dev_err(aw8838->dev, "%s error creating sysfs attr files\n",
			__func__);
}

void aw8838_monitor_deinit(struct aw8838_monitor *monitor)
{
	struct aw8838 *aw8838 = container_of(monitor,
			struct aw8838, monitor);

	aw8838_monitor_stop(monitor);
	aw8838_deinit_monitor_profile(monitor);

	sysfs_remove_group(&aw8838->dev->kobj, &aw8838_monitor_attr_group);
}

/*****************************************************
 * device tree parse monitor param
 *****************************************************/
void aw8838_parse_monitor_dt(struct aw8838_monitor *monitor)
{
	int ret;
	struct aw8838 *aw8838 = container_of(monitor,
				struct aw8838, monitor);
	struct device_node *np = aw8838->dev->of_node;

	ret = of_property_read_u32(np, "monitor-flag", &monitor->is_enable);
	if (ret) {
		monitor->is_enable = AW8838_MONITOR_DEFAULT_FLAG;
		aw_dev_info(aw8838->dev,
			"%s: monitor-flag get failed ,user default value!\n",
			__func__);
	} else {
		aw_dev_info(aw8838->dev, "%s: monitor-flag = %d\n",
			__func__, monitor->is_enable);
	}
}

