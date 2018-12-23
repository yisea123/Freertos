
#include "Sys_Init.h"

void Sys_Init(void)
{		

	/************系统初始化*******************************************************/
	/*公用*/
	delay_init();	    		//延时函数初始化	  
	LED_Init();		  			//初始化与LED连接的硬件接口
	Beep_Init();					//蜂鸣器初始化
	KEY_Init();						//按键初始化
	GPIO_OutPut_Init();
	GPIO_InPut_Init();
	PWM_Init();
	IIC_Init();
	USART3_Init(115200);		//串口3初始化 波特率9600
	USART2_Init(115200);		//串口2迪文屏初始化
	NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM3_Int_Init(9,7199);//10Khz的计数频率，计数到1000为1ms 	
	IWDG_Init(6,3000);//20秒钟喂狗一次
	//WIFI_Init();
	Reset_Flag = 0;
	for(int i=0;i<20;i++)
	{
		for(int j =0;j<100;j++)
		{
			Uart3[j].Buffer[j] = 0;			
		}
		Uart3[i].Enter_mode = Rec_Data;
	}
	
  All_I2C_Read();
	
	Change_X_MPP.X_MPP_Data = Change_X_MPP.X_MPP_Data;
	Change_X_MPP.X_MPP_Data = Change_X_MPP.X_MPP_Data;
	
	Change_X_Zero_Pluse.X_Zero_Pluse_Data = Change_X_Zero_Pluse.X_Zero_Pluse_Data;
	Change_X_Zero_Pluse.X_Zero_Pluse_Data = Change_X_Zero_Pluse.X_Zero_Pluse_Data;
	
	Change_First_Goods_Height.First_Goods_Height_Data = Change_First_Goods_Height.First_Goods_Height_Data;
  Change_First_Goods_Height.First_Goods_Height_Data = Change_First_Goods_Height.First_Goods_Height_Data;	
	
	Change_Z_MPP.Z_MPP_Data = Change_Z_MPP.Z_MPP_Data;
	Change_Z_MPP.Z_MPP_Data = Change_Z_MPP.Z_MPP_Data;
	
	Change_Plat_Axis_Z.Plat_Axis_Z_Data = Change_Plat_Axis_Z.Plat_Axis_Z_Data;
	Change_Plat_Axis_Z.Plat_Axis_Z_Data = Change_Plat_Axis_Z.Plat_Axis_Z_Data;
	
	Change_Plat_Axis_X.Plat_Axis_X_Data = Change_Plat_Axis_X.Plat_Axis_X_Data;
	Change_Plat_Axis_X.Plat_Axis_X_Data = Change_Plat_Axis_X.Plat_Axis_X_Data;
	
	Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data = Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data;
	Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data = Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data;
	
	Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data = Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data;
	Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data = Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data;
	
	Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data = Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data;
	Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data = Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data;
	
	Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data = Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data;
	Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data = Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data;
	
	Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data = Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data;
	Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data = Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data;
	
	Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data = Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data;
	Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data = Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data;
	
	Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data = Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data;
	Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data = Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data;
	
	Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data = Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data;
	Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data = Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data;
	
	Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data = Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data;
	Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data = Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data;
	Beep(50,50,1);
	
}


