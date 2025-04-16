#ifndef _AW8838_REG_H_
#define _AW8838_REG_H_

/********************************************
 * Register List
 *******************************************/
#define AW8838_REG_ID           0x00
#define AW8838_REG_SYSST        0x01
#define AW8838_REG_SYSINT       0x02
#define AW8838_REG_SYSINTM      0x03
#define AW8838_REG_SYSCTRL      0x04
#define AW8838_REG_I2SCTRL      0x05
#define AW8838_REG_I2STXCFG     0x06
#define AW8838_REG_PWMCTRL      0x08
#define AW8838_REG_HAGCCFG0     0x10
#define AW8838_REG_HAGCCFG1     0x11
#define AW8838_REG_HAGCCFG2     0x12
#define AW8838_REG_HAGCCFG3     0x13
#define AW8838_REG_HAGCCFG4     0x14
#define AW8838_REG_HAGCCFG5     0x15
#define AW8838_REG_HAGCCFG6     0x16
#define AW8838_REG_HAGCCFG7     0x17
#define AW8838_REG_HAGCCFG8     0x18
#define AW8838_REG_DBGCTRL      0x20
#define AW8838_REG_I2SCFG       0x21
#define AW8838_REG_I2SSTAT      0x22
#define AW8838_REG_I2SCAPCNT    0x23
#define AW8838_REG_HPFCCFG1     0x50
#define AW8838_REG_HPFCCFG2     0x51
#define AW8838_REG_HPFCCFG3     0x52
#define AW8838_REG_HPFCCFG4     0x53
#define AW8838_REG_HPFCCFG5     0x54
#define AW8838_REG_CPCTRL       0x60
#define AW8838_REG_PLLCTRL1     0x61
#define AW8838_REG_PLLCTRL2     0x62
#define AW8838_REG_AMPDBG1      0x63
#define AW8838_REG_AMPDBG2      0x64
#define AW8838_REG_TESTCTRL     0x65

#define AW8838_REG_MAX          0x6F

/********************************************
 * Register Access
 *******************************************/
#define REG_NONE_ACCESS 0
#define REG_RD_ACCESS  1 << 0
#define REG_WR_ACCESS  1 << 1

static const unsigned char aw8838_reg_access[AW8838_REG_MAX]={
    [AW8838_REG_ID       ] = REG_RD_ACCESS,
    [AW8838_REG_SYSST    ] = REG_RD_ACCESS,
    [AW8838_REG_SYSINT   ] = REG_RD_ACCESS,
    [AW8838_REG_SYSINTM  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_SYSCTRL  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_I2SCTRL  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_I2STXCFG ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_PWMCTRL  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG0 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG1 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG2 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG3 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG4 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG5 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG6 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG7 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HAGCCFG8 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_DBGCTRL  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_I2SCFG   ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_I2SSTAT  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_I2SCAPCNT] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HPFCCFG1 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HPFCCFG2 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HPFCCFG3 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HPFCCFG4 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_HPFCCFG5 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_CPCTRL   ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_PLLCTRL1 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_PLLCTRL2 ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_AMPDBG1  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_AMPDBG2  ] = REG_RD_ACCESS|REG_WR_ACCESS,
    [AW8838_REG_TESTCTRL ] = REG_RD_ACCESS|REG_WR_ACCESS,
};

/******************************************************
 * Register Detail
 *****************************************************/
// SYSST
#define AW8838_BIT_SYSST_UVLOS                      ( 1<<14)
#define AW8838_BIT_SYSST_ADPS                       ( 1<<13)
#define AW8838_BIT_SYSST_OVPS                       ( 1<<11)
#define AW8838_BIT_SYSST_HVS                        ( 1<<10)
#define AW8838_BIT_SYSST_VOS                        ( 1<< 9)
#define AW8838_BIT_SYSST_SWS                        ( 1<< 8)
#define AW8838_BIT_SYSST_NOCLKS                     ( 1<< 5)
#define AW8838_BIT_SYSST_CLKS                       ( 1<< 4)
#define AW8838_BIT_SYSST_OCDS                       ( 1<< 3)
#define AW8838_BIT_SYSST_OTLS                       ( 1<< 2)
#define AW8838_BIT_SYSST_OTHS                       ( 1<< 1)
#define AW8838_BIT_SYSST_PLLS                       ( 1<< 0)

// SYSINT
#define AW8838_BIT_SYSINT_UVLOI                     ( 1<<14)
#define AW8838_BIT_SYSINT_ADPI                      ( 1<<13)
#define AW8838_BIT_SYSINT_OVPI                      ( 1<<11)
#define AW8838_BIT_SYSINT_HVI                       ( 1<<10)
#define AW8838_BIT_SYSINT_VOI                       ( 1<< 9)
#define AW8838_BIT_SYSINT_SWI                       ( 1<< 8)
#define AW8838_BIT_SYSINT_NOCLKI                    ( 1<< 5)
#define AW8838_BIT_SYSINT_CLKI                      ( 1<< 4)
#define AW8838_BIT_SYSINT_OCDI                      ( 1<< 3)
#define AW8838_BIT_SYSINT_OTLI                      ( 1<< 2)
#define AW8838_BIT_SYSINT_OTHI                      ( 1<< 1)
#define AW8838_BIT_SYSINT_PLLI                      ( 1<< 0)

// SYSINTM
#define AW8838_BIT_SYSINTM_UVLOM                    ( 1<<14)
#define AW8838_BIT_SYSINTM_ADPM                     ( 1<<13)
#define AW8838_BIT_SYSINTM_OVPM                     ( 1<<11)
#define AW8838_BIT_SYSINTM_HVM                      ( 1<<10)
#define AW8838_BIT_SYSINTM_VOM                      ( 1<< 9)
#define AW8838_BIT_SYSINTM_SWM                      ( 1<< 8)
#define AW8838_BIT_SYSINTM_NOCLKM                   ( 1<< 5)
#define AW8838_BIT_SYSINTM_CLKM                     ( 1<< 4)
#define AW8838_BIT_SYSINTM_OCDM                     ( 1<< 3)
#define AW8838_BIT_SYSINTM_OTLM                     ( 1<< 2)
#define AW8838_BIT_SYSINTM_OTHM                     ( 1<< 1)
#define AW8838_BIT_SYSINTM_PLLM                     ( 1<< 0)

// SYSCTRL
#define AW8838_BIT_SYSCTRL_INTMODE_MASK             (~( 3<< 8))
#define AW8838_BIT_SYSCTRL_INT_HIGH_PP              ( 3<< 8)
#define AW8838_BIT_SYSCTRL_INT_LOW_PP               ( 2<< 8)
#define AW8838_BIT_SYSCTRL_INT_HIGH_OD              ( 1<< 8)
#define AW8838_BIT_SYSCTRL_INT_LOW_OD               ( 0<< 8)
#define AW8838_BIT_SYSCTRL_MODE_MASK                (~( 1<< 7))
#define AW8838_BIT_SYSCTRL_RCV_MODE                 ( 1<< 7)
#define AW8838_BIT_SYSCTRL_SPK_MODE                 ( 0<< 7)
#define AW8838_BIT_SYSCTRL_I2SEN_MASK               (~( 1<< 6))
#define AW8838_BIT_SYSCTRL_I2S_ENABLE               ( 1<< 6)
#define AW8838_BIT_SYSCTRL_I2S_DISABLE              ( 0<< 6)
#define AW8838_BIT_SYSCTRL_WSINV_MASK               (~( 1<< 5))
#define AW8838_BIT_SYSCTRL_WS_INVERT                ( 1<< 5)
#define AW8838_BIT_SYSCTRL_WS_NO_INVERT             ( 0<< 5)
#define AW8838_BIT_SYSCTRL_BCKINV_MASK              (~( 1<< 4))
#define AW8838_BIT_SYSCTRL_BCK_INVERT               ( 1<< 4)
#define AW8838_BIT_SYSCTRL_BCK_NO_INVERT            ( 0<< 4)
#define AW8838_BIT_SYSCTRL_IPLL_MASK                (~( 1<< 3))
#define AW8838_BIT_SYSCTRL_PLL_WORD                 ( 1<< 3)
#define AW8838_BIT_SYSCTRL_PLL_BIT                  ( 0<< 3)
#define AW8838_BIT_SYSCTRL_DSPBY_MASK               (~( 1<< 2))
#define AW8838_BIT_SYSCTRL_DSP_BYPASS               ( 1<< 2)
#define AW8838_BIT_SYSCTRL_DSP_WORK                 ( 0<< 2)
#define AW8838_BIT_SYSCTRL_CP_MASK                  (~( 1<< 1))
#define AW8838_BIT_SYSCTRL_CP_PDN                   ( 1<< 1)
#define AW8838_BIT_SYSCTRL_CP_ACTIVE                ( 0<< 1)
#define AW8838_BIT_SYSCTRL_PW_MASK                  (~( 1<< 0))
#define AW8838_BIT_SYSCTRL_PW_PDN                   ( 1<< 0)
#define AW8838_BIT_SYSCTRL_PW_ACTIVE                ( 0<< 0)
#define AW8838_BIT_SYSCTRL_CPPD_MASK                 (~( 1<< 1))
#define AW8838_BIT_SYSCTRL_CPPD_PDN                  ( 1<< 1)
#define AW8838_BIT_SYSCTRL_CPPD_ACTIVE               ( 0<< 1)

// I2SCTRL
#define AW8838_BIT_I2SCTRL_INPLEV_MASK              (~( 1<<13))
#define AW8838_BIT_I2SCTRL_INPLEV_0DB               ( 1<<13)
#define AW8838_BIT_I2SCTRL_INPLEV_NEG_6DB           ( 0<<13)
#define AW8838_BIT_I2SCTRL_STEREO_MASK              (~( 1<<12))
#define AW8838_BIT_I2SCTRL_STEREO_ENABLE            ( 1<<12)
#define AW8838_BIT_I2SCTRL_STEREO_DISABLE           ( 0<<12)
#define AW8838_BIT_I2SCTRL_CHS_MASK                 (~( 3<<10))
#define AW8838_BIT_I2SCTRL_CHS_MONO                 ( 3<<10)
#define AW8838_BIT_I2SCTRL_CHS_RIGHT                ( 2<<10)
#define AW8838_BIT_I2SCTRL_CHS_LEFT                 ( 1<<10)
#define AW8838_BIT_I2SCTRL_MD_MASK                  (~( 3<< 8))
#define AW8838_BIT_I2SCTRL_MD_LSB                   ( 2<< 8)
#define AW8838_BIT_I2SCTRL_MD_MSB                   ( 1<< 8)
#define AW8838_BIT_I2SCTRL_MD_STD                   ( 0<< 8)
#define AW8838_BIT_I2SCTRL_FMS_MASK                 (~( 3<< 6))
#define AW8838_BIT_I2SCTRL_FMS_32BIT                ( 3<< 6)
#define AW8838_BIT_I2SCTRL_FMS_24BIT                ( 2<< 6)
#define AW8838_BIT_I2SCTRL_FMS_20BIT                ( 1<< 6)
#define AW8838_BIT_I2SCTRL_FMS_16BIT                ( 0<< 6)
#define AW8838_BIT_I2SCTRL_BCK_MASK                 (~( 3<< 4))
#define AW8838_BIT_I2SCTRL_BCK_64FS                 ( 2<< 4)
#define AW8838_BIT_I2SCTRL_BCK_48FS                 ( 1<< 4)
#define AW8838_BIT_I2SCTRL_BCK_32FS                 ( 0<< 4)
#define AW8838_BIT_I2SCTRL_SR_MASK                  (~(15<< 0))
#define AW8838_BIT_I2SCTRL_SR_192K                  (10<< 0)
#define AW8838_BIT_I2SCTRL_SR_96K                   ( 9<< 0)
#define AW8838_BIT_I2SCTRL_SR_48K                   ( 8<< 0)
#define AW8838_BIT_I2SCTRL_SR_44P1K                 ( 7<< 0)
#define AW8838_BIT_I2SCTRL_SR_32K                   ( 6<< 0)
#define AW8838_BIT_I2SCTRL_SR_24K                   ( 5<< 0)
#define AW8838_BIT_I2SCTRL_SR_22K                   ( 4<< 0)
#define AW8838_BIT_I2SCTRL_SR_16K                   ( 3<< 0)
#define AW8838_BIT_I2SCTRL_SR_12K                   ( 2<< 0)
#define AW8838_BIT_I2SCTRL_SR_11K                   ( 1<< 0)
#define AW8838_BIT_I2SCTRL_SR_8K                    ( 0<< 0)


// I2STXCFG
#define AW8838_BIT_I2STXCFG_FSYNC_MASK              (~( 1<<15))
#define AW8838_BIT_I2STXCFG_FSYNC_BCK_CYCLE         ( 1<<15)
#define AW8838_BIT_I2STXCFG_FSYNC_ONE_SLOT          ( 0<<15)
#define AW8838_BIT_I2STXCFG_SLOT_NUM_MASK           (~( 1<<14))
#define AW8838_BIT_I2STXCFG_SLOT_NUM_4_TIMES        ( 1<<14)
#define AW8838_BIT_I2STXCFG_SLOT_NUM_2_TIMES        ( 0<<14)
#define AW8838_BIT_I2STXCFG_TX_SLOT_VLD_MASK        (~(15<<12))
#define AW8838_BIT_I2STXCFG_TX_SLOT_VLD_3           ( 3<<12)
#define AW8838_BIT_I2STXCFG_TX_SLOT_VLD_2           ( 2<<12)
#define AW8838_BIT_I2STXCFG_TX_SLOT_VLD_1           ( 1<<12)
#define AW8838_BIT_I2STXCFG_TX_SLOT_VLD_0           ( 0<<12)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_MASK        (~(15<< 8))
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_3_2         (12<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_3_1         (10<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_3_0         ( 9<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_2_1         ( 6<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_2_0         ( 5<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_1_0         ( 3<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_3           ( 8<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_2           ( 4<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_1           ( 2<< 8)
#define AW8838_BIT_I2STXCFG_RX_SLOT_VLD_0           ( 1<< 8)
#define AW8838_BIT_I2STXCFG_DRVSTREN_MASK           (~( 1<< 5))
#define AW8838_BIT_I2STXCFG_DRVSTREN_8MA            ( 1<< 5)
#define AW8838_BIT_I2STXCFG_DRVSTREN_2MA            ( 0<< 5)
#define AW8838_BIT_I2STXCFG_DOHZ_MASK               (~( 1<< 4))
#define AW8838_BIT_I2STXCFG_DOHZ_HIZ                ( 1<< 4)
#define AW8838_BIT_I2STXCFG_DOHZ_GND                ( 0<< 4)
#define AW8838_BIT_I2STXCFG_DSEL_MASK               (~( 3<< 2))
#define AW8838_BIT_I2STXCFG_DSEL_DSP                ( 2<< 2)
#define AW8838_BIT_I2STXCFG_DSEL_GAIN               ( 1<< 2)
#define AW8838_BIT_I2STXCFG_DSEL_ZERO               ( 0<< 2)
#define AW8838_BIT_I2STXCFG_CHS_MASK                (~( 1<< 1))
#define AW8838_BIT_I2STXCFG_CHS_RIGHT               ( 1<< 1)
#define AW8838_BIT_I2STXCFG_CHS_LEFT                ( 0<< 1)
#define AW8838_BIT_I2STXCFG_TX_MASK                 (~( 1<< 0))
#define AW8838_BIT_I2STXCFG_TX_ENABLE               ( 1<< 0)
#define AW8838_BIT_I2STXCFG_TX_DISABLE              ( 0<< 0)

// PWMCTRL
#define AW8838_BIT_PWMCTRL_HMUTE_MASK               (~( 1<< 0))
#define AW8838_BIT_PWMCTRL_HMUTE_ENABLE             ( 1<< 0)
#define AW8838_BIT_PWMCTRL_HMUTE_DISABLE            ( 0<< 0)

// HAGCCFG8
#define AW8838_BIT_HAGCCFG8_VOL_MASK                (~(255<< 8))
#define AW8838_VOLUME_MAX                           (0)
#define AW8838_VOLUME_MIN                           (-255)
#define AW8838_VOL_REG_SHIFT                        (8)
#define AW8838_VOL_6DB_STEP                         (6 * 2)

//CPCTRL
#define AW8838_BIT_CPCTRL_CP_IPEAK_MASK             (~( 1<< 15))
#define AW8838_BIT_CPCTRL_CP_MD_MASK                (~( 3<< 8))
#define AW8838_BIT_CPCTRL_CP_MD_TRANS               ( 0<<8)

#endif
