#include "Action_Status.h"

u32 First_Goods_Height;
u32 Plat_Axis_Z;
u32 Plat_Axis_X;
u32 AxisZ_UpDown_Pluse;
u32 AxisZ_UpDownMore_Pluse;

TYPEDEF_BACKZERO XYZ_To_Zero=
{
	.XYZRET_Status_Change = 0,//XYZRET_Status_Change状态改变标志位
	.XYZ_Ret_Stage 				= 1,//XYZ_Ret_Stage阶段跳转
	.Over_Flag 						= 0,//Over_Flag复位结束，返回信息给上位机
	.Start_Flag 	 				= 0,//Start_Flag复位开始标志位
	.X_Return_Flag				= 0,//X_Return_Flag，X轴回弹完成标志位
	.Tray_Reset_Flag			=	0//托盘自动复位标志位
};

u8 Last_AxisY_Dir = 2;//保存上次托盘运动方向
void XZ_Run(u16 AXIS_X_Mm,u16 AXIS_Z_Mm);
void Tray_Put_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position);
void Tray_Get_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position);


/**
  * @brief  取箱
  * @param  目标 X轴坐标，Z轴坐标
  * @retval 无
  */
void Get_Box(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)//取箱或者到达
{
	if(Action.Get_Box_Start_Flag == 1)
	{
		if(Action.Get_Box_Stage == 1)//XZ运动阶段
		{
		
			XZ_Run(AXIS_X_Mm,AXIS_Z_Mm);
			if(Action.end_flag == 1)
			{
				if(Action.command == Arrive)//转工位指令为到达，不做托盘拿箱动作，直接跳转到运动结束阶段
				{
					Action.Get_Box_Stage = 6;
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//XZ运动动作结束，返回报文
				}
				else//XZ运动状态结束，进入下一个阶段
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Second_Stage,0);//XZ运动动作结束，返回报文
					Action.Get_Box_Stage++;
					delay_ms(ActionDelay);
				}
			}
		}
		else if(Action.Get_Box_Stage == 2)//机械手拿箱阶段
		{
			Tray_Get_Box(!(Action.command&0x01),AxisZ_UpDown_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				if(Action.tray_position == Keep)//托盘属性为无，则跳转到结束阶段
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//托盘拿箱结束返回报文
					Action.Get_Box_Stage = 5;
				}
				else//托盘拿箱动作结束，进入下一个阶段
				{
					/**************后期测试代码************/
						delay_ms(800);
						if(Length_High_LimCtl())
						{
//							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//托盘拿箱结束返回报文
							Beep(50,50,5);					
							Action.Get_Box_Stage = 5;
							goto STATUS5;
						}
						/**************后期测试代码************/
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//托盘拿箱结束返回报文
					Action.Get_Box_Stage++;
					delay_ms(ActionDelay);
				}
			}
		}
		else if(Action.Get_Box_Stage == 3)//XZ运动阶段
		{
			if(Action.tray_position == Left_Tray||Action.tray_position == Right_Tray)
			{
				XZ_Run(Action.PlatX_Axis,Action.PlatZ_Axis+AxisZ_PutBoxFloat_Disance);
				if(Action.end_flag == 1)//XZ运动结束进入下一个阶段
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fourth_Stage,0);//XZ运动动作结束，返回报文
					Action.Get_Box_Stage++;
					delay_ms(ActionDelay);
				}
			}
		}
		else if(Action.Get_Box_Stage == 4)//机械手放箱阶段
		{
			Tray_Put_Box(!(Action.tray_position>>1),AxisZ_UpDownMore_Pluse,Tray);
			if(Action.end_flag == 1)
			{
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//托盘放箱结束，返回报文
				Action.Get_Box_Stage+=2;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Get_Box_Stage == 5)//报警复位阶段
		{
			STATUS5:
			Tray_Put_Box(!(Action.command&0x01),AxisZ_UpDown_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				Action.Get_Box_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else//所有动作结束置标志位
		{
			Action_Stop();
			Action.Get_Box_Stage = 1;
			Action.Get_Box_Start_Flag = 0;
			Action.end_flag = 0;
			Action.Target_X = 0;
			Action.Target_Z = 0;
		}
	}
}
/**
  * @brief  放箱
  * @param  目标 X轴坐标，Z轴坐标
  * @retval 无
  */
void Put_Box(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)//放箱
{
	if(Action.Put_Box_Start_Flag == 1)
	{
		if(Action.Put_Box_Stage == 1)//XZ运动阶段，回到托架层
		{
			if(Action.tray_position == Keep)//托盘属性为无
			{
				Action.Put_Box_Stage = 3;
			}
			else//有托盘属性
			{
				XZ_Run(Action.PlatX_Axis,Action.PlatZ_Axis-AxisZ_Float_Disance);
			}
			if(Action.end_flag == 1)
			{
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Second_Stage,0);//XZ运动结束返回报文
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 2)//托盘取箱阶段
		{
			Tray_Get_Box(!(Action.tray_position>>1),AxisZ_UpDownMore_Pluse,Tray);
			if(Action.end_flag == 1)
			{
				/**************后期测试代码************/
						delay_ms(800);
						if(Length_High_LimCtl())
						{
//							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//托盘拿箱结束返回报文
							Beep(50,50,5);
							Action.Put_Box_Stage = 5;
							goto status5;
//							Action_Stop();
//							return ;
						}
				/**************后期测试代码************/
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//托盘取箱结束，返回报文
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 3)//XZ运动阶段，前往货架层坐标
		{
			XZ_Run(AXIS_X_Mm,AXIS_Z_Mm);
			if(Action.end_flag == 1)
			{
				if(Action.tray_position == Left_Tray||Action.tray_position == Right_Tray)
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fourth_Stage,0);//XZ运动结束，返回报文
				}
				else
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Second_Stage,0);//XZ运动结束，返回报文
				}
				
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 4)//托盘放箱阶段
		{

			Tray_Put_Box(!(Action.command&0x01),AxisZ_UpDown_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				if(Action.tray_position == Left_Tray||Action.tray_position == Right_Tray)//托盘有属性
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//托盘放箱结束，返回报文
				}
				else//托盘无属性
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//托盘放箱结束，返回报文
				}
				Action.Put_Box_Stage+=2;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 5)//如若发生长高宽限位报警则跳转至第五阶段，正常情况放箱不进入此阶段
		{
			status5:
			Tray_Put_Box(!(Action.tray_position>>1),AxisZ_UpDownMore_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else//所有动作结束，置标志位
		{
			Action_Stop();
			Action.Put_Box_Start_Flag = 0;
			Action.Put_Box_Stage = 1;
			Action.end_flag = 0;
			Action.Target_X = 0;
			Action.Target_Z = 0;
		}
	}
}

void XZ_Run(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)//XZ运动
{
	if(Action.XZRun_Stage == 1)//XZ运动阶段
	{
		if(Action.XZRUN_Status_Change == 1)
		{
			Action.end_flag = 0;
			Action.XZRUN_Status_Change = 0;
			Motor_CtrlOver_Flag = 1;
			Motor_MoveToPosition(AXIS_X_Mm,AXIS_Z_Mm);
			
		}
		if(Motor_CtrlOver_Flag == 2)
		{
			Action.XZRun_Stage++;
		}
	}
	else//运动结束置位标志位
	{
		Action.end_flag = 1;
		Action.XZRun_Stage = 1;
		Action.XZRUN_Status_Change = 1;	
	}
}

/**
  * @brief  托盘放箱动作，经历伸托盘，Z轴下降，缩托盘三个过程
  * @param  托盘伸展放向(左或右)，Z轴下降脉冲数,托盘当前位置
  * @retval 无
  */
void Tray_Put_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position)//机械手放箱
{
		if(Action.Tray_PutBox_Stage == 1)//伸托盘
		{
			if(Action.Tray_PutBox_Status_Change == 1)
			{
				Action.end_flag = 0;
				Action.Tray_PutBox_Status_Change = 0;
				if(Tray_Position == Tray)//当前托盘位于托架处
					{
						if((Action.tray_position == Left_Tray&&Action.command == Get_Left_Box)||
							 (Action.tray_position == Right_Tray&&Action.command == Get_Right_Box))//托盘属性与货物属性一致，正常走
								{
									if(Axis_Y_Dir == Y_DIR_LEFT)//左箱
									{
										if(Last_AxisY_Dir==Axis_Y_Dir)
										{
											Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
											StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);//宏定义
										}
										else
										{
											Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
											StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);//宏定义
										}
									}
										else//右箱
									{
										if(Last_AxisY_Dir==Axis_Y_Dir)
										{
											Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
											StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);//宏定义
										}
										else
										{
											Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
											StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);//宏定义
										}
									}
								}
								else//托盘属性与货物属性不一致，脉冲数多走
								{
									Last_AxisY_Dir=Axis_Y_Dir;
									StepMotorCtrl_Pulse(Y_LeftTrayPluse,AXIS_Y,Axis_Y_Dir);	
								}
					}
				else//当前托盘位于货架处
				{
					if(Axis_Y_Dir == Y_DIR_LEFT)//左箱
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)
						{
							Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
							StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);//宏定义
						}
						else
						{
							Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
							StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);//宏定义
						}
					}
					else//右箱
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)
						{
							Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
							StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);//宏定义
						}
						else
						{
							Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
							StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);//宏定义
						}
					}
				}
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Action.Tray_PutBox_Status_Change = 1;
				Action.Tray_PutBox_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_PutBox_Stage == 2)//Z轴下降
		{
			if(Action.Tray_PutBox_Status_Change == 1)
			{
				Action.Tray_PutBox_Status_Change = 0;
				StepMotorCtrl_Pulse(UpDown_Pluse,AXIS_Z,Z_DIR_BACK);
			}
			if(Axis_Z.InCtrl_Flag == 0)//运动结束
			{
				Action.Tray_PutBox_Status_Change = 1;
				Action.Tray_PutBox_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_PutBox_Stage == 3)//缩托盘
		{
			if(Action.Tray_PutBox_Status_Change == 1)
			{
				Action.Tray_PutBox_Status_Change = 0;
				Last_AxisY_Dir = !Axis_Y_Dir;
				StepMotorCtrl_Pulse(2500,AXIS_Y,!Axis_Y_Dir);
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{				
				Action.Tray_PutBox_Stage++;
			}
		}
		else//动作结束，置标志位
		{
			Action.Tray_PutBox_Status_Change = 1;
			Action.Tray_PutBox_Stage = 1;
			Action.end_flag=1;
		}
	}

	
/**
  * @brief  托盘取箱动作，经历伸托盘，Z轴下降，缩托盘三个过程
  * @param  托盘伸展放向(左或右)，Z轴下降脉冲数,托盘当前位置
  * @retval 无
  */
void Tray_Get_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position)//机械手拿箱
{
		if(Action.Tray_GetBox_Stage == 1)//伸托盘
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				Action.end_flag = 0;
				Action.Tray_GetBox_Status_Change = 0;
				if(Tray_Position == Tray)//当前托盘处于托架处
				{
					if((Action.tray_position == Left_Tray&&Action.command == Return_Left_Box)||
						(Action.tray_position == Right_Tray&&Action.command == Return_Right_Box))//托盘属性与货物属性一致，正常走
					{
						if(Axis_Y_Dir == Y_DIR_LEFT)//伸左箱
						{
							if(Last_AxisY_Dir==Axis_Y_Dir)//托盘与上次移动相同的方向
							{
								Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
								StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);
							}
							else//托盘与上次移动不同的方向
							{
								Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
								StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);
							}
						}
						else//伸右箱
						{
							if(Last_AxisY_Dir==Axis_Y_Dir)//与上次行走相同的方向
							{
								Last_AxisY_Dir=Axis_Y_Dir;
								StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);
							}
							else//与上次行走不同的方向
							{
								Last_AxisY_Dir=Axis_Y_Dir;
								StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);
							}
						}
					}
					else//托盘属性与货物属性方向不一致，脉冲数多走
					{
							Last_AxisY_Dir=Axis_Y_Dir;
							StepMotorCtrl_Pulse(Y_LeftTrayPluse,AXIS_Y,Axis_Y_Dir);						
					}
				}
				else//当前托盘处于货道
				{
					if(Axis_Y_Dir == Y_DIR_LEFT)//伸左箱
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)//托盘与上次移动相同的方向
						{
							Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
							StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);
						}
						else//托盘与上次移动不同的方向
						{
							Last_AxisY_Dir = Axis_Y_Dir;//记录本次托盘运动方向
							StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);
						}
					}
					else//伸右箱
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)//与上次行走相同的方向
						{
							Last_AxisY_Dir=Axis_Y_Dir;
							StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);
						}
						else//与上次行走不同的方向
						{
							Last_AxisY_Dir=Axis_Y_Dir;
							StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);
						}
					}
				}
			}
			if(Axis_Y.InCtrl_Flag == 0)//Y轴动作结束，阶段跳转
			{
				Action.Tray_GetBox_Stage++;
				Action.Tray_GetBox_Status_Change = 1;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_GetBox_Stage == 2)//Z轴上升
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				Action.Tray_GetBox_Status_Change = 0;
				StepMotorCtrl_Pulse(UpDown_Pluse,AXIS_Z,Z_DIR_FRONT);
			}
			if(Axis_Z.InCtrl_Flag == 0)
			{
				Action.Tray_GetBox_Stage++;
				Action.Tray_GetBox_Status_Change = 1;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_GetBox_Stage == 3)//缩托盘
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				Last_AxisY_Dir = !Axis_Y_Dir;//记录本次托盘运动方向
				StepMotorCtrl_Pulse(2500,AXIS_Y,!Axis_Y_Dir);
				Action.Tray_GetBox_Status_Change = 0;
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Action.Tray_GetBox_Status_Change = 1;
				Action.Tray_GetBox_Stage++;
			}
		}
		else//动作结束，置位标志位
		{
			Action.Tray_GetBox_Status_Change = 1;
			Action.Tray_GetBox_Stage = 1;
			Action.end_flag = 1;
		}
}

/**
  * @brief  工程师模式单轴调节
  * @param  目标控制轴，方向
  * @retval 无
  */
void Axis_Move_Alone(u8 Axis ,bool Dir)//工程师模式单点调节
{
	if(Axis == AXIS_X)
	{
//	if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)//动作前判断托盘是否位于零位，否则不执行动作
//		{
//			return ;
//		}
		Axis_X.Dir = Dir;
		DIR_X = Axis_X.Dir;
		Axis_X.NowPulse = 0;
		Axis_X.InCtrl_Flag = 1;
	}
	else if(Axis == AXIS_Y)
	{
			Axis_Y.Dir = Dir;
			DIR_Y = Axis_Y.Dir;
			Axis_Y.NowPulse = 0;
			Axis_Y.InCtrl_Flag = 1;
	}
	else
	{
//	if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)//动作前判断托盘是否位于零位，否则不执行动作
//		{
//			return ;
//		}
		Axis_Z.Dir = Dir;
		DIR_Z = Axis_Z.Dir;
		Axis_Z.NowPulse = 0;
		Axis_Z.InCtrl_Flag = 1;
	}
}

/**
  * @brief  立库复位状态机
  * @param  经历Z轴上升;Z轴下降，X轴回零;Y轴左复位;Y轴右复位.五个过程
  * @retval 无
  */
void XYZ_BackZero()
{
	if(XYZ_To_Zero.Start_Flag == 1)
	{
		if(XYZ_To_Zero.XYZ_Ret_Stage == 1)//自动复位
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
		{
				XYZ_To_Zero.XYZRET_Status_Change =1;
			if(A_SENSOR == 1&&B_SENSOR == 0)//托盘位于右边，往左复位
			{
				DIR_Y = Y_DIR_LEFT;
				Axis_Y.Dir = Y_DIR_LEFT;
				Axis_Y.Target_Pulse = 2500;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//归位模式
					XYZ_To_Zero.Tray_Reset_Flag = 1;
			}
			else if(B_SENSOR == 1&&A_SENSOR == 0)//托盘位于左边，往右复位
			{				
					DIR_Y = Y_DIR_RIGHT;
					Axis_Y.Dir = Y_DIR_RIGHT;
					Axis_Y.Target_Pulse = 2500;
					Axis_Y.NowPulse = 0;
					Axis_Y.InCtrl_Flag = 3;//归位模式
					XYZ_To_Zero.Tray_Reset_Flag = 1;
			}
			else//托盘位于正中间
			{
				
			}
		}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 2)//Z轴上升过程
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)//动作前判断托盘是否位于零位，否则不执行动作
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
					return ;
				}
				XYZ_To_Zero.XYZRET_Status_Change = 1;
				DIR_Z = Z_DIR_FRONT;
				Axis_Z.Dir = Z_DIR_FRONT;
				Axis_Z.Target_Pulse = Z_ZeroUp_Pluse;
				Axis_Z.NowPulse = 0;
				Axis_Z.InCtrl_Flag = 3;//归位模式
			}
			if(Axis_Z.InCtrl_Flag == 0)
			{
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 3)//Z轴下降，X轴回零过程
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				XYZ_To_Zero.XYZRET_Status_Change = 1;
				/*************Z轴下降归位********************/
				DIR_Z = Z_DIR_BACK;
				Axis_Z.Dir = Z_DIR_BACK;
				Axis_Z.NowPulse = 0;
				Axis_Z.Target_Pulse = 50000;
				Axis_Z.InCtrl_Flag = 3;//归位模式
				/*************X轴归位***********************/
				DIR_X = X_DIR_BACK;
				Axis_X.Dir = X_DIR_BACK;
				Axis_X.NowPulse = 0;
				Axis_X.Target_Pulse = 50000;
				Axis_X.InCtrl_Flag = 3;//归位模式
			}
			if(Axis_Z.InCtrl_Flag == 0&&Axis_X.InCtrl_Flag == 0&&XYZ_To_Zero.X_Return_Flag == 1)
			{
				XYZ_To_Zero.X_Return_Flag = 0;
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 4)//Y轴左复位过程，消除托盘机械误差
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				XYZ_To_Zero.XYZRET_Status_Change =1;
				DIR_Y = Y_DIR_LEFT;
				Axis_Y.Dir = Y_DIR_LEFT;
				Axis_Y.Target_Pulse = 500;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//归位模式
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 5)//Y轴右复位过程，消除托盘机械误差
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				XYZ_To_Zero.XYZRET_Status_Change =1;
				DIR_Y = Y_DIR_RIGHT;
				Axis_Y.Dir = Y_DIR_RIGHT;
				Axis_Y.Target_Pulse = 1000;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//归位模式
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Last_AxisY_Dir = Y_DIR_RIGHT;//记录托盘运动方向
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else//过程结束置位
		{
			XYZ_To_Zero.Over_Flag = 1;
			XYZ_To_Zero.XYZ_Ret_Stage = 1;
			XYZ_To_Zero.XYZRET_Status_Change = 0;
			XYZ_To_Zero.Start_Flag = 0;
			Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//XZ运动动作结束，返回报文
			Beep(50,50,1);
		}
	}
}

/**
  * @brief  取箱，放箱，到达分析
  * @param  1:函数指针指向取箱\放箱\到达，2:托盘属性，3:货物属性，4:X轴坐标，5:Z轴坐标
  * @retval 无
  */
void Get_PutBox_Analysis(pick_one open,u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
	open(tray_position,command_data,x_run,Z_Run);//选择所属的函数
}
/**
  * @brief  到达分析
  * @param  1:托盘属性，2:货物属性，3:X轴坐标，4:Z轴坐标
  * @retval 无
  */
void Arrive_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
	if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)//动作前判断托盘是否位于零位，否则不执行动作
	{
		return ;
	}
	Action.Comend  = CMD_CTRL_XZ;//指令集
	Action.command = Arrive;//转工位指令
	Action.Target_X = x_run;//目标X轴坐标
	Action.Target_Z = Z_Run;//目标
	Action.tray_position = Keep;//托盘属性
	Action.PlatX_Axis = 0;//托架X轴坐标
	Action.PlatZ_Axis = 240;//托架Z轴坐标
	Motor_CtrlOver_Flag = 1;
	Action.Get_Box_Start_Flag = 1;
	Action.Get_Box_Stage = 1;
	Action.end_flag = 0;
	Action.XZRun_Stage = 1;
	Action.XZRUN_Status_Change = 1;
}
/**
  * @brief  放箱分析
  * @param  1:托盘属性，2:货物属性，3:X轴坐标，4:Z轴坐标
  * @retval 无
  */
void Put_Box_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)//动作前判断托盘是否位于零位，否则不执行动作
	{
		return ;
	}
	Action.Comend  = CMD_CTRL_XZ;//指令集
	Action.command = command_data;//转工位指令
	Action.Target_X = x_run;//目标X轴坐标
	Action.Target_Z = Z_Run+AxisZ_Float_Disance;//目标
	Action.tray_position = tray_position;//托盘属性
	Action.PlatX_Axis = Plat_Axis_X;//托架X轴坐标
	Action.PlatZ_Axis = Plat_Axis_Z;//托架Z轴坐标
	Motor_CtrlOver_Flag = 1;
	Action.Put_Box_Start_Flag = 1;
	Action.Put_Box_Stage = 1;
	Action.XZRun_Stage = 1;
	Action.XZRUN_Status_Change = 1;
	Action.Tray_PutBox_Stage = 1;
	Action.Tray_PutBox_Status_Change = 1;
	Action.Tray_GetBox_Stage = 1;
	Action.Tray_GetBox_Status_Change = 1;
	Action.end_flag = 0;
}

/**
  * @brief  取箱分析
  * @param  1:托盘属性，2:货物属性，3:X轴坐标，4:Z轴坐标
  * @retval 无
  */
void Get_Box_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)//动作前判断托盘是否位于零位，否则不执行动作
	{
		return ;
	}
	Action.Comend  = CMD_CTRL_XZ;//指令集
	Action.command = command_data;//转工位指令
	Action.Target_X = x_run;//目标X轴坐标
	Action.Target_Z = Z_Run-AxisZ_Float_Disance;//目标
	Action.tray_position = tray_position;//托盘属性
	Action.PlatX_Axis = Plat_Axis_X;//托架X轴坐标
	Action.PlatZ_Axis = Plat_Axis_Z;//托架Z轴坐标
	Motor_CtrlOver_Flag = 1;
	Action.Get_Box_Start_Flag = 1;
	Action.Get_Box_Stage = 1;
	Action.XZRun_Stage = 1;
	Action.XZRUN_Status_Change = 1;
	Action.Tray_PutBox_Stage = 1;
	Action.Tray_PutBox_Status_Change = 1;
	Action.Tray_GetBox_Stage = 1;
	Action.Tray_GetBox_Status_Change = 1;
	Action.end_flag = 0;
}


/************************************************
* 函数名称：void Action_Stop()
* 功能描述：停止所有工作
* 返回结果：无
*************************************************/
void Action_Stop(void)
{
	Axis_X.InCtrl_Flag = 0;
	TIM_Cmd(TIM2, DISABLE);
	Axis_Y.InCtrl_Flag = 0;
	TIM_Cmd(TIM3, DISABLE);
	Axis_Z.InCtrl_Flag = 0;
	TIM_Cmd(TIM4, DISABLE);
	Action.Get_Box_Start_Flag = 0;
	Action.Put_Box_Start_Flag = 0;
	Action.Target_Z = 0;
	Action.Target_X = 0;
	Motor_CtrlOver_Flag = 1;
	Action.Get_Box_Stage = 1;
	Action.Put_Box_Stage = 1;
	Action.XZRun_Stage = 1;
	Action.XZRUN_Status_Change = 1;
	Action.Tray_PutBox_Stage = 1;
	Action.Tray_PutBox_Status_Change = 1;
	Action.Tray_GetBox_Stage = 1;
	Action.Tray_GetBox_Status_Change = 1;
	Action.end_flag = 0;
}


/**
  * @brief  长高限位判断
  * @param  
* @retval 1:触发限位报警不动作，0 :无触发
  */
bool Length_High_LimCtl(void)
{
	if(AB_HIGHTLENGTH_SENSOR == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
