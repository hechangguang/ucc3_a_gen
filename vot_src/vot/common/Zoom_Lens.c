/*
 * Zoom_Lens.c
 *
 *  Created on: 2024年5月11日
 *      Author: Administrator
 */


#include "../myheader.h"
#include <uapi/pwm_v100.h>
// #include "Zoom_Lens.h"
#include <menu/votmenu.h>
#include <uapi/isp_comm.h>
#include <uapi/isp_v100.h>
#include <fontlib_vot.h>
#include <menu/usermenufunction.h>
#include <menu/ispfunction.h>
#include <planck640c_vot.h>

// u8 MotorParameterBuff[MOTOR_LENGTH] = {0};
ts_s32 FocusMotorTab[MOTORPRESET_NUM][MOTORPOS_NUM+1] = {0};
ts_s32 ZoomMotorTab[MOTORPRESET_NUM][MOTORPOS_NUM+1] = {0};


MotorPresetPara MotorPresetParameters;
MotorMeasureParameters MeasureParameters;
MotorFocusParameters  FocusParameters;
MotorZoomParameters  ZoomParameters;
VoltageSample VoltageSamplePara;
motorFlag_t MotorFlag;
MotorStoragePara MotorStorageParameters;
MotorFlashPara MotorPresetParemeters[MOTORPRESET_NUM] = {
		{FocusMotorTab[0],ZoomMotorTab[0]},
		{FocusMotorTab[1],ZoomMotorTab[1]},
		{FocusMotorTab[2],ZoomMotorTab[2]},
		{FocusMotorTab[3],ZoomMotorTab[3]},
		{FocusMotorTab[4],ZoomMotorTab[4]},
		{FocusMotorTab[5],ZoomMotorTab[5]},
		{FocusMotorTab[6],ZoomMotorTab[6]},
		{FocusMotorTab[7],ZoomMotorTab[7]},
		{FocusMotorTab[8],ZoomMotorTab[8]},
		{FocusMotorTab[9],ZoomMotorTab[9]},
		{FocusMotorTab[10],ZoomMotorTab[10]},
		{FocusMotorTab[11],ZoomMotorTab[11]},
		{FocusMotorTab[12],ZoomMotorTab[12]},
		{FocusMotorTab[13],ZoomMotorTab[13]},
		{FocusMotorTab[14],ZoomMotorTab[14]},
		{FocusMotorTab[15],ZoomMotorTab[15]},
};
extern SysCtrlInfo_s_t global_sys_ctrl_info;
//u8 testflag = 1;
u8 FocusTable_AorM = 1; //开启自动切换跟焦标定表
ts_u16 Calculate_PeakVal = 0;
ts_f32 ZoomMotorSen = 0;
ts_u08 MCP3221ReadFlag = 0;
ts_u08 Focus_MCP3221_addr;    
ts_u08 Zoom_MCP3221_addr;  
static int s32_tp_pwm = -1;
/************MainFunction*************/
static ts_s32 MotorOperate(ts_void *pv_param)
{
	TS_UNUSED(pv_param);
	while(1) {
		ts_cpl_msleep(10);
		MotorFlagRelated();
		FocusRelated();
		// ZoomRelated(); //暂时不需要
//		AutoFocusVoltageSample();
		PresetRelated();
		}

	return 0;
}
/***********************Motor功能线程任务***************************/
ts_void MoterFun_Init(ts_void)
{
if(get_focus_type()>0){
    ts_cpl_task_s_t st_Motor_task;

    MotorInit();
	MotorModeSelect();
	Read_MotorParameterFromFlash();

    // ts_cpl_kthread_create((ts_cpl_task_s_t *)&st_Motor_task, (ts_cpl_kthread_t)MotorOperate, ts_null, "Motor", 5, 4*2048);
	ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&st_Motor_task, (ts_cpl_kthread_t)MotorOperate, ts_null, "Motor", 5, 4*2048,ts_null);  //在用
}
}


void FocusRelated(void){

	if(Focus_Calibration_Flag){
    	MotorAutoMeasure();
    	MotorAutoFocus();
    }

}

void ZoomRelated(void){
if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
		ManualZoom();
}
}

void PresetRelated(void){
#ifdef FunctuinMotorPreset
	PresetPosSearch(&MotorPresetParameters.PresetSwitch);
if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
	MotorSensor_Read();
	PresetPositionCalibration();
	FollowFocus(MotorPresetParameters.FollowSwitch,\
			&MotorPresetParemeters[MotorPresetParameters.PresetNum],(ts_s32)GetVol(Zoom_MCP3221_addr));
}
#endif
}

void MotorFlagRelated(void){

	MotorFlag.Flag = MeasureParameters.MeasureFocusFlag ? MotorFlag.Flag | (0x01<<0) : MotorFlag.Flag & (~(0x01<<0));
	MotorFlag.Flag = FocusParameters.FocusFlag ? MotorFlag.Flag | (0x01<<2) : MotorFlag.Flag & (~(0x01<<2));


if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
	MotorFlag.Flag = MeasureParameters.MeasureZoomFlag ? MotorFlag.Flag | (0x01<<1) : MotorFlag.Flag & (~(0x01<<1));
	MotorFlag.Flag = ZoomParameters.ZoomFlag ? MotorFlag.Flag | (0x01<<3) : MotorFlag.Flag & (~(0x01<<3));
}

#ifdef FunctuinMotorPreset
	MotorFlag.Flag = MotorPresetParameters.CalibrationFlag ? MotorFlag.Flag | (0x01<<4) : MotorFlag.Flag & (~(0x01<<4));
	MotorFlag.Flag = MotorPresetParameters.ZoomOperateFlag ? MotorFlag.Flag | (0x01<<5) : MotorFlag.Flag & (~(0x01<<5));
#endif
}
/*************************************/
#if 0
static ts_s32 i2C0_hd = -1;
ts_u16 Read2ByteFromMCP3221_IIC(u8 MCP3221_addr)
{
	ts_u16 VoltageVal;
	ts_s32 s32_ret;

	if(i2C0_hd == -1){
		i2C0_hd = ts_lib_i2c_open("/dev/i2c0");
		// vision_log_debug(" Open IIC file Success! \n");		
	}
	if(i2C0_hd >= 0){
		s32_ret = ts_lib_i2c_ra08d16_readw(i2C0_hd,MCP3221_addr,0x00,(ts_u16 *)&VoltageVal);
	 	if (s32_ret != TS_OK){
		vision_log_debug("i2c0 read error\n");
        return 0xFFFF;}
        vision_log_debug("i2c0 addr[%#x] : %#x\n", MCP3221_addr, VoltageVal);
		return VoltageVal;
	}else{
		vision_log_debug(" Open i2C0 file Fail! \n");		
		return 0xFFFF;
	}
	
}
#endif
/*************MotorOperateFunction*************************/
ts_u32  GetVol(u8 MCP3221_addr)
{
	uint32_t temp = 0;
	uint16_t val;
	float vol=0;

	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER || SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER)
	{
	// val = Read2ByteFromMCP3221_IIC(MCP3221_addr);
	if(MCP3221ReadFlag == 0){
		val = Device_I2c0_Data_Get(MCP3221_Addr,MCP3221_addr);
	}
	else{
		val = 0xFFFF;
	}
	// vision_log_debug("VoltageVal == %#x \n",val);
	vol =  val*3.3/4096;
	temp = (uint32_t )(vol*1000);
	// vision_log_debug("VoltageAD == %#x \n",temp);
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER || SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER)
	{
		if(MCP3221_addr == Zoom_MCP3221_addr){
		ts_s32 RetZoom = ts_lib_pwm_get_qdr_cnt(s32_tp_pwm, 1, (ts_u32 *)&temp);		
		if(RetZoom != TS_OK){
			vision_log_debug(" Get Zoom_Encoder Count Error!!! \n");
			return 0;
		}
		}else if(MCP3221_addr == Focus_MCP3221_addr){
			ts_s32 RetFocus = ts_lib_pwm_get_qdr_cnt(s32_tp_pwm, 0, (ts_u32 *)&temp);
			if(RetFocus != TS_OK){
				vision_log_debug(" Get Focus_Encoder Count Error!!! \n");
				return 0;
			}
		}
	// vision_log_notice("Ret = %d,Encoder Count = [%d] \n",Ret,temp);
		temp = temp/16;
	}
	return temp;

}


ts_s32 Get_Encoder_Vol(u8 MCP3221_addr)  //编码器计数读取
{
	ts_s32 delta_count = 0;
	if(MCP3221_addr == Zoom_MCP3221_addr){
		ts_s32 RetZoom = ts_lib_pwm_get_qdr_cnt(s32_tp_pwm, 1, (ts_u32 *)&delta_count);		
		if(RetZoom != TS_OK){
			vision_log_debug(" Get Zoom_Encoder Count Error!!! \n");
			return 0;
		}
		// ts_lib_pwm_set_qdr_center_val(s32_tp_pwm, 0, 4000); //编码器计数清零
		// return 0;
	}else if(MCP3221_addr == Focus_MCP3221_addr){
		ts_s32 Ret = ts_lib_pwm_get_qdr_cnt(s32_tp_pwm, 0, (ts_u32 *)&delta_count);
		if(Ret != TS_OK){
			vision_log_debug(" Get Focus_Encoder Count Error!!! \n");
		}
	}
	// vision_log_debug("Encoder Count =[%d] current_count =[%d] delta_count =[%d] \n",s_total_position,temp,delta_count);
	// vision_log_notice("Ret = %d,Encoder Count = [%d] \n",Ret,delta_count);
	delta_count = delta_count/16;
	return delta_count;
}

void Encoder_Clear(ts_u32 setval,ts_u08 Type)
{

	setval = setval*16;
	if(Type == Focus_MCP3221_addr){
		ts_lib_pwm_set_qdr_center_val(s32_tp_pwm, 0, setval); //编码器计数清零
	}
	else if(Type == Zoom_MCP3221_addr){
		ts_lib_pwm_set_qdr_center_val(s32_tp_pwm, 1, setval); //编码器计数清零
	}
	// ts_lib_pwm_set_qdr_center_val(s32_tp_pwm, 0, setval); //编码器计数清零

}

void MotorEncoder_Init(void)
{
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER || SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER)
	{
		pwm_bind_encoder_irq_set(s32_tp_pwm, 0);
		ts_lib_pwm_set_qdr_center_val(s32_tp_pwm, 0, 0); //编码器计数清零
		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER)
		{
			pwm_bind_encoder_irq_set(s32_tp_pwm, 1);
			ts_lib_pwm_set_qdr_center_val(s32_tp_pwm, 1, 0); //编码器计数清零
		}
	}
}

void FocusMotorOperate(u8 state,u8 speed,u8 direction,u8 stepnum)
{

	// MotorState(state);
	// MotorStepLenth(speed);
	// MotorPolarity(direction);
	// MotorStepNum(stepnum);
	// delay_1ms(5);

	// ts_s32 retVal;
	ts_u32 u32_duty0 = 0,u32_duty1 = 0;
	ts_u32 u32_channel0,u32_channel1;
	if(s32_tp_pwm == -1) {
		s32_tp_pwm = ts_lib_pwm_open("/dev/pwm");
		vision_log_debug(" Open PWM file Success! pwm_handle = %d\n",s32_tp_pwm);
		MotorEncoder_Init();
	}
	
	if (s32_tp_pwm >= 0) {
	
    u32_channel0 = 0; // channel PWM0
	u32_channel1 = 1; // channel PWM1

	// vision_log_debug(" Direction == %d \n",direction);

	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER ||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
	if(direction == 0){
    u32_duty0 = 0;
	u32_duty1 = speed;
	}else if(direction == 1){
    u32_duty1 = 0;
	u32_duty0 = speed;
	}
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER ||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
	if(direction == 0){
    u32_duty0 = speed;
	u32_duty1 = 0;
	}else if(direction == 1){
    u32_duty1 = speed;
	u32_duty0 = 0;
	}
	}

	
	if (state == ForceStopState) {
        ts_lib_pwm_stop(s32_tp_pwm, u32_channel0);
		// vision_log_debug(" PWM_Stop0 == %d \n",retVal);	
		ts_lib_pwm_stop(s32_tp_pwm, u32_channel1);
		// vision_log_debug(" PWM_Stop1 == %d \n",retVal);	

		// ts_lib_pwm_close(s32_tp_pwm);
		// s32_tp_pwm = -1;
		// vision_log_debug(" Close PWM Success! \n");
    } else {
        ts_lib_pwm_set_mode(s32_tp_pwm, u32_channel0, TS_PWM_MODE_CONTINUOUS, TS_PWM_PFOUT_HIGH);
		// vision_log_debug(" PWM_Mode0 == %d \n",retVal);
		ts_lib_pwm_set_mode(s32_tp_pwm, u32_channel1, TS_PWM_MODE_CONTINUOUS, TS_PWM_PFOUT_HIGH);
		// vision_log_debug(" PWM_Mode1 == %d \n",retVal);
        ts_lib_pwm_set_freq_duty(s32_tp_pwm, u32_channel0, 10*1000, u32_duty0, TS_PWM_DUTY_ROUNDING);
		// vision_log_debug(" PWM_Set_FreqDuty0 == %d \n",retVal);
		ts_lib_pwm_set_freq_duty(s32_tp_pwm, u32_channel1, 10*1000, u32_duty1, TS_PWM_DUTY_ROUNDING);
		// vision_log_debug(" PWM_Set_FreqDuty1 == %d \n",retVal);
        ts_lib_pwm_set_irqen(s32_tp_pwm, u32_channel0, TS_PWM_IRQ_DISABLE);
		ts_lib_pwm_set_irqen(s32_tp_pwm, u32_channel1, TS_PWM_IRQ_DISABLE);
		// if(direction == 0){
        ts_lib_pwm_start(s32_tp_pwm, u32_channel0, 0);
        ts_lib_pwm_start(s32_tp_pwm, u32_channel1, 0);
		//}
		// else if(direction == 1){      
		// ts_lib_pwm_start(s32_tp_pwm, u32_channel1, 0);
        // ts_lib_pwm_start(s32_tp_pwm, u32_channel0, 0);}
		// ts_cpl_msleep(10);
    }
	
	}

}

void ZoomMotorOperate(u8 state,u8 speed,u8 direction,u8 stepnum)
{

	// Motor2State(state);
	// Motor2StepLenth(speed);
	// Motor2Polarity(direction);
	// Motor2StepNum(stepnum);
	// delay_1ms(5);

	// ts_s32 retVal;
	ts_u32 u32_duty0 = 0,u32_duty1 = 0;
	ts_u32 u32_channel2,u32_channel3;
	if(s32_tp_pwm == -1) {
		s32_tp_pwm = ts_lib_pwm_open("/dev/pwm");
		vision_log_debug(" Open PWM file Success! \n");
	}
	
	if (s32_tp_pwm >= 0) {
    u32_channel2 = 2; // channel PWM2
	u32_channel3 = 3; // channel PWM3

	// vision_log_debug(" Direction == %d \n",direction);
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
	if(direction == 0){
    u32_duty0 = 0;
	u32_duty1 = speed;
	}else if(direction == 1){
    u32_duty1 = 0;
	u32_duty0 = speed;
	}
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
	if(direction == 0){
    u32_duty0 = speed;
	u32_duty1 = 0;
	}else if(direction == 1){
    u32_duty1 = speed;
	u32_duty0 = 0;
	}
	}
	
	if (state == ForceStopState) {
        ts_lib_pwm_stop(s32_tp_pwm, u32_channel2);
		// vision_log_debug(" PWM_Stop2 == %d \n",retVal);	
		ts_lib_pwm_stop(s32_tp_pwm, u32_channel3);
		// vision_log_debug(" PWM_Stop3 == %d \n",retVal);	

		// ts_lib_pwm_close(s32_tp_pwm);
		// s32_tp_pwm = -1;
		// vision_log_debug(" Close PWM Success! \n");
    } else {
        ts_lib_pwm_set_mode(s32_tp_pwm, u32_channel2, TS_PWM_MODE_CONTINUOUS, TS_PWM_PFOUT_HIGH);
		// vision_log_debug(" PWM_Mode2 == %d \n",retVal);
		ts_lib_pwm_set_mode(s32_tp_pwm, u32_channel3, TS_PWM_MODE_CONTINUOUS, TS_PWM_PFOUT_HIGH);
		// vision_log_debug(" PWM_Mode3 == %d \n",retVal);
        ts_lib_pwm_set_freq_duty(s32_tp_pwm, u32_channel2, 10*1000, u32_duty0, TS_PWM_DUTY_ROUNDING); //10KHz
		// vision_log_debug(" PWM_Set_FreqDuty2 == %d \n",retVal);
		ts_lib_pwm_set_freq_duty(s32_tp_pwm, u32_channel3, 10*1000, u32_duty1, TS_PWM_DUTY_ROUNDING); //10KHz
		// vision_log_debug(" PWM_Set_FreqDuty3 == %d \n",retVal);
        ts_lib_pwm_set_irqen(s32_tp_pwm, u32_channel2, TS_PWM_IRQ_DISABLE);
		ts_lib_pwm_set_irqen(s32_tp_pwm, u32_channel3, TS_PWM_IRQ_DISABLE);
		// if(direction == 0){
        ts_lib_pwm_start(s32_tp_pwm, u32_channel2, 0);
        ts_lib_pwm_start(s32_tp_pwm, u32_channel3, 0);
	}
	}

}

/**************************************************/
/******************编码器相关配置*******************/
/**************************************************/
 /* QDR中断回调函数 */
ts_void Encoder_irq_cb(ts_u32 u32_channel, ts_void * pv_priv)
{
	vision_log_debug("Encoder_irq_cb, channel = %d\n", u32_channel);
}
ts_void pwm_bind_encoder_irq_set(ts_s32 s32_encoder_pwm, ts_u32 u32_qdr_ch)
{

    ts_pwm_qdr_attr_s_t st_qdr_attr = {0}; // 初始化结构体为0
    qdr_ctrl_un_t *pun_ctrl;
	// ts_u32 temp = 0;

	//  // 1. 先获取当前属性（如果有默认值需要保留）
    // ts_s32 ret2 = ts_lib_pwm_get_qdr_attr(s32_encoder_pwm, (ts_pwm_qdr_attr_s_t *)&st_qdr_attr);
    // if (ret2 != TS_OK) {
    //     vision_log_debug("Failed to get QDR attributes, ret = %d\n", ret2);
    //     return;
    // }
	// vision_log_notice("get_attr ret111 = %d, work_en = %d, work_mode = %d, u32_ch = %d \n", ret2, st_qdr_attr.un_ctrl.work_en,st_qdr_attr.un_ctrl.work_mode,st_qdr_attr.u32_ch);

	st_qdr_attr.u32_ch = u32_qdr_ch;
    pun_ctrl = (qdr_ctrl_un_t *)&st_qdr_attr.un_ctrl;

    /* ctrl */
    pun_ctrl->work_en = 1;
    pun_ctrl->interrupt_en = 0;
    pun_ctrl->work_mode = 0;      //0x0：单纯计数模式；0x1：计数比较模式
    pun_ctrl->auto_clr_en = 0;	//0：不自动清零；1：自动清零 计数比较模式下生效
	/*双相输入*/
    pun_ctrl->deglitch_en = 1;
    pun_ctrl->pa_cnt_en = 1;
    pun_ctrl->pb_cnt_en = 1;

    /* param *//*4倍频*/
    pun_ctrl->pa_oppo_level = 0;
    pun_ctrl->pa_posedge_en = 1;
    pun_ctrl->pa_negedge_en = 1;
    pun_ctrl->pb_oppo_level = 0;
    pun_ctrl->pb_posedge_en = 1;
    pun_ctrl->pb_negedge_en = 1;

    st_qdr_attr.u32_compare_cnt = 120;

	// 根据中断使能设置回调函数
    if (pun_ctrl->interrupt_en == 1) {
        st_qdr_attr.fp_irqcb = Encoder_irq_cb;  // 中断回调
        st_qdr_attr.pv_priv = ts_null;         // 私有数据
    } else {
        st_qdr_attr.fp_irqcb = ts_null; 
        st_qdr_attr.pv_priv = ts_null;
	}

    ts_s32 ret = ts_lib_pwm_set_qdr_attr(s32_encoder_pwm, (ts_pwm_qdr_attr_s_t *)&st_qdr_attr); //设置磁编模块的属性
	vision_log_notice("ts_set_Encoder_attr ret = %d,u32_ch = %d\n", ret, st_qdr_attr.u32_ch);
    // ts_lib_pwm_set_qdr_compare_cnt(s32_encoder_pwm, u32_qdr_ch, 150); //设置磁编模块的比较值

    ret = ts_lib_pwm_get_qdr_attr(s32_encoder_pwm, (ts_pwm_qdr_attr_s_t *)&st_qdr_attr); //获取磁编模块的属性
	vision_log_notice("ts_get_Encoder_attr ret = %d, work_mode = %d, work_en = %d, interrupt_en = %d \n", ret, st_qdr_attr.un_ctrl.work_mode,st_qdr_attr.un_ctrl.work_en,st_qdr_attr.un_ctrl.interrupt_en);
	// ts_u32 Ret = ts_lib_pwm_get_qdr_cnt(s32_encoder_pwm, u32_qdr_ch, (ts_u32 *)&temp);
	// if(Ret != TS_OK){
	// 	vision_log_debug(" Get Encoder Count Error!!! \n");
	// }

  vision_log_debug("---Encoder_configuration success!!\n");

}

/**************************************************/

void BubbleSort(u32 a[],u8 len)
{

	uint32_t i,j,temp;
	for(i=0;i<len-1;i++)
	{
		for(j=0;j<len-1-i;j++)//
		{
			if(a[j]>a[j+1])//
			{
				temp = a[j];
				a[j] = a[j+1];
				a[j+1] = temp;
			}
		}
	}

}

void FocusAreaSet(u16 Lx,u16 Ly,u16 Rx,u16 Ry){

	FocusAreaSetLx(Lx);
	FocusAreaSetLy(Ly);
	FocusAreaSetRx(Rx);
	FocusAreaSetRy(Ry);

}

/*******************************************************/
/*Focus菜单初始化设置*/
void FocusParInit(ts_u08 state)
{
	if(state)
	{

		SystemParaMenu2.Zoom_Lens = 1;
		SystemParaMenu2.Right_Val = 100;
		SystemParaMenu2.Left_Val = 100;
		SystemParaMenu2.FocusSite = 0;
		//SystemParaMenu2.FCal_SW = 1;
		SystemParaMenu2.OperatorSpeed = 25;  //微调速度
		if(SystemParaMenu2.m_force_detector_type==FORCE_DETECTOR_TYPE_VOT_384X288){
			SystemParaMenu2.Focus_Lx = 10;
			SystemParaMenu2.Focus_Ly = 5;
			SystemParaMenu2.Focus_Rx = 372;
			SystemParaMenu2.Focus_Ry = 279;
		}else{
		SystemParaMenu2.Focus_Lx = 20;
		SystemParaMenu2.Focus_Ly = 10;
		SystemParaMenu2.Focus_Rx = 630;
		SystemParaMenu2.Focus_Ry = 490;
		}

		// MeasureParameters.focusForward = MotorForward;
		// MeasureParameters.focusReversal = MotorReversal;
		// MeasureParameters.zoomForward = MotorForward;
		// MeasureParameters.zoomReversal = MotorReversal;

		Focus_Calibration_Flag = 1;

		SystemParaMenu2.Focus_DNumSet = 13;
		SystemParaMenu2.Focus_UNumSet = 13;
		SystemParaMenu2.MotorSenFlag = 0; //单个标定表
		SystemParaMenu2.FocusPeakMode = 0; //不设置假峰值阈值
		SystemParaMenu2.Focus_PeakVal = 100;//假峰值阈值
		vision_log_notice("FocusParInit Finish!!!Lx[%d]Ly[%d]Rx[%d]Ry[%d]\n",SystemParaMenu2.Focus_Lx,SystemParaMenu2.Focus_Ly,SystemParaMenu2.Focus_Rx,SystemParaMenu2.Focus_Ry);


	}
}

/***********MotorParameterrInitFunction**************/
 void FocusParaInit(void)
{

	FocusParameters.FocusFlag = 0;
	FocusParameters.ValidSteps = 0;
	FocusParameters.NowSite = 0;
	FocusParameters.MaxValue = 0;
	FocusParameters.FocusSite = 0;
	FocusParameters.BackSteps = 0;

	FocusParameters.UpNum = 0;
	FocusParameters.DownNum = 0;
	FocusParameters.LastSite = 0;

	FocusParameters.PresetPos = 0;
	FocusParameters.PresetRightFlag = 0;
	FocusParameters.PresetLeftFlag = 0;

	memset(FocusParameters.ImageValue, 0, sizeof(FocusParameters.ImageValue));

}

 void MeasureParaInit(void)
 {

	 MeasureParameters.MeasureFocusFlag = 1;
	 MeasureParameters.LimitStepMax = 0;
	 MeasureParameters.LimitStepMin = 0;
	 MeasureParameters.MeasureFocusSum = 0;
	 MeasureParameters.MeasureFocusNum = 0;
	 MeasureParameters.focusForward = MotorForward;
	 MeasureParameters.focusReversal = MotorReversal;



if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER ||SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
	 MeasureParameters.MeasureZoomFlag = 1;
	 MeasureParameters.LimitZoomMax = 0;
	 MeasureParameters.LimitZoomMin = 0;
	 MeasureParameters.MeasureZoomSum = 0;
	 MeasureParameters.MeasureZoomNum = 0;
	 MeasureParameters.zoomForward = MotorForward;
	 MeasureParameters.zoomReversal = MotorReversal;
}
 }

 void ZoomParaInit(void)
 {

	 ZoomParameters.ZoomFlag = 0;
	 ZoomParameters.ZoomSite = 1;
	 ZoomParameters.PresetPos = 0;
	 ZoomParameters.PresetRightFlag = 0;
	 ZoomParameters.PresetLeftFlag = 0;

 }

 void PresetPositionParaInit(void){
#ifdef FunctuinMotorPreset
 	MotorPresetParameters.CalibrationFlag = 0;

 	// MotorPresetParameters.PresetNum = 0; //标定表标号
 	MotorPresetParameters.CalibrationPos = 0;
 	MotorPresetParameters.CalibrationNum = 0;

// 	MotorPresetParameters.SearchFlag = 0;
 	MotorPresetParameters.PresetSwitch = OFF_State;
 	MotorPresetParameters.FollowFlag = 0;

 	MotorPresetParameters.FollowSwitch = ON_State;

 	MotorPresetParameters.ZoomPresetSpeed = 21;
 	MotorPresetParameters.FocusPresetSpeed = 15;

 	MotorPresetParameters.ZoomOperateFlag = 0;
#endif
 }

 void ZhongLin_Motor(void){
#if SUPPORT_ZhongLin_UART_PROTOCOL
	 FocusParameters.FocusSpeed = 3;
	 ZoomParameters.ZoomSpeed = 3;
#endif
 }
ts_void AFS_Focus_init(ts_void)
{
    ts_isp_afs_attr_s_t afs_attr;
    ts_isp_pipe_attr_s_t pipe_attr;
    ts_s32 s32_ret;

    s32_ret = ts_tdk_isp_get_afs_attr(TS_ISP_MDEV0, (ts_isp_afs_attr_s_t *)&afs_attr); 

    afs_attr.u08_af_en = 1;
    s32_ret = ts_tdk_isp_set_afs_attr(TS_ISP_MDEV0, (ts_isp_afs_attr_s_t *)&afs_attr); 

    pipe_attr.en_pipe_module_sel = TS_ISP_PIPE_AFS_SEL;
    pipe_attr.u08_mod_pos = 1;
    ts_tdk_isp_set_pipe_pos(TS_ISP_MDEV0, &pipe_attr);

    AFS_Focus_Adjust();
    vision_log_warn("afs init done, ret = %d!\n", s32_ret);
}

ts_void AFS_Focus_Adjust(ts_void)
{
    ts_s32 s32_thr1 = 0;  
    ts_s32 s32_delta = 5;
    ts_s32 s32_coef1 = 0;  ///高频太少认为是噪声， 进行抑制
    ts_s32 s32_coef2 = 64;

    ts_isp_afs_attr_s_t st_af_attr;
    ts_tdk_isp_get_afs_attr(TS_ISP_MDEV0, &st_af_attr);
    st_af_attr.st_hf_adj_h.u08_hf_coef1 = s32_coef1;
    st_af_attr.st_hf_adj_h.u08_hf_coef2 = s32_coef2;
    st_af_attr.st_hf_adj_h.u32_hf_thr1 = s32_thr1;
    st_af_attr.st_hf_adj_h.u08_hf_delta1 = s32_delta;
    ts_tdk_isp_set_afs_attr(TS_ISP_MDEV0, &st_af_attr);    
    vision_log_debug("afs flt_param set to %d, %d, %d, %d \n", s32_thr1, s32_delta, s32_coef1, s32_coef2);  

}


void MotorInit(void)
{
	ts_u08 MotorInitType = get_focus_type();
	MotorFlag.Flag = 0;

    //调焦相关初始化
    AFS_Focus_init();
	//MCP3221_IIC_Init();
	FocusParaInit();
	MeasureParaInit();
	FocusParameters.FocusSpeed = FullSpeed;
	Motor_AddressType_Select();


if(MotorInitType == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER||MotorInitType == FOCUS_TYPE_ZOOM_LENS_ENCODER){
	ZoomParaInit();
	MotorSen_Init(); //多个跟焦标定表温度
}

#if SUPPORT_ZhongLin_UART_PROTOCOL
	ZhongLin_Motor();
#endif

#ifdef FunctuinMotorPreset
	VoltageSamplePareInit();
	PresetPositionParaInit();
#endif
	vision_log_notice("MotorInit Finish!!!\n");
}

void MotorModeSelect(void){

	// if(!SystemParaMenu2.Zoom_Lens){
	// 	if(SystemParaMenu2.FocusSite && SystemParaMenu2.FocusSite!=0xFFFF){
	// 		Focus_Calibration_Flag = 1;
	// 	}else{
	// 		Focus_Calibration_Flag = 0;
	// 		//MeasureParameters.MeasureFlag = 0;
	// 	}
	// }else{
		Focus_Calibration_Flag = 1;
	// }
}

void Motor_AddressType_Select(void)
{
	if(SystemParaMenu2.ADDR_Sel > 2){
		SystemParaMenu2.ADDR_Sel = 0;
	}
	//5T：0x9b, 4T：0x99,  7T：0x9F
	if(SystemParaMenu2.ADDR_Sel == 0){  //MY
		Focus_MCP3221_addr = FocusMCP_MYAddresss;
		Zoom_MCP3221_addr = ZoomMCP_MYAddresss;
	}else if(SystemParaMenu2.ADDR_Sel == 1){  //SS1
		Focus_MCP3221_addr = FocusMCP_SS1Addresss;
		Zoom_MCP3221_addr = ZoomMCP_SS1Addresss;
	}
	else if(SystemParaMenu2.ADDR_Sel == 2){  //CML
		Focus_MCP3221_addr = FocusMCP_MYAddresss;
		Zoom_MCP3221_addr = ZoomMCP_MYAddresss;
	}
	vision_log_notice("ADDR_Sel = %d,Focus_MCP3221_addr = %#x,Zoom_MCP3221_addr = %#x \n", SystemParaMenu2.ADDR_Sel, (Focus_MCP3221_addr<<1)|0x01, (Zoom_MCP3221_addr<<1)|0x01);
}

/******************************Evaluation Function*********************************/
// 单次计算
ts_void afs_calc_meanTT(ts_void)
{
    ts_cpl_timeval_s_t st_time0, st_time1;
    ts_isp_af_statis_s_t st_af_statis;
    ts_s32 s32_ret; 

    s32_ret = ts_tdk_isp_get_af_statis(TS_ISP_MDEV0, (ts_isp_af_statis_s_t *)&st_af_statis, 2000);
    if (s32_ret != TS_OK) {
        printf("get af res err! \n");
    } else {
         ts_cpl_gettimeofday(&st_time0);   
        printf("get af res ok !\n");
        ts_s64 s64_sum_h1 = 0;
        ts_s64 s64_sum_h2 = 0;

        ts_s64 s64_sum_v1 = 0;
        ts_s64 s64_sum_v2 = 0;

        ts_s64 s64_abs_h = 0;
        ts_s64 s64_abs_v = 0;

        ts_s64 s64_sum_aver = 0;
        for (ts_s32 i = 0; i < st_af_statis.u16_blk_num_h * st_af_statis.u16_blk_num_w; i++) {
            s64_sum_h1 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h1);
            s64_sum_h2 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h2);

            s64_sum_v1 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v1);
            s64_sum_v2 += (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v2);

            s64_abs_v += abs((ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v1) - (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_v2));
            s64_abs_h += abs((ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h1) - (ts_s64)(st_af_statis.pst_af_statis_info[i].s32_h2));
            s64_sum_aver += (ts_s64)st_af_statis.pst_af_statis_info[i].u16_aver;
        }
        ts_cpl_gettimeofday(&st_time1);
        vision_log_debug("afs  %d us : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec));

        printf("af info num = [%d, %d], addr = 0x%x, af mean = h:[%d, %d, %d], v:[%d, %d: %d], aver:[%d]\n", st_af_statis.u16_blk_num_h, st_af_statis.u16_blk_num_w,
                                                (ts_u32)st_af_statis.pst_af_statis_info, 
                                                (ts_s32)(s64_sum_h1 / 1024), 
                                                (ts_s32)(s64_sum_h2 / 1024), 
                                                (ts_s32)(s64_abs_h / 1024), 
                                                (ts_s32)(s64_sum_v1 / 1024), 
                                                (ts_s32)(s64_sum_v2 / 1024), 
                                                (ts_s32)(s64_abs_v / 1024), 
                                                (ts_s32)(s64_sum_aver /1024));
    }

    ts_tdk_isp_free_af_statis(TS_ISP_MDEV0, &st_af_statis);
}


ts_s32 AFS_EvaluationFuc_Cal(ts_void)
{
	// ts_cpl_timeval_s_t st_time0, st_time1;
	ts_isp_af_statis_s_t st_af_statis;
    ts_s32 s32_ret; 
	// ts_s32 Image_V[8] = {0};
	ts_s32 AFS_CalValue = 0;	//AFS 评价函数值
//  ts_cpl_gettimeofday(&st_time0); 
	// for(ts_s32 m=0;m<3;m++){
    s32_ret = ts_tdk_isp_get_af_statis(TS_ISP_MDEV0, (ts_isp_af_statis_s_t *)&st_af_statis, 2000);
    if (s32_ret != TS_OK) {
        vision_log_debug("get afs res err! \n");
    } else {
		
        // vision_log_debug("get af res ok !\n");
        ts_u64 s64_sum_h1 = 0;
        ts_s32 s64_sum_h2 = 0;

        ts_s32 s64_sum_v1 = 0;
        ts_s32 s64_sum_v2 = 0;

        ts_s32 s64_abs_h = 0;
        ts_s32 s64_abs_v = 0;

        ts_s32 s64_sum_aver = 0;

	#if 0	//完整图像统计

        for (ts_s32 i = 0; i < st_af_statis.u16_blk_num_h * st_af_statis.u16_blk_num_w; i++) {
			
            s64_sum_h1 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h1);
			// s64_sum_h1 += (ts_u64)(st_af_statis.pst_af_statis_info[i].s32_h1) * (ts_u64)(st_af_statis.pst_af_statis_info[i].s32_h1);
            s64_sum_h2 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h2);

            s64_sum_v1 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v1);
            s64_sum_v2 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v2);

            s64_abs_v += abs((ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v1) - (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v2));
            s64_abs_h += abs((ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h1) - (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h2));
            s64_sum_aver += (ts_s32)st_af_statis.pst_af_statis_info[i].u16_aver;
        }
	#else	//自定义区域统计 测试
		ts_s32 F_AreaLx = SystemParaMenu2.Focus_Lx/20;
		ts_s32 F_AreaLy = SystemParaMenu2.Focus_Ly/16;
		ts_s32 F_AreaRx = SystemParaMenu2.Focus_Rx/20;
		ts_s32 F_AreaRy = SystemParaMenu2.Focus_Ry/16;	
		for (ts_s32 k = F_AreaLx; k <= F_AreaRx; k++)
		 {
			for(ts_s32 j = F_AreaLy; j <= F_AreaRy; j++){
				ts_s32 i = j*st_af_statis.u16_blk_num_h + k;
				s64_sum_h1 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h1);
				// s64_sum_h1 += (ts_u64)(st_af_statis.pst_af_statis_info[i].s32_h1) * (ts_u64)(st_af_statis.pst_af_statis_info[i].s32_h1);
				s64_sum_h2 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h2);

				s64_sum_v1 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v1);
				s64_sum_v2 += (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v2);

				s64_abs_v += abs((ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v1) - (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_v2));
				s64_abs_h += abs((ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h1) - (ts_s32)(st_af_statis.pst_af_statis_info[i].s32_h2));
				s64_sum_aver += (ts_s32)st_af_statis.pst_af_statis_info[i].u16_aver;
			}
		}
	#endif
		// vision_log_debug("af info num = [%d, %d], addr = 0x%x, af mean = h:[%d, %d, %d], v:[%d, %d: %d], aver:[%d]\n", st_af_statis.u16_blk_num_h, st_af_statis.u16_blk_num_w,
        //                                         (ts_u32)st_af_statis.pst_af_statis_info, 
        //                                         (ts_s32)(s64_sum_h1 / 1024), 
        //                                         (ts_s32)(s64_sum_h2 / 1024), 
        //                                         (ts_s32)(s64_abs_h / 1024), 
        //                                         (ts_s32)(s64_sum_v1 / 1024), 
        //                                         (ts_s32)(s64_sum_v2 / 1024), 
        //                                         (ts_s32)(s64_abs_v / 1024), 
        //                                         (ts_s32)(s64_sum_aver /1024));
// vision_log_debug("af info num = [%d, %d], h:[%d, %d], V:[%d, %d]\n",st_af_statis.u16_blk_num_h, st_af_statis.u16_blk_num_w,(ts_s32)(s64_sum_h1 / 1024), (ts_s32)(s64_sum_h2 / 1024),
													// (ts_s32)(s64_sum_v1 / 1024), (ts_s32)(s64_sum_v2 / 1024));
		AFS_CalValue = s64_sum_h1/1024;
		// AFS_CalValue = s64_sum_h1/(st_af_statis.u16_blk_num_h * st_af_statis.u16_blk_num_w*1024);
				
		}
    	ts_tdk_isp_free_af_statis(TS_ISP_MDEV0, &st_af_statis);	
	// }

	// ts_cpl_gettimeofday(&st_time1);
	// vision_log_debug("afs  %d ms : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec) / 1000);
	// BubbleSort(Image_V,8);
	// AFS_CalValue = Image_V[4];
	// AFS_CalValue = (Image_V[0]+Image_V[1]+Image_V[2])/3;

	// ts_cpl_gettimeofday(&st_time1);
	// // vision_log_debug("afs8  %d ms : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec) / 1000);
	// vision_log_debug("afs  %d us : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec));
	return AFS_CalValue;
}

/************************************************************/

/**************MotorFocusFunction************************************/
void MotorAutoMeasure(void){
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_ENCODER || SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
		FocusRangeMeasure_encoder();
		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){
		ZoomRangeMeasure_encoder();
		}
	}else if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_FOCUS_LENS_POTENTIOMETER || SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
	FocusRangeMeasure();
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_POTENTIOMETER){
	ZoomRangeMeasure();
	}
  }
}
/*************************编码器模式 *****************/
void FocusRangeMeasure_encoder(void)
{
	ts_s32 NowSite;
	ts_s32 NowRange=0;
	// ts_s32 temp;
	static ts_s32 FocusLastRange_en = 0;
	static ts_u08 Focus_count = 0;

	 if(FocusParameters.FocusFlag == 0)
	 {

	 switch(MeasureParameters.MeasureFocusFlag)
	 {
     	 case 1:
     	 {
			// FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2); //1
			FocusMotorOperate(NormalState,Speedset1,MeasureParameters.focusReversal,2);
     		NowSite = Get_Encoder_Vol(Focus_MCP3221_addr);
     		NowRange = NowSite;
     		vision_log_debug("[1]FFF_Measure Minval= [%d],lastVal= [%d] \n",NowRange,FocusLastRange_en);

			if(NowRange <= FocusLastRange_en)
			{
				if(Focus_count)	{Focus_count = 0;}
				if(MeasureParameters.MeasureFocusNum)
				{
					if(abs(NowRange-FocusLastRange_en) > 0x20)
					{
						MeasureParameters.MeasureFocusNum = 0;
						MeasureParameters.MeasureFocusSum = 0;
					}
				}

				if(abs(NowRange-FocusLastRange_en) <= 0x20)
				{
					MeasureParameters.MeasureFocusNum++;
					MeasureParameters.MeasureFocusSum += NowSite;
				}
				if(MeasureParameters.MeasureFocusNum == 5)
				{
					FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);

					// MeasureParameters.LimitStepMax = (MeasureParameters.MeasureFocusSum/MeasureParameters.MeasureFocusNum);
					MeasureParameters.LimitStepMin = 625;//10000;
					Encoder_Clear(MeasureParameters.LimitStepMin,Focus_MCP3221_addr);
					vision_log_debug("[1]FFF_LimitStepMin Initialize to [%d]!Get_encoderval=[%d] \n",MeasureParameters.LimitStepMin,Get_Encoder_Vol(Focus_MCP3221_addr));
					MeasureParameters.MeasureFocusSum = 0;
					MeasureParameters.MeasureFocusNum = 0;
					MeasureParameters.MeasureFocusFlag = 2;
					// ts_s16 encoder = Get_Encoder_Vol(Focus_MCP3221_addr);
					// vision_log_debug("[1]FFF_Encoder_Clear [%d]! \n",encoder);
				}

				delay_1ms(250);
				FocusLastRange_en = NowRange;
				break;
			}else{
				Focus_count++;
				if(Focus_count>=3){
					Focus_count = 0;
					FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2); //1
					MeasureParameters.focusForward = MotorReversal;
					MeasureParameters.focusReversal = MotorForward;
					vision_log_debug("[1]FFF_First Reverse direction!!! LastRange =[%d] \n",FocusLastRange_en);
				}
				delay_1ms(250);
				FocusLastRange_en = NowRange;
				break;
			}
     	 }
		 case 2:
		 {
			 NowSite = Get_Encoder_Vol(Focus_MCP3221_addr);
		     NowRange = NowSite;
			 vision_log_debug("[2]FFF_Measure Maxval = [%d],lastVal = [%d] \n",NowRange,FocusLastRange_en);
		     FocusMotorOperate(NormalState,Speedset1,MeasureParameters.focusForward,2); //0

			if(NowRange >= FocusLastRange_en)
			{
			 if(Focus_count)	{Focus_count = 0;}
     		 if(MeasureParameters.MeasureFocusNum)
     		 {
     			if(abs(NowRange-FocusLastRange_en) > 0x20)
     			{
     				MeasureParameters.MeasureFocusNum = 0;
     				MeasureParameters.MeasureFocusSum = 0;
     			}
     		 }

			 if(abs(NowRange-FocusLastRange_en) <= 0x20)
			 {
				 MeasureParameters.MeasureFocusSum += NowSite;
				 MeasureParameters.MeasureFocusNum++;
			 }
			 if(MeasureParameters.MeasureFocusNum == 5)
			 {
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				MeasureParameters.LimitStepMax = (MeasureParameters.MeasureFocusSum/MeasureParameters.MeasureFocusNum);
				MeasureParameters.LimitStepMax -= 100; //冗余100
				MeasureParameters.LimitStepMin = 625 + 100; //冗余100
				vision_log_debug("[2]FFF---MEASURE_LimitFocusMax=[%d] LimitFocusMin=[%d] ---FFF\n", MeasureParameters.LimitStepMax,MeasureParameters.LimitStepMin);
				
				MeasureParameters.MeasureFocusFlag = 4;
				}
			}
			else{
				Focus_count++;
				if(Focus_count >= 3){
					Focus_count = 0;
					FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
					MeasureParameters.MeasureFocusFlag = 1;
					FocusLastRange_en = 0;
					//方向取反
					MeasureParameters.focusForward = MotorReversal;
					MeasureParameters.focusReversal = MotorForward;
					vision_log_debug("[2]FFF_Second FocusRangeMeasure Reverse Direction!!!!\n");
					break;
				}

			}
			delay_1ms(250);
 			FocusLastRange_en = NowRange;
			break;
		 }

		case 3:
			if(Get_Encoder_Vol(Focus_MCP3221_addr) > MeasureParameters.LimitStepMin){
				FocusMotorOperate(NormalState,Speedset1,MeasureParameters.focusReversal,2);
			}else{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(100);
				MeasureParameters.MeasureFocusFlag = 4;
				vision_log_debug("[3]FFF_Final MeasureMin is [%d]\n",Get_Encoder_Vol(Focus_MCP3221_addr));
			}
			break;

		case 4:
			FocusParameters.FocusStepMax = MeasureParameters.LimitStepMax;
			FocusParameters.FocusStepMin = MeasureParameters.LimitStepMin;
			vision_log_debug("[4]FFF---MeasureFocus Finish!\n");

			MeasureParameters.MeasureFocusFlag = 0;
			if(!MeasureParameters.MeasureZoomFlag){
				// Write_TestmenuParameterToFlash();
				FocusParameters.FocusFlag = 1;
			}

			break;
		default:
            break;
		}
	}
}

void FocusRangeMeasure(void)
{
/*******************电位器模式***************************/
     u32 NowSite;
	 u16 NowRange=0;
	 u32 temp;
	 static u16 FocusLastRange = 0;

	 if(FocusParameters.FocusFlag == 0)
	 {

	 switch(MeasureParameters.MeasureFocusFlag)
	 {
     	 case 1:
     	 {
     		 NowSite = GetVol(Focus_MCP3221_addr);
     		 NowRange = (uint16_t)NowSite;
     		FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusForward,2);

     		 if(MeasureParameters.MeasureFocusNum)
     		 {
     			if(abs(NowRange-FocusLastRange) > 0x20)
     			{
     				MeasureParameters.MeasureFocusNum = 0;
     				MeasureParameters.MeasureFocusSum = 0;
     			}
     		 }

     		 if(abs(NowRange-FocusLastRange) <= 0x20)
     		 {
     			 MeasureParameters.MeasureFocusNum++;
     			 MeasureParameters.MeasureFocusSum += NowSite;
     		 }
 			 if(MeasureParameters.MeasureFocusNum == 5)
 			 {
 				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);

 				MeasureParameters.LimitStepMax = (MeasureParameters.MeasureFocusSum/MeasureParameters.MeasureFocusNum);

 				MeasureParameters.MeasureFocusSum = 0;
 				MeasureParameters.MeasureFocusNum = 0;
 				MeasureParameters.MeasureFocusFlag = 2;
 			 }

 			 delay_1ms(250);
 			FocusLastRange = NowRange;
     		 break;
     	 }
		 case 2:
		 {
			 NowSite = GetVol(Focus_MCP3221_addr);
		     NowRange = (uint16_t)NowSite;
		     FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);

     		 if(MeasureParameters.MeasureFocusNum)
     		 {
     			if(abs(NowRange-FocusLastRange) > 0x20)
     			{
     				MeasureParameters.MeasureFocusNum = 0;
     				MeasureParameters.MeasureFocusSum = 0;
     			}
     		 }

			 if(abs(NowRange-FocusLastRange) <= 0x20)
			 {
				 MeasureParameters.MeasureFocusSum += NowSite;
				 MeasureParameters.MeasureFocusNum++;
			 }
			 if(MeasureParameters.MeasureFocusNum == 5)
			 {
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				MeasureParameters.LimitStepMin = (MeasureParameters.MeasureFocusSum/MeasureParameters.MeasureFocusNum);

				if(MeasureParameters.LimitStepMin > MeasureParameters.LimitStepMax) {
					temp = MeasureParameters.LimitStepMin - 100;
					MeasureParameters.LimitStepMin = MeasureParameters.LimitStepMax + 100;
					MeasureParameters.LimitStepMax = temp;

					MeasureParameters.focusForward = MotorReversal;
					MeasureParameters.focusReversal = MotorForward;

					MeasureParameters.MeasureFocusFlag = 3;
					vision_log_notice("FFFFF---MotorFocus For=Rev[0] Rev=For[1]!\n");
				}else{
					MeasureParameters.LimitStepMin += 100;
					MeasureParameters.LimitStepMax -= 100;

//					MeasureParameters.focusForward = MotorForward;
//					MeasureParameters.focusReversal = MotorReversal;

					MeasureParameters.MeasureFocusFlag = 4;
				}
				vision_log_notice("FFFFF---The last LimitFocusMax=[%#x] LimitFocusMin=[%#x]\n", MeasureParameters.LimitStepMax,MeasureParameters.LimitStepMin);
			}

 			delay_1ms(250);
 			FocusLastRange = NowRange;
			break;
		 }

		case 3:
			if(GetVol(Focus_MCP3221_addr) > MeasureParameters.LimitStepMin){
				FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);
			}else{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				MeasureParameters.MeasureFocusFlag = 4;
			}
			break;

		case 4:
			if(SystemParaMenu2.FocusSite && SystemParaMenu2.FocusSite!=0xFFFF)
			{
				//Read_TestmenuParameterFromFlash();
				FocusParameters.FocusStepMax = SystemParaMenu2.FocusSite + SystemParaMenu2.Left_Val;
				FocusParameters.FocusStepMin = SystemParaMenu2.FocusSite - SystemParaMenu2.Right_Val;
			}
			else
			{
				FocusParameters.FocusStepMax = MeasureParameters.LimitStepMax;
				FocusParameters.FocusStepMin = MeasureParameters.LimitStepMin;
				vision_log_notice("FFFFF---MeasureFocus Finish!\n");
			}

			MeasureParameters.MeasureFocusFlag = 0;
			if(!MeasureParameters.MeasureZoomFlag){
				// Write_TestmenuParameterToFlash();
				FocusParameters.FocusFlag = 1;
			}

			break;
		default:
            break;
	}
	 }
}

void ZoomRangeMeasure_encoder(void)
{
	/*************************编码器模式 *****************/
	s32 NowSite;
	s32 NowRange=0;
	// u16 temp;
	static s32 zoomLastRange_en = 0;
	static u8 Zoom_count = 0;

	switch(MeasureParameters.MeasureZoomFlag)
	{
		case 1:
		{
			NowSite = Get_Encoder_Vol(Zoom_MCP3221_addr);
			NowRange = NowSite;
			vision_log_notice("[1]ZZZ_ZoomMin_Now=[%d],zoomLastRange_en=[%d]\n", NowRange,zoomLastRange_en);
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
			if(NowRange <= zoomLastRange_en)
			{
				if(Zoom_count)	{Zoom_count = 0;}
				if(MeasureParameters.MeasureZoomNum)
				{
					if(abs(NowRange-zoomLastRange_en) > 0x20)
					{
						MeasureParameters.MeasureZoomNum = 0;
						MeasureParameters.MeasureZoomSum = 0;
					}
				}

				if(abs(NowRange-zoomLastRange_en) <= 0x20)
				{
					MeasureParameters.MeasureZoomSum += NowSite;
					MeasureParameters.MeasureZoomNum++;
				}
				if(MeasureParameters.MeasureZoomNum == 5)
				{
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);
					
					// MeasureParameters.LimitZoomMin = (MeasureParameters.MeasureZoomSum/MeasureParameters.MeasureZoomNum);
					MeasureParameters.LimitZoomMin = 625;//10000;
					Encoder_Clear(MeasureParameters.LimitZoomMin,Zoom_MCP3221_addr); //重置编码器的值
					MeasureParameters.MeasureZoomSum = 0;
					MeasureParameters.MeasureZoomNum = 0;
					MeasureParameters.MeasureZoomFlag = 2;
					
					vision_log_notice("[1]ZZZ_LimitZoomMin Set[%d] \n",MeasureParameters.LimitZoomMin);
					// ts_s16 encoder = Get_Encoder_Vol(Zoom_MCP3221_addr);
					// vision_log_debug("[1]ZZZ_Encoder_Clear [%d]! \n",encoder);
				}
			}else
			{
				Zoom_count++;
				if(Zoom_count >= 3){
					Zoom_count = 0;
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);
					MeasureParameters.zoomForward = MotorReversal;
					MeasureParameters.zoomReversal = MotorForward;
					vision_log_notice("[1]ZZZ_First Zoom Reverse direction![1->0,0->1] \n");
				}
			}

			delay_1ms(250);
			zoomLastRange_en = NowRange;
			break;
		}
		
		case 2:
		{
			NowSite = Get_Encoder_Vol(Zoom_MCP3221_addr);
			NowRange = NowSite;
			vision_log_notice("[2]ZZZ_ZoomMax_Now=[%d],zoomLastRange_en=[%d]\n", NowRange,zoomLastRange_en);
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);

			if(NowRange >= zoomLastRange_en)
			{
				if(Zoom_count)	{Zoom_count = 0;}
				if(MeasureParameters.MeasureZoomNum)
				{
					if(abs(NowRange-zoomLastRange_en) > 0x20)
					{
						MeasureParameters.MeasureZoomNum = 0;
						MeasureParameters.MeasureZoomSum = 0;
					}
				}

				if(abs(NowRange-zoomLastRange_en) <= 0x20)
				{
					MeasureParameters.MeasureZoomNum++;
					MeasureParameters.MeasureZoomSum += NowSite;
				}
				if(MeasureParameters.MeasureZoomNum == 5)
				{
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);

					MeasureParameters.LimitZoomMax = (MeasureParameters.MeasureZoomSum/MeasureParameters.MeasureZoomNum);
					vision_log_notice("[2]ZZZ_LimitZoomMax Initialize to [%d]! \n",MeasureParameters.LimitZoomMax);
					MeasureParameters.MeasureZoomSum = 0;
					MeasureParameters.MeasureZoomNum = 0;
					MeasureParameters.MeasureZoomFlag = 3;
				}
			}
			else{
				Zoom_count++;
				if(Zoom_count>=3){
					Zoom_count = 0;
					zoomLastRange_en = 0;
					MeasureParameters.MeasureZoomFlag = 1;
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2); //1
					MeasureParameters.zoomForward = MotorReversal;
					MeasureParameters.zoomReversal = MotorForward;
					vision_log_debug("[2]ZZZ_Second ZoomRangeMeasure Reverse direction!!!!\n");
					break;
				}
			}

			delay_1ms(250);
			zoomLastRange_en = NowRange;
			break;
		}
		case 3:
		{
			NowSite = Get_Encoder_Vol(Zoom_MCP3221_addr);
			NowRange = NowSite;
			vision_log_notice("[3]ZZZ_ZoomMin final=[%d],LastRange=[%d]\n", NowRange,zoomLastRange_en);
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);

				if(MeasureParameters.MeasureZoomNum)
				{
					if(abs(NowRange-zoomLastRange_en) > 0x20)
					{
						MeasureParameters.MeasureZoomNum = 0;
						MeasureParameters.MeasureZoomSum = 0;
					}
				}

				if(abs(NowRange-zoomLastRange_en) <= 0x20)
				{
					MeasureParameters.MeasureZoomSum += NowSite;
					MeasureParameters.MeasureZoomNum++;
				}
				if(MeasureParameters.MeasureZoomNum == 5)
				{
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);
					
					// MeasureParameters.LimitZoomMin = (MeasureParameters.MeasureZoomSum/MeasureParameters.MeasureZoomNum);
					MeasureParameters.LimitZoomMin = 625 + 100;//10000;
					MeasureParameters.LimitZoomMax -= 100; //留100的冗余范围
					Encoder_Clear(MeasureParameters.LimitZoomMin,Zoom_MCP3221_addr); //重置编码器的值
					
					MeasureParameters.MeasureZoomFlag = 0;
					if(!MeasureParameters.MeasureFocusFlag)	{FocusParameters.FocusFlag = 1;}
					
					vision_log_notice("[3]ZZZ---Finish! Measure LimitZoomMax=[%d] ZoomMin=[%d] and FinalMinval= [%d] \n", MeasureParameters.LimitZoomMax,MeasureParameters.LimitZoomMin,Get_Encoder_Vol(Zoom_MCP3221_addr));
					// ts_s16 encoder = Get_Encoder_Vol(Zoom_MCP3221_addr);
					// vision_log_debug("[3]ZZZ_Encoder_Clear [%d]! \n",encoder);
				}

			delay_1ms(250);
			zoomLastRange_en = NowRange;
			break;
		}

	// case 4:
		// if(GetVol(Zoom_MCP3221_addr) > MeasureParameters.LimitZoomMin){
		// 	ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
		// }else{
		// 	ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);
		// 	MeasureParameters.MeasureZoomFlag = 0;
		// 	if(!MeasureParameters.MeasureFocusFlag){
		// 		// Write_TestmenuParameterToFlash();
		// 		FocusParameters.FocusFlag = 1;
		// 	}
		// 	vision_log_notice("ZZZZZ---For=Rev=0,Rev=For=1,LimitZoomMax[%#x] ZoomMin[%#x]\n", MeasureParameters.LimitZoomMax,MeasureParameters.LimitZoomMin);
		// }
		// break;

	default:
		break;
	}
}

void ZoomRangeMeasure(void){
/*******************电位器模式***************************/
    u32 NowSite;
	 u16 NowRange=0;
	 u16 temp;
	 static u16 zoomLastRange = 0;

	switch(MeasureParameters.MeasureZoomFlag)
	{
	     	 case 1:
	     	 {
	     		 NowSite = GetVol(Zoom_MCP3221_addr);
	     		 NowRange = (uint16_t)NowSite;
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//	     		 ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
//#else
	     		 ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
//#endif

	     		 if(MeasureParameters.MeasureZoomNum)
	     		 {
	     			if(abs(NowRange-zoomLastRange) > 0x20)
	     			{
	     				MeasureParameters.MeasureZoomNum = 0;
	     				MeasureParameters.MeasureZoomSum = 0;
	     			}
	     		 }

	     		 if(abs(NowRange-zoomLastRange) <= 0x20)
	     		 {
	     			 MeasureParameters.MeasureZoomNum++;
	     			 MeasureParameters.MeasureZoomSum += NowSite;
	     		 }
	 			 if(MeasureParameters.MeasureZoomNum == 5)
	 			 {
	 				ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);

	 				MeasureParameters.LimitZoomMax = (MeasureParameters.MeasureZoomSum/MeasureParameters.MeasureZoomNum);
					
	 				MeasureParameters.MeasureZoomSum = 0;
	 				MeasureParameters.MeasureZoomNum = 0;
	 				MeasureParameters.MeasureZoomFlag = 2;
	 			 }

	 			 delay_1ms(250);
	 			zoomLastRange = NowRange;
	     		 break;
	     	 }
			 case 2:
			 {
				 NowSite = GetVol(Zoom_MCP3221_addr);
			     NowRange = (uint16_t)NowSite;

//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			     ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
//#else
			     ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
//#endif
	     		 if(MeasureParameters.MeasureZoomNum)
	     		 {
	     			if(abs(NowRange-zoomLastRange) > 0x20)
	     			{
	     				MeasureParameters.MeasureZoomNum = 0;
	     				MeasureParameters.MeasureZoomSum = 0;
	     			}
	     		 }

				 if(abs(NowRange-zoomLastRange) <= 0x20)
				 {
					 MeasureParameters.MeasureZoomSum += NowSite;
					 MeasureParameters.MeasureZoomNum++;
				 }
				 if(MeasureParameters.MeasureZoomNum == 5)
				 {
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);
					MeasureParameters.LimitZoomMin = (MeasureParameters.MeasureZoomSum/MeasureParameters.MeasureZoomNum);
					if(MeasureParameters.LimitZoomMin > MeasureParameters.LimitZoomMax) {
						temp = MeasureParameters.LimitZoomMin - 15; //10
						MeasureParameters.LimitZoomMin = MeasureParameters.LimitZoomMax + 15; //10
						MeasureParameters.LimitZoomMax = temp;

						MeasureParameters.zoomForward = MotorReversal;
						MeasureParameters.zoomReversal = MotorForward;

						MeasureParameters.MeasureZoomFlag = 3;
					}else{
						MeasureParameters.LimitZoomMin += 15; //10
						MeasureParameters.LimitZoomMax -= 15; //10

//						MeasureParameters.zoomForward = MotorForward;
//						MeasureParameters.zoomReversal = MotorReversal;

						MeasureParameters.MeasureZoomFlag = 0;

		 				if(!MeasureParameters.MeasureFocusFlag)
		 				FocusParameters.FocusFlag = 1;

						vision_log_notice("ZZZZZ---Measure LimitZoomMax=[%#x] ZoomMin=[%#x] and Finish!\n", MeasureParameters.LimitZoomMax,MeasureParameters.LimitZoomMin);
					}
				}

	 			delay_1ms(250);
	 			zoomLastRange = NowRange;
				break;
			 }

			case 3:
				if(GetVol(Zoom_MCP3221_addr) > MeasureParameters.LimitZoomMin){
					ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
				}else{
					ZoomMotorOperate(ForceStopState,StepSpeed,MeasureParameters.zoomForward,2);

					MeasureParameters.MeasureZoomFlag = 0;

	 				if(!MeasureParameters.MeasureFocusFlag){
	 					// Write_TestmenuParameterToFlash();
		 				FocusParameters.FocusFlag = 1;
	 				}
					vision_log_notice("ZZZZZ---For=Rev=0,Rev=For=1,LimitZoomMax[%#x] ZoomMin[%#x]\n", MeasureParameters.LimitZoomMax,MeasureParameters.LimitZoomMin);

				}
				break;

			default:
	            break;
		}
}


void MotorAutoFocus(void)
{
	ts_u16 max_width, max_heigth;
	v_getDector_DisplayResolution(&max_width, &max_heigth);

	if(MeasureParameters.MeasureFocusFlag)
	{
		switch(FocusParameters.FocusFlag)
		{
			default:
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				FocusParameters.FocusFlag=0;
				return;
			case 0:
				return;
		}
	}
/*******************OldFocusAlgorithm*******************************/
	if(SystemParaMenu2.Zoom_Lens == 0)
	{
#if 0
    switch(FocusParameters.FocusFlag)
	{
    	case 1:
    		FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);
			if(GetVol(Focus_MCP3221_addr) <= FocusParameters.FocusStepMin)
			{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				FocusParameters.NowSite = FocusParameters.FocusStepMin;
				FocusParameters.FocusFlag = 2;
			}
			break;
    	case 2:
    		if(FocusParameters.NowSite < FocusParameters.FocusStepMax)
    		{
    			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);

				for(u8 i = 0; i<9 ;i++)
				{
					FocusParameters.ImageValue[i] = ReadImageSum();
				}

				BubbleSort(FocusParameters.ImageValue,9);

				if(FocusParameters.ImageValue[4] >= FocusParameters.MaxValue)
				{
					FocusParameters.MaxValue = FocusParameters.ImageValue[4];
					FocusParameters.FocusSite = FocusParameters.NowSite;
				}
    		}
    		else
    		{
    			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				FocusParameters.FocusFlag=3;
    		}
    		break;
    	case 3:
			FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);
    		if(FocusParameters.NowSite <= FocusParameters.FocusSite)
			{
    			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				FocusParameters.MaxValue= 0;
				memset(FocusParameters.ImageValue, 0, sizeof (FocusParameters.ImageValue));
				FocusParameters.FocusFlag=4;
			}
    		break;
    	case 4:
    		FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
    		if(FocusParameters.NowSite < FocusParameters.FocusSite)
    		{
    			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    		}
    		else
    		{
    			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    			delay_1ms(50);
				FocusParameters.BackSteps = 0;
				FocusParameters.FocusFlag=5;
    		}
            break;

    	case 5:
			if(FocusParameters.BackSteps<10)
			{
				FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);

				for(u8 i = 0; i<9 ;i++)
				{
					FocusParameters.ImageValue[i] = ReadImageSum();
				}

				BubbleSort(FocusParameters.ImageValue,9);

				if(FocusParameters.ImageValue[4]>=FocusParameters.MaxValue)
				{
					FocusParameters.MaxValue = FocusParameters.ImageValue[4];
					FocusParameters.ValidSteps = FocusParameters.BackSteps;
				}

				delay_1ms(200);
			}
			else
			{
				FocusParameters.BackSteps = 0;
				FocusParameters.FocusFlag=6;
				break;
			}
			FocusParameters.BackSteps++;
            break;


    	case 6:
			if(FocusParameters.BackSteps< 10-FocusParameters.ValidSteps)
			{
				FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(200);
			}
			if(FocusParameters.BackSteps == 10 )
			{
				if(FocusParameters.FocusStepMax == MeasureParameters.LimitStepMax&&\
				FocusParameters.FocusStepMin == MeasureParameters.LimitStepMin)
				{
					SystemParaMenu2.FocusSite = GetVol(Focus_MCP3221_addr);
					// Write_TestmenuParameterToFlash();
				}
				FocusParameters.FocusFlag=0;
				break;
			}
			else
			{
				FocusParameters.BackSteps++;
			}
          break;
      default:
          break;
  }
#endif
	}
/*******************NewFocusAlgorithm*******************************/
	else
	{
	switch(FocusParameters.FocusFlag)
	{
    	case 1:
			ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 1);
			semiAutoMode_DrawRect(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth, SystemParaMenu2.Focus_Lx, SystemParaMenu2.Focus_Ly,SystemParaMenu2.Focus_Rx, SystemParaMenu2.Focus_Ry);

			vision_log_debug("----Start AutoFocus!!!Width[%d]Height[%d]----\n",max_width,max_heigth);
    		FocusMotorOperate(NormalState,FocusParameters.FocusSpeed,MeasureParameters.focusReversal,2);
			// FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
    		delay_1ms(50);
    		FocusParameters.FocusFlag = 2;
			break;
    	case 2:
    		FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
    		FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
			// vision_log_debug("Step[2]---NowSite = [%d][0x%x] \n",FocusParameters.NowSite,FocusParameters.NowSite);
		// #ifdef FunctionMotor_encoder
			if((FocusParameters.NowSite <= MeasureParameters.LimitStepMin)||(FocusParameters.NowSite>>16)) //异常值
		// #else
		// 	if(FocusParameters.NowSite <= MeasureParameters.LimitStepMin)
		// #endif
			{
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				FocusParameters.FocusFlag = 7;
				FocusParameters.UpNum = 0;
				FocusParameters.DownNum = 0;
				FocusParameters.MaxValue = 0;
				FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(500);
				vision_log_notice("Step[2]---Less than LimitStepMin And Go to Step[7],NowSite =%d \n",FocusParameters.NowSite);
				break;
			}
    		// for(u8 i = 0; i<8 ;i++){
    		// 	FocusParameters.ImageValue[i] = AFS_EvaluationFuc_Cal();
    		// }
    		// BubbleSort(FocusParameters.ImageValue,8);
			FocusParameters.ImageValue[4] = AFS_EvaluationFuc_Cal();

			if(FocusParameters.ImageValue[4] >= FocusParameters.MaxValue){
				FocusParameters.MaxValue = FocusParameters.ImageValue[4];
				FocusParameters.FocusSite = FocusParameters.NowSite;
			}
			
    		if(FocusParameters.DownNum){
    			if(FocusParameters.ImageValue[4] > FocusParameters.ImageValue[8]){
    				FocusParameters.DownNum = 0;
    			}
    		}
			vision_log_notice("Step[2]---Last_ImageValue = [%d] NewImageValue = [%d] MaxValue = [%d] \n",FocusParameters.ImageValue[8],FocusParameters.ImageValue[4],FocusParameters.MaxValue);
			if(FocusParameters.ImageValue[4] < FocusParameters.ImageValue[8]){
				FocusParameters.DownNum++;
//				if(FocusParameters.DownNum == 6){
//					FocusParameters.FocusSite = FocusParameters.NowSite;
//					//FocusParameters.MaxValue = FocusParameters.ImageValue[8];
//				}
				// if(FocusParameters.UpNum < 6)
				if(FocusParameters.UpNum < SystemParaMenu2.Focus_UNumSet)
				{
					FocusParameters.UpNum = 0;
				}
/******************进一步判断假峰值*************************/
			if(FocusParameters.DownNum==1)	{
				FocusParameters.DownImage[0] = FocusParameters.ImageValue[8];
				vision_log_notice("Step[2]---DownNum is D[%d]\n",FocusParameters.DownNum);}
			// else if(FocusParameters.DownNum==6)	
			else if(FocusParameters.DownNum==SystemParaMenu2.Focus_DNumSet)	
			{
				FocusParameters.DownImage[1] = FocusParameters.ImageValue[4];
				Calculate_PeakVal = FocusParameters.DownImage[0] - FocusParameters.DownImage[1];
				/*****************手动设置假峰值阈值*********************///暂时弃用
				// if(SystemParaMenu2.FocusPeakMode == 1){ 
				// 	if(Calculate_PeakVal <= SystemParaMenu2.Focus_PeakVal)
				// 	{
				// 		FocusParameters.DownNum = 0;
				// 		vision_log_debug("Step[2]+++SetPeakVal is [%d] And DownNum reset to 0!! \n",SystemParaMenu2.Focus_PeakVal);
				// 	}
				// }
				/*******************************************************/
				vision_log_debug("Step[2]---DownNum is D[%d] And Cal_PeakVal= [%d] UpNum is U[%d]\n",FocusParameters.DownNum,Calculate_PeakVal,FocusParameters.UpNum);}
			else{
				vision_log_notice("Step[2]---DownNum is D[%d]\n",FocusParameters.DownNum);
			}
/**********************************************************/
			// vision_log_debug("Step[2]---DownNum is D[%d] = [%d] \n",FocusParameters.DownNum,(FocusParameters.ImageValue[8] - FocusParameters.ImageValue[4]));

			// if(FocusParameters.DownNum == 6)
			if(FocusParameters.DownNum == SystemParaMenu2.Focus_DNumSet)
			{
					FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
					delay_1ms(50);
					// if(FocusParameters.UpNum > 6)
					if(FocusParameters.UpNum > SystemParaMenu2.Focus_UNumSet)
					{
						FocusParameters.FocusFlag = 3;
						break;
					}else{
						FocusParameters.DownNum = 0;
						FocusParameters.MaxValue = 0;
						FocusParameters.FocusFlag = 7;
						FocusMotorOperate(NormalState,FocusParameters.FocusSpeed,MeasureParameters.focusForward,2);
						delay_1ms(200);
						break;
					}
			}
			}else if(FocusParameters.ImageValue[4] > FocusParameters.ImageValue[8]){
//				FocusParameters.FocusSite = FocusParameters.NowSite;
				FocusParameters.UpNum++;
				vision_log_notice( "Step[2]---UpNum = U[%d]\n",FocusParameters.UpNum);
			}

			FocusParameters.ImageValue[8] = FocusParameters.ImageValue[4];
//			FocusParameters.LastSite = FocusParameters.NowSite;
    		break;
    	case 3:
			FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
    		if(FocusParameters.NowSite >= FocusParameters.FocusSite){
    			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
//				FocusParameters.MaxValue= 0;
//				memset(FocusParameters.ImageValue, 0, sizeof (FocusParameters.ImageValue));
				FocusParameters.FocusFlag = 4;
				vision_log_notice("Step[3]---NowSite = [%#x]>=FocusSite = [%#x]\n",FocusParameters.NowSite,FocusParameters.FocusSite);
			}else{
				FocusMotorOperate(NormalState,FocusParameters.FocusSpeed,MeasureParameters.focusForward,2);
			}
    		break;
    	case 4:
    		FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
//			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
//			delay_1ms(50);
//			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
//			delay_1ms(50);
    		if(FocusParameters.NowSite <= FocusParameters.FocusSite ){
    			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
    			delay_1ms(50);
    			FocusParameters.MaxValue = 0;
//    			FocusParameters.FocusSite = GetVol(Focus_MCP3221_addr);
    			FocusParameters.FocusFlag=5;
				vision_log_notice("Step[4]---NowSite = [%#x]<=FocusSite = [%#x]\n",FocusParameters.NowSite,FocusParameters.FocusSite);
    		}else{
    			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
    		}
    		break;
    	case 5:
			if(FocusParameters.BackSteps< 10 ){
				FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				// for(u8 i = 0; i<9 ;i++){
				// 	FocusParameters.ImageValue[i] = AFS_EvaluationFuc_Cal();
				// }

				// BubbleSort(FocusParameters.ImageValue,9);
				FocusParameters.ImageValue[4] = AFS_EvaluationFuc_Cal();
//				FocusParameters.ImageValue[4] = ReadImageSum();

				if(FocusParameters.ImageValue[4] >= FocusParameters.MaxValue){
					FocusParameters.MaxValue = FocusParameters.ImageValue[4];
					FocusParameters.ValidSteps = FocusParameters.BackSteps;
					FocusParameters.FocusSite = GetVol(Focus_MCP3221_addr);
//					bsp_printf("FocusParameters.FocusSite : %x",FocusParameters.FocusSite);
				}
				vision_log_notice("Step[5]---FineTure_ImageValue = [%d],MaxVal = [%d],Maxsite = [%#x]\n",FocusParameters.ImageValue[4],FocusParameters.MaxValue,FocusParameters.FocusSite);
			}else{
				FocusParameters.BackSteps = 0;
				FocusParameters.FocusFlag=6;
				vision_log_notice("Step[5]---ValidSteps = [%d]\n",FocusParameters.ValidSteps);
				break;
			}
			FocusParameters.BackSteps++;

    		break;
    	case 6:
//    		FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
//			if(FocusParameters.NowSite > FocusParameters.FocusSite)
    		if(FocusParameters.BackSteps< 10-FocusParameters.ValidSteps){
    			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				vision_log_notice("Step[6]---Readjust_Steps = [%d],is Backward(0)[%d]\n",FocusParameters.BackSteps,MeasureParameters.focusReversal);
			}
			if(FocusParameters.BackSteps == 10 ){
//				bsp_printf("Nowsite : %x",GetVol(Focus_MCP3221_addr));
				FocusParameters.FocusFlag=0;

		    	// VoltageSamplePara.SensorTemp = GetSensor();
		    	// VoltageSamplePara.VoltagePos = GetVol(Focus_MCP3221_addr);
		    	// VoltageSamplePara.Position = VoltageSamplePara.VoltagePos;
				vision_log_debug("Step[6]---backstep=[10] AutoFocus Finish!!!\n");
				ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
				// semi_auto_clear();
				semiAutoMode_clearScreen(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth);
				break;
			}else{
				FocusParameters.BackSteps++;
			}

//    		else{
//				FocusParameters.FocusFlag=0;
//			}

            break;
    	case 7:
    		FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    		FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
			// vision_log_debug("Step[7]---NowSite = [%d] \n",FocusParameters.NowSite);
			if(FocusParameters.NowSite >= MeasureParameters.LimitStepMax){
				FocusParameters.FocusFlag = 0;
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);

		    	//   VoltageSamplePara.SensorTemp = GetSensor();
		    	//   VoltageSamplePara.VoltagePos = GetVol(Focus_MCP3221_addr);
		    	//   VoltageSamplePara.Position = VoltageSamplePara.VoltagePos;
				  vision_log_notice("Step[7]---greater than LimitStepMax And over Focus!!\n");
				ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
				// semi_auto_clear();
				semiAutoMode_clearScreen(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth);
				break;
			}
    		// for(u8 i = 0; i<8 ;i++){
    		// 	FocusParameters.ImageValue[i] = AFS_EvaluationFuc_Cal();
    		// }
    		// BubbleSort(FocusParameters.ImageValue,8);
			FocusParameters.ImageValue[4] = AFS_EvaluationFuc_Cal();


			if(FocusParameters.ImageValue[4] >= FocusParameters.MaxValue){
				FocusParameters.MaxValue = FocusParameters.ImageValue[4];
				FocusParameters.FocusSite = FocusParameters.NowSite;
			}

    		if(FocusParameters.DownNum){
    			if(FocusParameters.ImageValue[4] > FocusParameters.ImageValue[8]){
    				FocusParameters.DownNum = 0;
    			}
    		}
		vision_log_notice("Step[7]---Last_ImageValue = [%d] NewImageValue = [%d] MaxValue = [%d] \n",FocusParameters.ImageValue[8],FocusParameters.ImageValue[4],FocusParameters.MaxValue);
			if(FocusParameters.ImageValue[4] < FocusParameters.ImageValue[8]){
				FocusParameters.DownNum++;
//				if(FocusParameters.DownNum == 6){
//					FocusParameters.FocusSite = FocusParameters.NowSite;
//					//FocusParameters.MaxValue = FocusParameters.ImageValue[8];
//				}
/******************进一步判断假峰值*************************/
			if(FocusParameters.DownNum==1)	{
				FocusParameters.DownImage[0] = FocusParameters.ImageValue[8];
				vision_log_notice("Step[7]---DownNum is D[%d]\n",FocusParameters.DownNum);}
			// else if(FocusParameters.DownNum==6)	
			else if(FocusParameters.DownNum== SystemParaMenu2.Focus_DNumSet)
			{
				FocusParameters.DownImage[1] = FocusParameters.ImageValue[4];
				Calculate_PeakVal = FocusParameters.DownImage[0] - FocusParameters.DownImage[1];
				/*****************手动设置假峰值阈值*********************///暂时弃用
				// if(SystemParaMenu2.FocusPeakMode == 1){ 
				// 	if(Calculate_PeakVal <= SystemParaMenu2.Focus_PeakVal)
				// 	{
				// 		FocusParameters.DownNum = 0;
				// 		vision_log_debug("Step[7]+++SetPeakVal is [%d] And DownNum reset to 0!! \n",SystemParaMenu2.Focus_PeakVal);
				// 	}
				// }
				/*******************************************************/
				vision_log_debug("Step[7]---DownNum is D[%d] And Cal_PeakVal= [%d] \n",FocusParameters.DownNum,Calculate_PeakVal);}
			else{
				vision_log_notice("Step[7]---DownNum is D[%d]\n",FocusParameters.DownNum);
			}
/********************************************************/			
			// vision_log_notice("Step[7]---DownNum is [%d] =[%d]\n", FocusParameters.DownNum,(FocusParameters.ImageValue[8] - FocusParameters.ImageValue[4]));
				// if(FocusParameters.DownNum == 6)
				 if(FocusParameters.DownNum== SystemParaMenu2.Focus_DNumSet)
				{
					FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
					delay_1ms(50);
					FocusParameters.FocusFlag = 8;
					break;
				}
			}

			FocusParameters.ImageValue[8] = FocusParameters.ImageValue[4];
//			else{
//				FocusParameters.FocusSite = FocusParameters.NowSite;
//			}

//			FocusParameters.LastSite = FocusParameters.NowSite;
//	        bsp_printf("ImageValue : %x",FocusParameters.ImageValue[4]);
            break;
    	case 8:
			FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
    		if(FocusParameters.NowSite <= FocusParameters.FocusSite){
    			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
//				FocusParameters.MaxValue= 0;
//				memset(FocusParameters.ImageValue, 0, sizeof (FocusParameters.ImageValue));
				FocusParameters.FocusFlag = 9;
				vision_log_notice("Step[8]---NowSite1 = [%#x]<=maxsite = [%#x]\n",FocusParameters.NowSite,FocusParameters.FocusSite);
			}else{
				FocusMotorOperate(NormalState,FocusParameters.FocusSpeed,MeasureParameters.focusReversal,2);
			}
    		break;
    	case 9:
    		FocusParameters.NowSite = GetVol(Focus_MCP3221_addr);
//			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
//			delay_1ms(50);
//			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
//			delay_1ms(50);
    		if(FocusParameters.NowSite >= FocusParameters.FocusSite){
    			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    			delay_1ms(50);
    			FocusParameters.MaxValue = 0;
//    			FocusParameters.FocusSite = GetVol(Focus_MCP3221_addr);
    			FocusParameters.FocusFlag=10;
				vision_log_notice("Step[9]---NowSite2 = [%#x]>=maxsite = [%#x]\n",FocusParameters.NowSite,FocusParameters.FocusSite);
    		}else{
    			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    		}
    		break;
    	case 10:
			if(FocusParameters.BackSteps < 10){
				FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				// for(u8 i = 0; i<8 ;i++){
				// 	FocusParameters.ImageValue[i] = AFS_EvaluationFuc_Cal();
				// }

				// BubbleSort(FocusParameters.ImageValue,8);
				FocusParameters.ImageValue[4] = AFS_EvaluationFuc_Cal();
//				FocusParameters.ImageValue[4] = ReadImageSum();
				if(FocusParameters.ImageValue[4] >= FocusParameters.MaxValue){
					FocusParameters.MaxValue = FocusParameters.ImageValue[4];
					FocusParameters.ValidSteps = FocusParameters.BackSteps;

					FocusParameters.FocusSite = GetVol(Focus_MCP3221_addr);
				}
				vision_log_notice("Step[10]---FineTure_ImageValue= [%d]And Maxval= [%d] MaxSite= [%#x]\n",FocusParameters.ImageValue[4],FocusParameters.MaxValue,FocusParameters.FocusSite);
			}else{
				FocusParameters.BackSteps = 0;
				FocusParameters.FocusFlag=11;
				vision_log_notice("Step[10]---ValidSteps = [%d]\n",FocusParameters.ValidSteps);
				break;
			}
			FocusParameters.BackSteps++;
            break;
       	case 11:
       		if(FocusParameters.BackSteps< 10-FocusParameters.ValidSteps){
       			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    			delay_1ms(50);
    			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
    			delay_1ms(50);

				//
				vision_log_notice("Step[11]---Readjust_Steps = [%d],is Forward(1)[%d]\n",FocusParameters.BackSteps,MeasureParameters.focusForward);
    		}
			if(FocusParameters.BackSteps == 10 ){
				FocusParameters.FocusFlag=0;

		    	//   VoltageSamplePara.SensorTemp = GetSensor();
		    	//   VoltageSamplePara.VoltagePos = GetVol(Focus_MCP3221_addr);
		    	//   VoltageSamplePara.Position = VoltageSamplePara.VoltagePos;
				  vision_log_debug("Step[11]---backstep=[10] AutoFocus Finish!!!\n");
				ts_lib_osd_set_visible_vot(global_sys_ctrl_info.m_special_info_osd_node, 0);
				semiAutoMode_clearScreen(global_sys_ctrl_info.m_special_info_osd_node, max_width, max_heigth);
				break;
			}else{
				FocusParameters.BackSteps++;
			}

//       		else{
//    			FocusParameters.FocusFlag=0;
//    		}
            break;

      default:
          break;
//#endif

	}
	}

}

/**************MotorZoomFunction************************************/

void ManualZoom(void){

	if(ZoomParameters.ZoomFlag){
		if(GetVol(Zoom_MCP3221_addr) + 30 < ZoomParameters.ZoomSite){
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
		}else if(GetVol(Zoom_MCP3221_addr) - 30 > ZoomParameters.ZoomSite){
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
		}else{
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomParameters.ZoomFlag = 0;
			FocusParaInit();
			FocusParameters.FocusFlag = 1;
		}
	}

}


/*************TEST****************/
void VoltageSamplePareInit(void){
#ifdef FunctuinMotorPreset
//	VoltageSamplePara.StartFlag = 0;
	VoltageSamplePara.SampleFlag = 1;
	VoltageSamplePara.Temperature = 2;
	VoltageSamplePara.VoltagePos = 0;
	VoltageSamplePara.SensorTemp = 0;
	VoltageSamplePara.Position = 0;
#endif
}

//
//void AutoFocusVoltageSample(void){
//	uint16_t Temp_int,Temp_point;
//	uint32_t Temp;
//	static u32 LastTemp;
//	float	TEM_Sensor;
//	float temp1,temp2,D_Value;
//
//	if(!VoltageSamplePara.StartFlag) return;
//
//	switch(VoltageSamplePara.SampleFlag){
//	case 0:
//		TEM_Sensor = READ_TMP117_Temp_ZL();
//		if(TEM_Sensor<0)
//		{
//			Temp_int= (u16) abs((int)TEM_Sensor);
//			//bsp_printf("Temp_int1= %x\n",Temp_int);
//			Temp_point =(u16)((TEM_Sensor+((float)Temp_int))*(-10000));
//		//	bsp_printf("point*10000= %x\n",Temp_point);
//			Temp_int = Temp_int |0x8000;
//
//		}
//		else
//		{
//			Temp_int = (u16)TEM_Sensor;
//			Temp_point = (u16)((TEM_Sensor-Temp_int)*10000);
//			Temp_int = Temp_int &0x7fff;
//
//		}
//
//		LastTemp = Temp_int<<16 | Temp_point;
//		temp1 = (float)((LastTemp>>16)&0x7fff) + ((float)(LastTemp & 0x0000ffff)/10000);
//		if(LastTemp & 0x80000000)
//			temp1 = temp1 * (-1);
//		bsp_printf("Now Sensor is  : %f",temp1);
//		FocusParaInit();
//		FocusParameters.FocusFlag = 1;
//		VoltageSamplePara.SampleFlag = 2;
//		break;
//	case 1:
//		TEM_Sensor = READ_TMP117_Temp_ZL();
//		if(TEM_Sensor<0)
//		{
//			Temp_int= (u16) abs((int)TEM_Sensor);
//			//bsp_printf("Temp_int1= %x\n",Temp_int);
//			Temp_point =(u16)((TEM_Sensor+((float)Temp_int))*(-10000));
//		//	bsp_printf("point*10000= %x\n",Temp_point);
//			Temp_int = Temp_int |0x8000;
//
//		}
//		else
//		{
//			Temp_int = (u16)TEM_Sensor;
//			Temp_point = (u16)((TEM_Sensor-Temp_int)*10000);
//			Temp_int = Temp_int &0x7fff;
//
//		}
//		Temp = Temp_int<<16 | Temp_point;
//
////		bsp_printf("Now Sensor is  : %x",Temp_point);
//
//		if((LastTemp & 0x80000000) == (Temp & 0x80000000)){
//			temp1 = (float)((LastTemp>>16)&0x7fff) + ((float)(LastTemp & 0x0000ffff)/10000);
//			temp2 = (float)((Temp>>16)&0x7fff) + ((float)(Temp & 0x0000ffff)/10000);
////			bsp_printf("Now Sensor is  : %f",temp1);
////			bsp_printf("Now Sensor is  : %f",temp2);
//		}else{
//			temp1 = (float)((LastTemp>>16)&0x7fff) + ((float)(LastTemp & 0x0000ffff)/10000);
//			temp1 = temp1 * (-1);
//			temp2 = (float)((Temp>>16)&0x7fff) + ((float)(Temp & 0x0000ffff)/10000);
////			bsp_printf("Now Sensor is  : %f",temp1);
////			bsp_printf("Now Sensor is  : %f",temp2);
//		}
//
//
//		D_Value = abs(temp1 - temp2);
//		if(D_Value >= VoltageSamplePara.Temperature){
//			if(Temp & 0x80000000)
//				temp2 = temp2 * (-1);
//			bsp_printf("Now Sensor is  : %f",temp2);
//			FocusParaInit();
//			FocusParameters.FocusFlag = 1;
//			VoltageSamplePara.SampleFlag = 2;
//			LastTemp = Temp;
//		}
//		break;
//	case 2:
//		if(FocusParameters.FocusFlag){
//			return;
//		}else{
//			bsp_printf("Now Voltage is  : %x",GetVol(Focus_MCP3221_addr));
//			VoltageSamplePara.SampleFlag = 1;
//		}
//		break;
//	default:
//		break;
//	}
//
//}

//void AutoFocusVoltageSample(void){
//	uint16_t Temp_int,Temp_point;
//	uint32_t Temp;
//	static u32 LastTemp;
//	float	TEM_Sensor;
//	float temp1,temp2,D_Value;
//
//	if(!VoltageSamplePara.StartFlag) return;
//
//	switch(VoltageSamplePara.SampleFlag){
//	case 0:
//		TEM_Sensor = READ_TMP112_Temp();
//		if(TEM_Sensor<0)
//		{
//			Temp_int= (u16) abs((int)TEM_Sensor);
//			//bsp_printf("Temp_int1= %x\n",Temp_int);
//			Temp_point =(u16)((TEM_Sensor+((float)Temp_int))*(-10000));
//		//	bsp_printf("point*10000= %x\n",Temp_point);
//			Temp_int = Temp_int |0x8000;
//
//		}
//		else
//		{
//			Temp_int = (u16)TEM_Sensor;
//			Temp_point = (u16)((TEM_Sensor-Temp_int)*10000);
//			Temp_int = Temp_int &0x7fff;
//
//		}
//
//		LastTemp = Temp_int<<16 | Temp_point;
//		temp1 = (float)((LastTemp>>16)&0x7fff) + ((float)(LastTemp & 0x0000ffff)/10000);
//		if(LastTemp & 0x80000000)
//			temp1 = temp1 * (-1);
//		bsp_printf("Now Sensor is  : %f",temp1);
//		FocusParaInit();
//		FocusParameters.FocusFlag = 1;
//		VoltageSamplePara.SampleFlag = 2;
//		break;
//	case 1:
//		TEM_Sensor = READ_TMP112_Temp();
//		if(TEM_Sensor<0)
//		{
//			Temp_int= (u16) abs((int)TEM_Sensor);
//			//bsp_printf("Temp_int1= %x\n",Temp_int);
//			Temp_point =(u16)((TEM_Sensor+((float)Temp_int))*(-10000));
//		//	bsp_printf("point*10000= %x\n",Temp_point);
//			Temp_int = Temp_int |0x8000;
//
//		}
//		else
//		{
//			Temp_int = (u16)TEM_Sensor;
//			Temp_point = (u16)((TEM_Sensor-Temp_int)*10000);
//			Temp_int = Temp_int &0x7fff;
//
//		}
//		Temp = Temp_int<<16 | Temp_point;
//
////		bsp_printf("Now Sensor is  : %x",Temp_point);
//
//		if((LastTemp & 0x80000000) == (Temp & 0x80000000)){
//			temp1 = (float)((LastTemp>>16)&0x7fff) + ((float)(LastTemp & 0x0000ffff)/10000);
//			temp2 = (float)((Temp>>16)&0x7fff) + ((float)(Temp & 0x0000ffff)/10000);
////			bsp_printf("Now Sensor is  : %f",temp1);
////			bsp_printf("Now Sensor is  : %f",temp2);
//		}else{
//			temp1 = (float)((LastTemp>>16)&0x7fff) + ((float)(LastTemp & 0x0000ffff)/10000);
//			temp1 = temp1 * (-1);
//			temp2 = (float)((Temp>>16)&0x7fff) + ((float)(Temp & 0x0000ffff)/10000);
////			bsp_printf("Now Sensor is  : %f",temp1);
////			bsp_printf("Now Sensor is  : %f",temp2);
//		}
//
//
//		D_Value = abs(temp1 - temp2);
//		if(D_Value >= VoltageSamplePara.Temperature){
//			if(Temp & 0x80000000)
//				temp2 = temp2 * (-1);
//			bsp_printf("Now Sensor is  : %f",temp2);
//			VoltageSamplePara.VoltagePos = VoltageSamplePara.VoltagePos -2;
////			FocusParaInit();
////			FocusParameters.FocusFlag = 1;
//			VoltageSamplePara.SampleFlag = 3;
//			LastTemp = Temp;
//		}
//		break;
//	case 2:
//		if(FocusParameters.FocusFlag){
//			return;
//		}else{
//			VoltageSamplePara.VoltagePos = GetVol(Focus_MCP3221_addr);
//			bsp_printf("Now Voltage is  : %x",VoltageSamplePara.VoltagePos);
//			VoltageSamplePara.SampleFlag = 1;
//		}
//		break;
//	case 3:
//		if(GetVol(Focus_MCP3221_addr) > VoltageSamplePara.VoltagePos){
//			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
//			delay_1ms(50);
//			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
//			delay_1ms(50);
//		}else{
//			VoltageSamplePara.SampleFlag = 2;
//		}
//		break;
//	default:
//		break;
//	}
//
//}

void AutoFocusVoltageSample(void){
#ifdef FunctuinMotorPreset
	if(!VoltageSamplePara.StartFlag) return;

//	bsp_printf("VoltageSamplePara.SampleFlag  : %x \n",VoltageSamplePara.SampleFlag);
	if(FocusParameters.FocusFlag ||MeasureParameters.MeasureFocusFlag||MeasureParameters.MeasureZoomFlag) return;

	switch(VoltageSamplePara.SampleFlag){
	case 1:
		VoltageSamplePara.SampleFlag = CompareTemperature(GetSensor());
		break;
	case 2:
		if(GetVol(Focus_MCP3221_addr) > VoltageSamplePara.VoltagePos){
			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
		}else{
			VoltageSamplePara.SampleFlag = 1;
		}
		break;
	case 3:
		if(GetVol(Focus_MCP3221_addr) < VoltageSamplePara.VoltagePos){
			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
		}else{
			VoltageSamplePara.SampleFlag = 1;
		}
		break;
	default:
		break;
	}
#endif
}

ts_f32 READ_TMP117_Temp(void)//调焦测温小板
{
	uint16_t Data_16bit;
	ts_f32 temp = 0;

	if(SystemParaMenu2.sensor){ //地址冲突
		// vision_log_notice("Motor sensor address conflict, return 0xFFFF!\n");
		return 0xFFFF;
	}
	Data_16bit = Device_I2c0_Data_Get(MotorSen_Addr,SlaveAddressByte4);
	// vision_log_notice("READ_TMP117 Data is: %04X \n",Data_16bit);
	delay_1ms(5);
	if(Data_16bit == 0xFFFF){
		return Data_16bit;}
	if(!(Data_16bit>>15))
	{
		temp  = (ts_f32)(Data_16bit*0.0078125);
	}
	else
	{
		Data_16bit = ~((Data_16bit)-1);
		temp  = 0 -(ts_f32)(Data_16bit)*0.0078125;
	}
	return temp;
}

float GetSensor(void){
#ifdef FunctuinMotorPreset
	u16 Temp_int,Temp_point;
	float	TEM_Sensor;
	float temp1;

	TEM_Sensor = READ_TMP117_Temp();
	if(TEM_Sensor<0)
	{
		Temp_int= (u16) abs((int)TEM_Sensor);
		//bsp_printf("Temp_int1= %x\n",Temp_int);
		Temp_point =(u16)((TEM_Sensor+((float)Temp_int))*(-10000));
	//	bsp_printf("point*10000= %x\n",Temp_point);
		Temp_int = Temp_int |0x8000;
	}
	else
	{
		Temp_int = (u16)TEM_Sensor;
		Temp_point = (u16)((TEM_Sensor-Temp_int)*10000);
		Temp_int = Temp_int &0x7fff;
	}
	temp1 = (float)((Temp_int)&0x7fff) + ((float)(Temp_point)/10000);
	if(Temp_int & 0x8000){
		temp1 = temp1 * (-1);}
	// vision_log_notice("Now Motor_Sensor222 is: %+.2f,int:%d,point:%d,TEM_Sensor:%f \n",temp1,Temp_int,Temp_point,TEM_Sensor);
	return temp1;
#endif
}

u8 CompareTemperature(float temp){
#ifdef FunctuinMotorPreset
	float D_Value;
	// static float temp3;

	D_Value = temp - VoltageSamplePara.SensorTemp;
	if(D_Value >= 1){
		VoltageSamplePara.Position = VoltageSamplePara.Position - 7.6;
		VoltageSamplePara.VoltagePos = VoltageSamplePara.Position;
		VoltageSamplePara.SensorTemp = temp;
//		bsp_printf("Now Pos is  : %f  \n",VoltageSamplePara.Position);
//		bsp_printf("Now Pos is  : %x  \n",VoltageSamplePara.VoltagePos);
//		bsp_printf("Now Sensor is  : %f  \n",VoltageSamplePara.SensorTemp);
		return 2;
	}else if(D_Value <= -1){
		VoltageSamplePara.Position = VoltageSamplePara.Position + 7.6;
		VoltageSamplePara.VoltagePos = VoltageSamplePara.Position;
		VoltageSamplePara.SensorTemp = temp;
//		bsp_printf("Now Pos is  : %f  \n",VoltageSamplePara.Position);
//		bsp_printf("Now Pos is  : %x  \n",VoltageSamplePara.VoltagePos);
//		bsp_printf("Now Sensor is  : %f  \n",VoltageSamplePara.SensorTemp);
		return 3;
	}else{
		return 1;
	}
#endif
	return 0;
}



/*****************MotorFlash**********************/
void Write_MotorParameterToFlash(void){

#ifdef FunctuinMotorPreset
	u16 i,m;
	UpdatePromptInfo("MotorFollowTab Saving...", 1,VOT_LOG_TYPE_MOTOR);
	for(i = 0;i < MOTORPRESET_NUM;i++){
		for(m = 0;m < MOTORPOS_NUM+1;m++){
			MotorStorageParameters.Focus_MotorSite[i][m] = MotorPresetParemeters[i].FocusMotorPos[m];
			MotorStorageParameters.Zoom_MotorSite[i][m] = MotorPresetParemeters[i].ZoomMotorPos[m];
		}
	}
	ISP_write_Motorparameter_cfg();
	UpdatePromptInfoWithTimeMs("MotorFollowTab save done!",2000,VOT_LOG_TYPE_MOTOR);
	vision_log_notice("Write_MotorParameterToFlash Success!!!\n");
#endif
}

void MotorFlashParaInit(void){
#ifdef FunctuinMotorPreset
	u16 i,j;
	// MotorParameterBuff[0] = FLASH_MOTOR_FLAG1;
	// MotorParameterBuff[1] = FLASH_MOTOR_FLAG2;

	for(i = 0;i < MOTORPRESET_NUM;i++){
		for(j = 0;j < MOTORPOS_NUM+1;j++){
			MotorPresetParemeters[i].FocusMotorPos[j] = 0;
			MotorPresetParemeters[i].ZoomMotorPos[j] = 0;
		}
	}
#endif
}

void Read_MotorParameterFromFlash(void){
#ifdef FunctuinMotorPreset
	u16 i,m;

	if(ISP_read_Motorparameter_cfg() != 0)
	{
		//如果读取失败，初始化参数并写入FLASH
		vision_log_notice("Read_MotorParameterFromFlash Failed!!!\n");
		MotorFlashParaInit();
		Write_MotorParameterToFlash();
		return;
	}
		
	for(i = 0; i < MOTORPRESET_NUM; i++){
		for(m = 0;m < MOTORPOS_NUM+1;m++){
			MotorPresetParemeters[i].FocusMotorPos[m] = MotorStorageParameters.Focus_MotorSite[i][m];
			MotorPresetParemeters[i].ZoomMotorPos[m] = MotorStorageParameters.Zoom_MotorSite[i][m];

		// if(i == MotorPresetParameters.PresetNum){
		// vision_log_notice("--Read_group[%d].Num[%d] = F[0x%04x]Z[0x%04x]--\n", i,m,MotorPresetParemeters[i].FocusMotorPos[m],MotorPresetParemeters[i].ZoomMotorPos[m]);
		// }
		}	
	}

#endif
}
/******************PresetPos***********************************/
void PresetPositionCalibration(void){
#ifdef FunctuinMotorPreset
	float D_Value;
	ts_s32 temp_vol;

	switch(MotorPresetParameters.CalibrationFlag){
	default:break;
	case 1:
		ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
		if(GetVol(Zoom_MCP3221_addr) > MeasureParameters.LimitZoomMax){
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			MotorPresetParameters.CalibrationFlag = 2;
			vision_log_notice("[1]ZoomMotor over MaxLimit And PresetPositionCal Start!\n");
		}
		break;
	case 2:
		if(MotorPresetParameters.CalibrationNum == MOTORPOS_NUM - 1)	{MotorPresetParameters.CalibrationPos = MeasureParameters.LimitZoomMin;}
		else{
			D_Value = (float)((MeasureParameters.LimitZoomMax - MeasureParameters.LimitZoomMin)/(MOTORPOS_NUM - 1));
			MotorPresetParameters.CalibrationPos = MeasureParameters.LimitZoomMax - (u16)(D_Value*((float)MotorPresetParameters.CalibrationNum));
		}
//		MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum] = MotorPresetParameters.CalibrationPos;
		MotorPresetParameters.CalibrationFlag = 3;
	if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){ //编码器处理
		if(MotorPresetParameters.CalibrationNum == 0)	
		{
			MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum] = (ts_s32)GetVol(Zoom_MCP3221_addr);
			MotorPresetParameters.CalibrationFlag = 5;
			 vision_log_notice("[2]Group[%d]Num[0] PresetPos set Max,ZoomSites=[0x%x]!\n",MotorPresetParameters.PresetNum,\
				MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum]); 
		}
	}
		vision_log_notice("[2]CalPosNum[%d],Zoom_pos[0x%x] !\n",MotorPresetParameters.CalibrationNum,MotorPresetParameters.CalibrationPos);
		break;
	case 3:
		temp_vol = (ts_s32)GetVol(Zoom_MCP3221_addr) - (ts_s32)MotorPresetParameters.CalibrationPos;
		// vision_log_notice("[3]GetZoomVol[%d]! \n",temp_vol);
		if(temp_vol > 0){
		// if(GetVol(Zoom_MCP3221_addr) > MotorPresetParameters.CalibrationPos){
		ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
		}else{
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomReversal,2);

#ifdef brakeMode
			MotorPresetParameters.CalibrationFlag = 5;
#else
			MotorPresetParameters.CalibrationFlag = 4;
#endif
		// vision_log_notice("[3]ZoomMotor to [4] \n");
		}
		break;
	case 4:
		temp_vol = (ts_s32)GetVol(Zoom_MCP3221_addr) - (ts_s32)MotorPresetParameters.CalibrationPos;
		// vision_log_notice("[4]GetZoomVol[%d]! \n",temp_vol);
		if(temp_vol < 0){
		// if(GetVol(Zoom_MCP3221_addr) < MotorPresetParameters.CalibrationPos){
			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			// vision_log_notice("[4]ZoomMotor Runing! <ZoomCal_pos!\n");
		}else{
			MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum] = (ts_s32)GetVol(Zoom_MCP3221_addr);
			MotorPresetParameters.CalibrationFlag = 5;
			 vision_log_notice("[4]WriteGoup[%d]Num[%d]ZoomSites=[0x%x]!\n",MotorPresetParameters.PresetNum,MotorPresetParameters.CalibrationNum,\
			 	MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum]); 
			
		}
		break;
	case 5:
		FocusParaInit();
		FocusParameters.FocusFlag = 1;
		MotorPresetParameters.CalibrationFlag = 6;
		vision_log_notice("[5]Follow_Focus Runing!\n");
		break;
	case 6:
		if(FocusParameters.FocusFlag){
			break;
		}else{
			MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[MotorPresetParameters.CalibrationNum] = (ts_s32)GetVol(Focus_MCP3221_addr);
			MotorPresetParameters.CalibrationFlag = 7;
			 vision_log_notice("[6]WriteGoup[%d]Num[%d]Focussites=[0x%x]!\n",MotorPresetParameters.PresetNum,MotorPresetParameters.CalibrationNum,\
				MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[MotorPresetParameters.CalibrationNum]);
			
			delay_1ms(1000);
		}
		break;
	case 7:
		if(MotorPresetParameters.CalibrationNum == MOTORPOS_NUM - 1){
			D_Value = (float)((MeasureParameters.LimitZoomMax - MeasureParameters.LimitZoomMin)/(MOTORPOS_NUM - 1));
			vision_log_notice("[7]D_Value = %+.2f\n",D_Value);
			MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum+1] = (ts_s32)((float)MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum] - D_Value);
			MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[MotorPresetParameters.CalibrationNum+1] = (ts_s32)Pos_Calibration_extend(D_Value);

			vision_log_notice("[7]Goup[%d]FZ_SitesNum[30] = [0x%x][0x%x]!\n",MotorPresetParameters.PresetNum,MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[MotorPresetParameters.CalibrationNum+1],\
				MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[MotorPresetParameters.CalibrationNum+1]);
			Write_MotorParameterToFlash();
			MotorPresetParameters.CalibrationFlag = 0;
			vision_log_debug("PresetPositionCalibration Finish!!!\n");
		}else{
			MotorPresetParameters.CalibrationNum++;
			MotorPresetParameters.CalibrationFlag = 2;
		}
		break;
	}
#endif
}

void FocusPresetPositionRightOperate(void){
#ifdef FunctuinMotorPreset

#ifdef brakeMode
	switch(FocusParameters.PresetRightFlag){
	default:break;
	case 1:
		if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusForward,2);
		}else{
			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusParameters.PresetRightFlag = 0;
		}
		break;
	}
#else
	switch(FocusParameters.PresetRightFlag){
	default:break;
	case 1:
		if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusForward,2);
		}else{
			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusParameters.PresetRightFlag = 2;
		}
		break;
	case 2:
		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
		}else{
			FocusParameters.PresetPos = 0;
			FocusParameters.PresetRightFlag = 0;
		}
		break;
	}
#endif

#endif
}

void FocusPresetPositionLeftOperate(void){
#ifdef FunctuinMotorPreset

#ifdef brakeMode
	switch(FocusParameters.PresetLeftFlag){
	default:break;
	case 1:
		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);
		}else{
			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusParameters.PresetLeftFlag = 0;
		}
		break;
	}
#else
	switch(FocusParameters.PresetLeftFlag){
	default:break;
	case 1:
		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusReversal,2);
		}else{
			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusParameters.PresetLeftFlag = 2;
		}
		break;
	case 2:
		if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
		}else{
			FocusParameters.PresetPos = 0;
			FocusParameters.PresetLeftFlag = 0;
		}
		break;
	}
#endif

#endif
}

void FocusPresetPositionOperate(void){
#ifdef FunctuinMotorPreset
	FocusPresetPositionLeftOperate();
	FocusPresetPositionRightOperate();
#endif
}


void ZoomPresetPositionRightOperate(void){
#ifdef FunctuinMotorPreset

#ifdef brakeMode
	switch(ZoomParameters.PresetRightFlag){
	default:break;
	case 1:
		if(GetVol(Zoom_MCP3221_addr) < ZoomParameters.PresetPos){
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
//#else
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
//#endif
		}else{
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomParameters.PresetRightFlag = 0;
		}
		break;
	}
#else
	switch(ZoomParameters.PresetRightFlag){
	default:break;
	case 1:
		if(GetVol(Zoom_MCP3221_addr) < ZoomParameters.PresetPos){
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
//#else
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
//#endif
		}else{
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomParameters.PresetRightFlag = 2;
		}
		break;
	case 2:
		if(GetVol(Zoom_MCP3221_addr) > ZoomParameters.PresetPos){
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
//			delay_1ms(50);
//			ZoomMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
//			delay_1ms(50);
//#else
			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
//#endif
		}else{
			ZoomParameters.PresetPos = 0;
			ZoomParameters.PresetRightFlag = 0;
		}
		break;
	}
#endif

#endif
}

void ZoomPresetPositionLeftOperate(void){
#ifdef FunctuinMotorPreset

#ifdef brakeMode
	switch(ZoomParameters.PresetLeftFlag){
	default:break;
	case 1:
		if(GetVol(Zoom_MCP3221_addr) > ZoomParameters.PresetPos){
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
//#else
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
//#endif
		}else{
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
			ZoomParameters.PresetLeftFlag = 0;
		}
		break;
	}
#else
	switch(ZoomParameters.PresetLeftFlag){
	default:break;
	case 1:
		if(GetVol(Zoom_MCP3221_addr) > ZoomParameters.PresetPos){
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomForward,2);
//#else
			ZoomMotorOperate(NormalState,FullSpeed,MeasureParameters.zoomReversal,2);
//#endif
		}else{
			ZoomMotorOperate(ForceStopState,FullSpeed,MeasureParameters.zoomReversal,2);
			delay_1ms(50);
			ZoomParameters.PresetLeftFlag = 2;
		}
		break;
	case 2:
		if(GetVol(Zoom_MCP3221_addr) < ZoomParameters.PresetPos){
//#if SUPPORT_ZhongLin_UART_PROTOCOL
//			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomReversal,2);
//			delay_1ms(50);
//			ZoomMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomReversal,2);
//			delay_1ms(50);
//#else
			ZoomMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
			ZoomMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.zoomForward,2);
			delay_1ms(50);
//#endif
		}else{
			ZoomParameters.PresetPos = 0;
			ZoomParameters.PresetLeftFlag = 0;
		}
		break;
	}
#endif

#endif
}

void ZoomPresetPositionOperate(void){
#ifdef FunctuinMotorPreset
	ZoomPresetPositionLeftOperate();
	ZoomPresetPositionRightOperate();
#endif
}

u8 JudgeMotorState(u8 state,u32 FocusPresetPos,u32 ZoomPresetPos){
#ifdef FunctuinMotorPreset
	switch(state){
	default:return 0;
	case FocusState:
		if(GetVol(Focus_MCP3221_addr) < FocusPresetPos){
			return 1;
		}else if(GetVol(Focus_MCP3221_addr) > FocusPresetPos){
			return 2;
		}else{
			return 0;
		}
		break;
	case ZoomState:
		if(GetVol(Zoom_MCP3221_addr) < ZoomPresetPos){
			return 4;
		}else if(GetVol(Zoom_MCP3221_addr) > ZoomPresetPos){
			return 8;
		}else{
			return 0;
		}
		break;
	case BothState:
		if(GetVol(Focus_MCP3221_addr) < FocusPresetPos && GetVol(Zoom_MCP3221_addr) < ZoomPresetPos){
			return 5;
		}else if(GetVol(Focus_MCP3221_addr) < FocusPresetPos && GetVol(Zoom_MCP3221_addr) > ZoomPresetPos){
			return 9;
		}else if(GetVol(Focus_MCP3221_addr) > FocusPresetPos && GetVol(Zoom_MCP3221_addr) < ZoomPresetPos){
			return 6;
		}else if(GetVol(Focus_MCP3221_addr) > FocusPresetPos && GetVol(Zoom_MCP3221_addr) > ZoomPresetPos){
			return 10;
		}else{
			return 0;
		}
		break;
	}
#endif
	return 0;
}

void SelectPresetFlag(u8 state,u32 FocusPresetPos,u32 ZoomPresetPos){
#ifdef FunctuinMotorPreset
	u8 FlagState;

	FlagState = JudgeMotorState(state,FocusPresetPos,ZoomPresetPos);
	FocusParameters.PresetRightFlag = (FlagState&(0x01)<<0)>>0;
	FocusParameters.PresetLeftFlag = (FlagState&(0x01)<<1)>>1;
	ZoomParameters.PresetRightFlag = (FlagState&(0x01)<<2)>>2;
	ZoomParameters.PresetLeftFlag = (FlagState&(0x01)<<3)>>3;

//	bsp_printf("Now Flag is  :  %x    ",FocusParameters.PresetRightFlag);
//	bsp_printf("Now Flag is  :  %x    ",FocusParameters.PresetLeftFlag);
//	bsp_printf("Now Flag is  :  %x    ",ZoomParameters.PresetRightFlag);
//	bsp_printf("Now Flag is  :  %x    ",ZoomParameters.PresetLeftFlag);
#endif
}

//void PresetPositionOperate(u8 state,u16 FocusPresetPos,u16 ZoomPresetPos){
//	switch(MotorPresetParameters.PresetSwitch){
//	default:break;
//	case 1:
//		MotorPresetParameters.PresetSwitch = JudgeMotorState(state,FocusPresetPos,ZoomPresetPos);
//	case 1:
//		if(GetVol(Focus_MCP3221_addr) < PresetPos){
//			FocusMotorOperate(NormalState,FullSpeed,MeasureParameters.focusForward,2);
//		}else{
//			FocusMotorOperate(ForceStopState,FullSpeed,MeasureParameters.focusForward,2);
//			delay_1ms(50);
//			FocusParameters.PresetRightFlag = 2;
//		}
//		break;
//	case 2:
//		if(GetVol(Focus_MCP3221_addr) > PresetPos){
//			FocusMotorOperate(NormalState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
//			delay_1ms(50);
//			FocusMotorOperate(ForceStopState,SystemParaMenu2.OperatorSpeed,MeasureParameters.focusReversal,2);
//			delay_1ms(50);
//		}else{
//			FocusParameters.PresetRightFlag = 0;
//		}
//		break;
//	}
//
//
//	FocusPresetPositionLeftOperate(PresetPos);
//	FocusPresetPositionRightOperate(PresetPos);
//}


void PresetPosSearch(u8 *SW){
#ifdef FunctuinMotorPreset
	u8 state;

//	FocusParameters.PresetPos = MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[15];
//	ZoomParameters.PresetPos = MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[15];

	if(*SW){
		if(!FocusParameters.PresetPos && !ZoomParameters.PresetPos){
			return;
		}else if(FocusParameters.PresetPos && !ZoomParameters.PresetPos){
			state = FocusState;
		}else if(!FocusParameters.PresetPos && ZoomParameters.PresetPos){
			state = ZoomState;
		}else{
			state = BothState;
		}

		SelectPresetFlag(state,FocusParameters.PresetPos,ZoomParameters.PresetPos);

		*SW = 0;
	}

	FocusPresetPositionOperate();
	ZoomPresetPositionOperate();
#endif
}


static u8 Follow_StopFlag = 0;

void FollowFocus(u8 SW,MotorFlashPara *MotorPos,ts_s32 NowSite){
#ifdef FunctuinMotorPreset
	u8 i;
	float D_Value;
	// static u8 OverFlag = 0;

	if(!SW)  return;

//	if(MotorPresetParameters.FollowFlag)

#ifdef brakeMode
	switch(MotorPresetParameters.FollowFlag){
	default:break;
	case 1:
		for(i = 0;i < MOTORPOS_NUM - 1;i++){
			if(NowSite <= MotorPos->ZoomMotorPos[i] && NowSite >= MotorPos->ZoomMotorPos[i+1]){
				D_Value = ((float)NowSite - (float)MotorPos->ZoomMotorPos[i+1])\
							/((float)MotorPos->ZoomMotorPos[i] - (float)MotorPos->ZoomMotorPos[i+1]);
//				if(((float)MotorPos.FocusMotorPos[i] - (float)MotorPos.FocusMotorPos[i+1]) > 0)
				FocusParameters.PresetPos = MotorPos->FocusMotorPos[i] - \
						((float)MotorPos->FocusMotorPos[i] - (float)MotorPos->FocusMotorPos[i+1])*D_Value;
				MotorPresetParameters.FollowFlag = JudgeMotorState(FocusState,FocusParameters.PresetPos,0)+1;
				return;
			}
		}
		if(OverFlag){
			MotorPresetParameters.FollowFlag = 4;
			OverFlag = 0;
		}
		break;
	case 2:
		if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusForward,2);
			OverFlag = 1;
		}else{
			if(MotorPresetParameters.ZoomOperateFlag){
				MotorPresetParameters.FollowFlag = 1;
			}else{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				MotorPresetParameters.FollowFlag = 0;
				FocusParameters.PresetPos = 0;
				OverFlag = 0;
			}
		}
		break;
	case 3:
		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusReversal,2);
			OverFlag = 1;
		}else{
			if(MotorPresetParameters.ZoomOperateFlag){
				MotorPresetParameters.FollowFlag = 1;
			}else{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				MotorPresetParameters.FollowFlag = 0;
				FocusParameters.PresetPos = 0;
				OverFlag = 0;
			}
		}
		break;
	case 4:
		if(GetVol(Focus_MCP3221_addr) >= MotorPos->FocusMotorPos[0] || GetVol(Focus_MCP3221_addr) <= MotorPos->FocusMotorPos[29]) {
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			MotorPresetParameters.FollowFlag = 0;
			FocusParameters.PresetPos = 0;
		}
		break;
	}
#else
	switch(MotorPresetParameters.FollowFlag){
	default:break;
	case 1:
	// vision_log_debug("[1]Follow Focus Start!!! \n");
		for(i = 0;i < MOTORPOS_NUM - 1;i++){
			if(NowSite <= MotorPos->ZoomMotorPos[i] && NowSite >= MotorPos->ZoomMotorPos[i+1]){
				D_Value = (float)(NowSite - MotorPos->ZoomMotorPos[i+1])\
							/(float)(MotorPos->ZoomMotorPos[i] - MotorPos->ZoomMotorPos[i+1]);
//				if(((float)MotorPos.FocusMotorPos[i] - (float)MotorPos.FocusMotorPos[i+1]) > 0)
				FocusParameters.PresetPos =(u32)((float)MotorPos->FocusMotorPos[i+1] + \
						((float)MotorPos->FocusMotorPos[i] - (float)MotorPos->FocusMotorPos[i+1])*D_Value);
				MotorPresetParameters.FollowFlag = JudgeMotorState(FocusState,FocusParameters.PresetPos,0)+1;

				vision_log_notice("[1]Zoom_Site1 = [0x%x],Focus_Pos = [0x%x],Now PresetNum=[%d],TabNum=[%d]--[%d] \n",NowSite,FocusParameters.PresetPos,MotorPresetParameters.PresetNum,i,i+1);
				return;
			}
			//加极限值限制
			if(NowSite > MotorPos->ZoomMotorPos[0])	{
				FocusParameters.PresetPos = MotorPos->FocusMotorPos[0];
				vision_log_notice("[1]Zoom_Site2 = [0x%x],Focus_PresetPosMax = [0x%x],PresetNum=[%d] \n",NowSite,FocusParameters.PresetPos,MotorPresetParameters.PresetNum);
				MotorPresetParameters.FollowFlag = JudgeMotorState(FocusState,FocusParameters.PresetPos,0)+1;return;}
			else if(NowSite < MotorPos->ZoomMotorPos[MOTORPOS_NUM - 1])
			{
//				FocusParameters.PresetPos = MotorPos->FocusMotorPos[MOTORPOS_NUM - 1];
				D_Value = (float)(NowSite - MotorPos->ZoomMotorPos[MOTORPOS_NUM])\
							/(float)(MotorPos->ZoomMotorPos[MOTORPOS_NUM - 1] - MotorPos->ZoomMotorPos[MOTORPOS_NUM]);
				FocusParameters.PresetPos =(u32)((float)MotorPos->FocusMotorPos[MOTORPOS_NUM] + \
						((float)MotorPos->FocusMotorPos[MOTORPOS_NUM - 1] - (float)MotorPos->FocusMotorPos[MOTORPOS_NUM])*D_Value);
				vision_log_notice("[1]Zoom_Site3 = [0x%x],Focus_PresetPosMin = [0x%x],PresetNum=[%d] \n",NowSite,FocusParameters.PresetPos,MotorPresetParameters.PresetNum);
				MotorPresetParameters.FollowFlag = JudgeMotorState(FocusState,FocusParameters.PresetPos,0)+1;return;
			}
		}
		// if(OverFlag){
		// 	MotorPresetParameters.FollowFlag = 4;
		// 	OverFlag = 0;
		// }
		break;
	case 2:
		if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){

		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){  //编码器跟焦
			if(Follow_StopFlag == 2){
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);
				vision_log_notice("[2]---Follow Focus Stop! \n");
			}
			Follow_StopFlag = 1;
		}
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusForward,2);
			// OverFlag = 1;
		}else{
			if(MotorPresetParameters.ZoomOperateFlag){
				MotorPresetParameters.FollowFlag = 1;
			}else{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusForward,2);
				delay_1ms(50);

			if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){ //编码器跟焦
				MotorPresetParameters.FollowFlag = 7;
			}else{
				MotorPresetParameters.FollowFlag = 5; 
			}
				vision_log_notice("[2]to[5]NowFocusSite > FocusPresetPos! \n");
			}
		}
		break;
	case 3:
		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){  //编码器跟焦
			if(Follow_StopFlag == 1){
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				vision_log_notice("[3]---Follow Focus Stop! \n");
			}
			Follow_StopFlag = 2;
		}
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusReversal,2);
			// OverFlag = 1;
		}else{
			if(MotorPresetParameters.ZoomOperateFlag){
				MotorPresetParameters.FollowFlag = 1;
			}else{
				FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
				delay_1ms(50);
				MotorPresetParameters.FollowFlag = 6;
				vision_log_notice("[3]to[6]NowFocusSite < FocusPresetPos! \n");
			}
		}
		break;
	case 4:
		FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
		delay_1ms(50);
		MotorPresetParameters.FollowFlag = 0;
		FocusParameters.PresetPos = 0;
		vision_log_debug("[4]Follow Focus Over!!! \n");
		break;
	case 5:
		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			vision_log_notice("[5]Focus single step Run!!! \n");
		}else{
			MotorPresetParameters.FollowFlag = 0;
			FocusParameters.PresetPos = 0;
			// OverFlag = 0;
		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){  //编码器跟焦
			Follow_StopFlag = 0;
		}
			vision_log_notice("[5]Speed[%d],NowsiteF[0x%x]<=Presetpos,Follow Focus Finish!!! \n",MotorPresetParameters.FocusPresetSpeed,GetVol(Focus_MCP3221_addr));
		}
		break;
	case 6:
		if(GetVol(Focus_MCP3221_addr) < FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			FocusMotorOperate(ForceStopState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusForward,2);
			delay_1ms(50);
			vision_log_notice("[6]Focus single step Run!!! \n");
		}else{
			MotorPresetParameters.FollowFlag = 0;
			FocusParameters.PresetPos = 0;
			// OverFlag = 0;
		if(SystemParaMenu2.m_force_focus_type == FOCUS_TYPE_ZOOM_LENS_ENCODER){  //编码器跟焦
			Follow_StopFlag = 0;
		}
			vision_log_notice("[6]Speed[%d],NowsiteF[0x%x]>=Presetpos,Follow Focus Finish!!! \n",MotorPresetParameters.FocusPresetSpeed,GetVol(Focus_MCP3221_addr));
		}
		break;

		case 7: //编码器跟焦

		if(GetVol(Focus_MCP3221_addr) > FocusParameters.PresetPos){
			FocusMotorOperate(NormalState,MotorPresetParameters.FocusPresetSpeed,MeasureParameters.focusReversal,2);
			// OverFlag = 1;
		}else{
			FocusMotorOperate(ForceStopState,StepSpeed,MeasureParameters.focusReversal,2);
			delay_1ms(50);
			MotorPresetParameters.FollowFlag = 6;
			vision_log_notice("encoder--[7]to[6]NowFocusSite < FocusPresetPos! \n");
		}

		break;
	}
#endif

#endif
}

u16 Pos_Calibration_extend(float DVal)
{
	float ZoomCal_30,Kval;
	u16 FocusCal;

	ZoomCal_30 = (float)MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[29] - DVal;
vision_log_notice("DVal = %+.2f,ZoomCal_30 = %+.2f\n",DVal,ZoomCal_30);
	Kval =(float) (MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[28] - MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[29])\
			/(float)(MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[28] - MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[29]);
vision_log_notice("Kval = %+.2f\n",Kval);
	FocusCal = (u16)((float)MotorPresetParemeters[MotorPresetParameters.PresetNum].FocusMotorPos[29] - ((float)MotorPresetParemeters[MotorPresetParameters.PresetNum].ZoomMotorPos[29] - ZoomCal_30)/ Kval);

	return FocusCal;
}

void MotorSensor_Read(void)
{
	s16 SenVal;
	if(SystemParaMenu2.MotorSenFlag == 0)	//单个标定表
	{
		MotorPresetParameters.PresetNum = 8;
	}else if(SystemParaMenu2.MotorSenFlag == 1) //多个标定表
	{								
		if(!FocusTable_AorM) 	{return;}
		ZoomMotorSen = GetSensor();
		SenVal = (s16)(ZoomMotorSen*10);

		if(SenVal <= -150)
		{
			MotorPresetParameters.PresetNum = 0;
		}else if((SenVal >= -145)&&(SenVal<= -100))
		{
			MotorPresetParameters.PresetNum = 1;
		}else if((SenVal >= -95)&&(SenVal<= -50))
		{
			MotorPresetParameters.PresetNum = 2;
		}else if((SenVal >= -45)&&(SenVal<= 0))
		{
			MotorPresetParameters.PresetNum = 3;
		}else if((SenVal >= 5)&&(SenVal<= 50))
		{
			MotorPresetParameters.PresetNum = 4;
		}else if((SenVal >= 55)&&(SenVal<= 100))
		{
			MotorPresetParameters.PresetNum = 5;
		}else if((SenVal >= 105)&&(SenVal<= 150))
		{
			MotorPresetParameters.PresetNum = 6;
		}else if((SenVal >= 155)&&(SenVal<= 200))
		{
			MotorPresetParameters.PresetNum = 7;
		}else if((SenVal >= 205)&&(SenVal<= 250))
		{
			MotorPresetParameters.PresetNum = 8;
		}else if((SenVal >= 255)&&(SenVal<= 300))
		{
			MotorPresetParameters.PresetNum = 9;
		}else if((SenVal >= 305)&&(SenVal<= 350))
		{
			MotorPresetParameters.PresetNum = 10;
		}else if((SenVal >= 355)&&(SenVal<= 400))
		{
			MotorPresetParameters.PresetNum = 11;
		}else if((SenVal >= 405)&&(SenVal<= 450))
		{
			MotorPresetParameters.PresetNum = 12;
		}else if((SenVal >= 455)&&(SenVal<= 500))
		{
			MotorPresetParameters.PresetNum = 13;
		}else if((SenVal >= 505)&&(SenVal<= 550))
		{
			MotorPresetParameters.PresetNum = 14;
		}
		else if(SenVal >= 555)
		{
			MotorPresetParameters.PresetNum = 15;
		}
	}
}




