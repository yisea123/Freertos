 
#include "Sys_Init.h"	
int main(void)
{
	/*******上电初始化*******/
	Sys_Init();		
	
	/***************************主循环****************************/
	while(1)
	{
		if(Reset_Flag ==1)//报警复位，清除标志位
		{
			PAout(5) = 0;
			Reset_Flag=0;
		}
		Uart_Analysis();//串口解析
		Axis_Ultra_Limit();//XZ轴坐标限位
		Tray_Limit_Contr();//托盘限位
		if(Timer_1ms_flag == 1)//1ms轮询，执行需要定时的功能   发送数据/模拟控制
		{
			Timer_1ms_flag = 0;
			//Return_SendMesge();//归位返回信息
			XYZ_BackZero();//三轴复位阶段轮询
			Key_Deal();//键盘扫描
			SURE_REC_UART2_Analyze();     //迪文屏串口解析
			Get_Box(Action.Target_X,Action.Target_Z);//取箱阶段轮询
			Put_Box(Action.Target_X,Action.Target_Z);//放箱阶段轮询
		}
	}
}


int fputc(int ch, FILE *f)
{
USART_SendData(USART3, (unsigned char) ch);
while (!(USART3->SR & USART_FLAG_TXE));

return (ch);

}

