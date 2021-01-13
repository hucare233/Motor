/*
 * @Descripttion: VESC
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2021-01-13 22:30:05
 * @FilePath: \MotoPro\USER\SRC\vesc.c
 */
#include "vesc.h"
//FIXME:位置模式
VESCParam SUNNYSKY, VESC_U10, VESC_U8;
VESCLimit VESClimit;
VESCArgum VESCargum;
VESC_MOTOR VESCmotor[4];

static float angle_gap = 90.f;
float dt = 1 / 20000.0f;//ASK:
/**
 * @author: 叮咚蛋
 * @brief: 电机参数初始化
 */

void VESCInit(void)
{
	{ //电机内参
		SUNNYSKY.POLE_PAIRS = 7;
		SUNNYSKY.MAX_CURRENT = 60.0f;
		SUNNYSKY.MAX_RPM = 1000;

		VESC_U10.POLE_PAIRS = 21;
		VESC_U10.MAX_RPM = 2100;

		VESC_U8.POLE_PAIRS = 18;
	}
	{
		//电机限制保护
		VESClimit.isPosLimitOn = false;
		VESClimit.maxPosition = 30000;
		VESClimit.isPosSPLimitOn = true;
		VESClimit.posSPlimit = 1000;
		VESClimit.maxCurrentSet = 10.f;
	}
	{								   //间值参数设置
		VESCargum.timeoutTicks = 1000; //100ms
		VESCargum.fistPos = true;
	}
	/****0号电机初始化****/
	VESCmotor[0].instrinsic = SUNNYSKY;
	VESCmotor[0].enable = DISABLE;
	VESCmotor[0].begin = true;
	VESCmotor[0].mode = vesc_RPM_I;
	VESCmotor[0].valSet.current = 0.0f;
	VESCmotor[0].valSet.speed = 500;
	VESCmotor[0].valSet.position = 0;
	VESCmotor[0].valSet.duty = 1;
	PID_Init(&VESCmotor[0].PIDx, 0.5, 0.045, 0, 1, VESCmotor[0].valSet.position);
	PID_Init(&VESCmotor[0].PIDs, 0.007, 0.001, 1, 1, VESCmotor[0].valSet.speed);
	VESC_PID_Init(&VESCmotor[0].p_pid, 0.03, 0, 0.0004, 0.2);
	VESC_PID_Init(&VESCmotor[0].s_pid, 0.1, 0.0001, 0.000001, 0.2);
	VESCmotor[0].limit = VESClimit;
	VESCmotor[0].limit.maxCurrentSet = 60.0f;

	/****1号电机初始化****/
	VESCmotor[1].instrinsic = VESC_U10;
	VESCmotor[1].enable = DISABLE;
	VESCmotor[1].begin = true;
	VESCmotor[1].mode = RPM;
	VESCmotor[1].valSet.current = 10;
	VESCmotor[1].valSet.speed = -200;
	VESCmotor[1].valSet.position = 0;
	VESCmotor[1].valSet.duty = 1;
	PID_Init(&VESCmotor[1].PIDx, 0.5, 0.2, 0, 1, VESCmotor[1].valSet.position);

	for (int i = 0; i < 4; i++)
	{
		VESCmotor[i].limit = VESClimit;
		VESCmotor[i].argum = VESCargum;
	}
}

/** 
  * @brief 位置清0
  */

void VESC_caculate(VESC_MOTOR *motor)
{
	if (motor->status.isSetZero)
	{
		motor->status.isSetZero = false;
		motor->valReal.position = 0;
	} /* 重置零点 */
}

/** 
  * @brief 速度模式--通过速度模式实现
  */

void VESC_RPM_mode_I(u8 id)
{
	utils_truncate_number_abs_s32((s32 *)&VESCmotor[id].valSet.speed, VESCmotor[id].instrinsic.MAX_RPM); //TODO:强制类型转换
	//1.VESC自身PID--直接计算法，对D单独低通滤波
	VESCmotor[id].s_pid.SetVal = VESCmotor[id].valSet.speed;
	VESC_PID_Operation(&VESCmotor[id].s_pid, VESCmotor[id].valSet.speed, VESCmotor[id].valReal.speed);
	VESCmotor[id].valSet.current = VESCmotor[id].s_pid.output * VESCmotor[id].instrinsic.MAX_CURRENT;
	//2.增量法
	//  {
	//    static s32 error1=0, error2=0, error3=0;
	//    static float Udlt;
	//    static float p_term, i_term, d_term;//便于调试
	//
	//    VESCmotor[id].PIDs.SetVal = VESCmotor[id].valSet.speed;
	//    VESCmotor[id].PIDs.CurVal = VESCmotor[id].valReal.speed;
	//
	//
	//      error1 = VESCmotor[id].PIDs.SetVal - VESCmotor[id].PIDs.CurVal;
	//      p_term = VESCmotor[id].PIDs.uKP_Coe * (error1 - error2);
	//      i_term = VESCmotor[id].PIDs.uKI_Coe * error1;
	//      d_term = VESCmotor[id].PIDs.uKD_Coe * (error1 + error3 - error2 * 2);
	//
	//
	//      Udlt =  p_term+ i_term + d_term;
	//      Udlt *=  VESCmotor[id].PIDs.uKS_Coe;
	//      error3=error2;
	//      error2=error1;
	//    VESCmotor[id].valSet.current += Udlt;
	//  }
	utils_truncate_number_abs((float *)&VESCmotor[id].valSet.current, VESCmotor[id].limit.maxCurrentSet); //TODO:强制类型转换
	VESC_Set_Current(id + 1, VESCmotor[id].valSet.current, 0);
}

/** 
  * @brief 位置模式--通过速度模式实现
  */

void VESC_position_mode_I(u8 id)
{
	static float i_term = 0;
	static float prv_angle_error = 0;
	float p_term;
	float d_term;

	// Compute parameters
	if (SIG(VESCmotor[id].argum.difPosition) == SIG(VESCmotor[id].valReal.speed))
		angle_gap = 170.f;
	else
		angle_gap = 90.f;
	if (ABS(VESCmotor[id].argum.difPosition) > (angle_gap * 10))
	{
		VESCmotor[id].valSet.angle = VESCmotor[id].valReal.angle + SIG(VESCmotor[id].argum.difPosition) * angle_gap;
	}
	else
	{
		float difpos = VESCmotor[id].argum.difPosition / 1e1f;
		VESCmotor[id].valSet.angle = VESCmotor[id].valReal.angle + difpos;
	}
	utils_norm_angle(&VESCmotor[id].valSet.angle);//确保在0-360之间
	float angle_error = utils_angle_difference(VESCmotor[id].valSet.angle, VESCmotor[id].valReal.angle);
	if (VESCmotor[id].Encoder_DIR_inverted)
		angle_error = -angle_error;

	p_term = angle_error * VESCmotor[id].p_pid.kp;
	i_term += angle_error * (VESCmotor[id].p_pid.ki * dt);

	static float dt_int = 0.0f;
	dt_int += dt;
	if (angle_error == prv_angle_error)//TODO:速度太低了它可能没有检测出来位置更新，也就是说前后误差不变。如果这样的话会丢失掉这段位置。
	                                         //所以在一个大的时间段内进行微分。
                                             //如果正常运行的话那么这个微分累计时间就是微分时间不起作用
	{
		d_term = 0.0f;
	}
	else
	{
		d_term = (angle_error - prv_angle_error) * (VESCmotor[id].p_pid.kd / dt_int);
		dt_int = 0.0f;
	}

	// Filter D
	static float d_filter = 0.0f;
	UTILS_LP_FAST(d_filter, d_term, VESCmotor[id].p_pid.kd_filter);
	d_term = d_filter;

	// I-term wind-up protection
	utils_truncate_number_abs(&p_term, 1.0f);
	utils_truncate_number_abs(&i_term, 1.0f - fabsf(p_term));

	// Store previous error
	prv_angle_error = angle_error;

	// Calculate output
	float output = p_term + i_term + d_term;
	utils_truncate_number(&output, -1.0f, 1.0f);

	VESCmotor[id].valSet.current = output * VESCmotor[id].instrinsic.MAX_CURRENT;
	PEAK(VESCmotor[id].valSet.current, VESCmotor[id].limit.maxCurrentSet);
	VESC_Set_Current(id + 1, VESCmotor[id].valSet.current, 0);
}

/** 
  * @brief 位置模式--通过速度模式实现
  */

void VESC_position_mode_rpm(u8 id)
{
	VESCmotor[id].PIDx.SetVal = VESCmotor[id].valSet.position;
	if (!VESCmotor[id].begin)
		VESCmotor[id].PIDx.SetVal = VESCmotor[id].argum.lockPosition;
	if (VESCmotor[id].limit.isPosLimitOn)
		PEAK(motor->PIDx.SetVal, VESCmotor[id].limit.maxPosition);
	VESCmotor[id].PIDx.CurVal = VESCmotor[id].valReal.position;
	PID_Operation(&VESCmotor[id].PIDx);
	if (motor->limit.isPosSPLimitOn)
		PEAK(VESCmotor[id].PIDx.Udlt, motor->limit.posSPlimit);
	VESC_Set_Speed(id + 1, VESCmotor[id].PIDx.Udlt * VESCmotor[id].instrinsic.POLE_PAIRS, 0);
}
/** 
  * @brief 位置模式--本杰明自带位置模式
  */

void VESC_position_mode_pos(u8 id)
{
	//反转最大允许15000转速
	if (SIG(VESCmotor[id].argum.difPosition) == SIG(VESCmotor[id].valReal.speed))//TODO:速度保护，正转角度越大，反转速度会越小
		angle_gap = 170.f;
	else
		angle_gap = 90.f;
	if (ABS(VESCmotor[id].argum.difPosition) > (angle_gap * 10))
	{
		VESCmotor[id].valSet.angle = VESCmotor[id].valReal.angle + SIG(VESCmotor[id].argum.difPosition) * angle_gap;
	}
	else
	{
		float difpos = VESCmotor[id].argum.difPosition / 1e1f;
		VESCmotor[id].valSet.angle = VESCmotor[id].valReal.angle + difpos;
	}
	utils_norm_angle(&VESCmotor[id].valSet.angle);
	VESC_Set_Pos(id + 1, VESCmotor[id].valSet.angle, 0);
}

/**
  * @brief 设定VESC角度
  */
void VESC_Set_Pos(u8 controller_ID, float pos, u8 InConGrpFlag)
{
	int32_t send_index = 0;

	if (Rear3 == VESC_Sendqueue.Front)
	{
		flag.VESCSendqueueFULL++;
		return;
	}
	else
	{
		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, (int32_t)(pos * 1e6f), &send_index);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_POS << 8);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	VESC_Sendqueue.Rear = Rear3;
	VESCmotor[controller_ID - 1].argum.timeout = 1;
	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
}

/** 
  * @brief 设定VESC占空比
  */
void VESC_Set_Duty_Cycle(u8 controller_ID, float duty_cycle, u8 InConGrpFlag)
{
	int32_t send_index = 0;
	if (Rear3 == VESC_Sendqueue.Front)
	{
		flag.VESCSendqueueFULL++;
		return;
	}
	else
	{
		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, (int32_t)(duty_cycle * 100000), &send_index);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_DUTY << 8);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	VESC_Sendqueue.Rear = Rear3;
	VESCmotor[controller_ID - 1].argum.timeout = 1;
	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
}
/** 
	* @brief 设定VESC转速
	*/
void VESC_Set_Speed(u8 controller_ID, s32 speed, u8 InConGrpFlag)
{
	int32_t send_index = 0;
	if (Rear3 == VESC_Sendqueue.Front)
	{
		flag.VESCSendqueueFULL++;
		return;
	}
	else
	{

		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, speed, &send_index);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_RPM << 8);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	VESC_Sendqueue.Rear = Rear3;
	VESCmotor[controller_ID - 1].argum.timeout = 1;
	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
}

/** 
	* @brief 设定VESC位置
	*/
void VESC_Set_Position(u8 controller_ID, s32 pos, u8 InConGrpFlag)
{
	int32_t send_index = 0;
	if (Rear3 == VESC_Sendqueue.Front)
	{
		flag.VESCSendqueueFULL++;
		return;
	}
	else
	{

		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, (pos * 1000000 / 5.0f), &send_index);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_POS << 8);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	VESC_Sendqueue.Rear = Rear3;
	VESCmotor[controller_ID - 1].argum.timeout = 1;
	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
}

/** 
	* @brief 设定VESC电流
	*/
void VESC_Set_Current(u8 controller_ID, float current, u8 InConGrpFlag)
{
	int32_t send_index = 0;
	//	if (Rear3 == VESC_Sendqueue.Front)
	//	{
	//		flag.VESCSendqueueFULL++;
	//		return;
	//	}
	//	else
	//	{

	//		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, (int32_t)(current * 1000), &send_index);
	//		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_CURRENT << 8);
	//		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
	//		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	//	}
	//	VESC_Sendqueue.Rear = Rear3;
	//	VESCmotor[controller_ID - 1].argum.timeout = 1;
	//	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
	static CanTxMsg VESC_tx_message;
	VESC_tx_message.ExtId = controller_ID | ((uint32_t)CAN_PACKET_SET_CURRENT << 8);
	VESC_tx_message.RTR = CAN_RTR_Data;
	VESC_tx_message.IDE = CAN_Id_Extended;
	VESC_tx_message.DLC = 8;
	buffer_append_int32(VESC_tx_message.Data, (int32_t)(current * 1e3f), &send_index);
	CAN_Transmit(CAN2, &VESC_tx_message);
}
/** 
	* @brief 设定VESC刹车电流
	*/
void VESC_Set_Brake_Current(u8 controller_ID, float brake_current, u8 InConGrpFlag)
{
	int32_t send_index = 0;
	if (Rear3 == VESC_Sendqueue.Front)
	{
		flag.VESCSendqueueFULL++;
		return;
	}
	else
	{

		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, (int32_t)(brake_current * 1000), &send_index);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE << 8);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	VESC_Sendqueue.Rear = Rear3;
	VESCmotor[controller_ID - 1].argum.timeout = 1;
	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
}
/** 
	* @brief 设定VESC锁电机电流
	*/
void VESC_Set_Handbrake_Current(u8 controller_ID, float handbrake_current, u8 InConGrpFlag)
{
	int32_t send_index = 0;
	if (Rear3 == VESC_Sendqueue.Front)
	{
		flag.VESCSendqueueFULL++;
		return;
	}
	else
	{
		buffer_append_int32(VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].Data, (int32_t)(handbrake_current * 1000), &send_index);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].ID = 0xf0000000 | controller_ID | ((uint32_t)CAN_PACKET_SET_CURRENT_HANDBRAKE << 8);
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].DLC = send_index;
		VESC_Sendqueue.Can_DataSend[VESC_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	VESC_Sendqueue.Rear = Rear3;
	VESCmotor[controller_ID - 1].argum.timeout = 1;
	VESCmotor[controller_ID - 1].argum.lastRxTim = OSTimeGet();
}
