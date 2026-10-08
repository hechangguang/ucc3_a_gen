/*
 * uart_opr.c
 *
 *  Created on: 2023年3月30日
 *      Author: yan
 */
#include "uart_opr.h"

#include <stdint.h>
// #include "plic.h"
// #include "clint.h"
// #include "bsp.h"
// #include "riscv.h"
// #include "prescaler.h"
// #include "timer.h"

#define UART_A_SAMPLE_PER_BAUD 8
#define CORE_HZ BSP_CLINT_HZ

COM_data FPGAcom_data;


u8 CVBS_flag = 0;
BAUD baud;
u16 VerifyVal=0;

void trap_entry();
/***************************************定时器初始化************************************************/

#define TIMER_PRESCALER_CTRL            (SYSTEM_USER_TIMER_0_CTRL + 0x00)
#define TIMER_0_CTRL                    (SYSTEM_USER_TIMER_0_CTRL + 0x40)
#define TIMER_1_PRESCALER_CTRL            (SYSTEM_USER_TIMER_1_CTRL + 0x00)
#define TIMER_1_CTRL                    (SYSTEM_USER_TIMER_1_CTRL + 0x40)

#define TIMER_CONFIG_WITH_PRESCALER     0x2
#define TIMER_CONFIG_WITHOUT_PRESCALER  0x1
#define TIMER_CONFIG_SELF_RESTART       0x10000

#ifdef SIM
    #define TIMER_TICK_DELAY (BSP_CLINT_HZ/200) //Faster timer tick in simulation to avoid having to wait too long
#else
    #define TIMER_TICK_DELAY  (BSP_CLINT_HZ)
#endif

void initTimer(){

    //Divide clock rate by 99+1
    prescaler_setValue(TIMER_PRESCALER_CTRL,99);
    //Will tick each (99999+1)*(99+1) cycles (as it use the prescaler)
    timer_setConfig(TIMER_0_CTRL, TIMER_CONFIG_WITH_PRESCALER | TIMER_CONFIG_SELF_RESTART);
    timer_setLimit(TIMER_0_CTRL, 99999);

}

void Timer0init(){

    //configure timer
    initTimer();

    //configure PLIC
    plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, 0); //cpu 0 accept all interrupts with priority above 0

    //enable Timer 0 interrupts
   plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_SYSTEM_USER_TIMER_0_INTERRUPTS_0, 1);
   plic_set_priority(BSP_PLIC, SYSTEM_PLIC_SYSTEM_USER_TIMER_0_INTERRUPTS_0, 2);

    //enable interrupts
    csr_write(mtvec, trap_entry); //Set the machine trap vector (../common/trap.S)
    csr_set(mie, MIE_MEIE); //Enable machine timer and external interrupts
    csr_write(mstatus, MSTATUS_MPP | MSTATUS_MIE);
}

void initTimer1(){

    //Divide clock rate by 99+1
    prescaler_setValue(TIMER_1_PRESCALER_CTRL,99);
    //Will tick each (99999+1)*(99+1) cycles (as it use the prescaler)
    timer_setConfig(TIMER_1_CTRL, TIMER_CONFIG_WITH_PRESCALER | TIMER_CONFIG_SELF_RESTART);
    timer_setLimit(TIMER_1_CTRL, 999);

}

/***********************************************************************************************/


/***************************************串口初始化增加定时器中断****************************************/
void UART_Init(void)
{
	Vot_Tools_Init();
	Vot_Parameters_Init();
	uart0_init();
#ifdef SUPPORT_FPGA_USB_UART_PROTOCOL
	FPGACommand_Init();//FPGA_Uart
#endif
}


void uart0_init(){
    //UART init
    Uart_Config uartA;
    uartA.dataLength = BITS_8;
    uartA.parity = NONE;
    uartA.stop = ONE;
    switch(SystemParaMenu2.BaudLevel)
    {
    	default:baud = Baud_115200bps;break;
    	case 1:baud = Baud_9600bps;break;
    	case 2:baud = Baud_19200bps;break;
    	case 3:baud = Baud_38400bps;break;
    	case 4:baud = Baud_57600bps;break;
    	case 5:baud = Baud_115200bps;break;
    }
    uartA.clockDivider = CORE_HZ/(baud*UART_A_SAMPLE_PER_BAUD)-1;
    uart_applyConfig(BSP_UART_TERMINAL, &uartA);

    initTimer(); //usertimer0 Init
	initTimer1(); //usertimer1 Init

    // TX FIFO empty interrupt enable
   // uart_TX_emptyInterruptEna(BSP_UART_TERMINAL,1);

    // RX FIFO not empty interrupt enable
    uart_RX_NotemptyInterruptEna(BSP_UART_TERMINAL,1);

    //configure PLIC
    //cpu 0 accept all interrupts with priority above 0
    plic_set_threshold(BSP_PLIC, BSP_PLIC_CPU_0, 0);

    //enable Timer 0 interrupts
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_SYSTEM_USER_TIMER_0_INTERRUPTS_0, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_SYSTEM_USER_TIMER_0_INTERRUPTS_0, 2);

    //enable Timer 1 interrupts
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_SYSTEM_USER_TIMER_1_INTERRUPTS_0, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_SYSTEM_USER_TIMER_1_INTERRUPTS_0, 2);

    //enable SYSTEM_PLIC_USER_INTERRUPT_A_INTERRUPT rising edge interrupt
    plic_set_enable(BSP_PLIC, BSP_PLIC_CPU_0, SYSTEM_PLIC_SYSTEM_UART_0_IO_INTERRUPT, 1);
    plic_set_priority(BSP_PLIC, SYSTEM_PLIC_SYSTEM_UART_0_IO_INTERRUPT, 1);

    //enable interrupts
    csr_write(mtvec, trap_entry); //Set the machine trap vector (../common/trap.S)
    csr_set(mie, MIE_MEIE); //Enable external interrupts
    csr_write(mstatus, MSTATUS_MPP | MSTATUS_MIE);
}
/***********************************************************************************************/

void UartInterrupt_Sub()
{
    if (uart_status_read(BSP_UART_TERMINAL) & 0x00000100){

        bsp_printf("\nuart 0 tx fifo empty interrupt routine \r\n");
        // TX FIFO empty interrupt Disable
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) & 0xFFFFFFFE);
        // TX FIFO empty interrupt enable
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) | 0x01);
    }
    else if (uart_status_read(BSP_UART_TERMINAL) & 0x00000200){

    	Uart_Receive();
        // RX FIFO not empty interrupt Disable
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) & 0xFFFFFFFD);
        //Dummy Read Clear FIFO

//      uart_write(BSP_UART_TERMINAL, uart_read(BSP_UART_TERMINAL));

        // RX FIFO not empty interrupt enable
        uart_status_write(BSP_UART_TERMINAL,uart_status_read(BSP_UART_TERMINAL) | 0x02);
    }
}

void Uart_Receive(void)
{
	unsigned char dat;
	dat=uart_read(BSP_UART_TERMINAL);

	if(com_data.count >= DATA_MAX_NUM){memset(com_data.buff,0,sizeof(com_data.buff));com_data.count = 0;}
	com_data.start_flag = 1;
	com_data.buff[com_data.count] = dat;
//	bsp_printf("%c",com_data.buff[com_data.count]);
	com_data.count++;
	com_data.time = 0;
}

u8 IrayAgreement(void)
{
	u16 i,temp,head,num;
#ifdef	SUPPORT_MEASURE_UART_PROTOCOL

	for(i = 0;i < com_data.count;i++){//此循环只抓取单条指令
		if(com_data.buff[i] == Iray_command_head1){
			if((com_data.buff[com_data.buff[i+1]+2+i] == Iray_command_tail1)&&(com_data.buff[com_data.buff[i+1]+3+i] == Iray_command_tail2)){
				if(i != 0){
					num = com_data.buff[i+1]+4+i;
					for(head = 0,temp = i;temp< num;temp++,head++){com_data.buff[head] = com_data.buff[temp];}
				}
				UartDataHanding();//iray协议
				memset(com_data.buff,0,sizeof(com_data.buff));
				com_data.count = 0;
				return 1;
			}
			else return 0;
		}
		else return 0;
#endif

/***************测距******************/
#if SUPPORT_MEASURE_UART_PROTOCOL
		else if(com_data.buff[i] == Measure_command_head1){
			if((com_data.buff[i+1] == Measure_command_head2)&&(com_data.buff[i+2] < 0xFF)){
				if(i != 0){
					for(head = 0,temp = i;temp< i+8;temp++,head++){com_data.buff[head] = com_data.buff[temp];}
				}
				UartDistanceMeasurement();//测距模块
				memset(com_data.buff,0,sizeof(com_data.buff));
				com_data.count = 0;
				return 1;
			}
			else return 0;
		}
		else return 0;
	}
#endif
	return 0;
}
u8 TESTCC(void)
{
	u16 i,temp,head,num,count;
#ifdef TEST_CC
		else if(com_data.buff[i] == 0xBB)
		{
			u8  RangeLimit,lever_Mid;
			send_data.rx_buff[0] = 0xBB;

			if(com_data.buff[i+1] == 0x01)
			{
				RangeLimit = com_data.buff[i+2];
				APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0xff00ffff)|(RangeLimit<<16)));
				RangeLimit = (APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)>>16)&0xFF;
				send_data.rx_buff[1] = 0x01;
				send_data.rx_buff[2] = RangeLimit;
				for(int i = 0;i<2;i++)
				{
					 bsp_printf("%c",send_data.rx_buff[i]);
				}
				bsp_printf("\n");
				memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
				memset(com_data.buff,0,sizeof(com_data.buff));
				com_data.count = 0;
				return 1;
			}
			else if(com_data.buff[i+1] == 0x02)
			{
				lever_Mid = com_data.buff[i+2];
				APB3_REGW(IO_APB_SLAVE_0_INPUT_ALPHA,0,((APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)&0x00ffffff)|(lever_Mid<<24)));
				lever_Mid =( APB3_REGR(IO_APB_SLAVE_0_INPUT_ALPHA,0)>>24)&0xFF;
				send_data.rx_buff[1] = 0x02;
				send_data.rx_buff[2] = lever_Mid;
				for(int i = 0;i<2;i++)
				{
					 bsp_printf("%c",send_data.rx_buff[i]);
				}
				bsp_printf("\n");
				memset(send_data.rx_buff, 0, sizeof (send_data.rx_buff));
				memset(com_data.buff,0,sizeof(com_data.buff));
				com_data.count = 0;
				return 1;
			}
			else
			{
				return 0;
			}
		}
#else
		return 0;
#endif
}

#if defined SUPPORT_HuanYu_UART_PROTOCOL || SUPPORT_ZhongLin_UART_PROTOCOL
void UartTx(u8*ptr,u32 len)
{
	for(int i=0;i<len;i++)
	{
		bsp_printf("%c",*ptr++);
	}
}
void ClearUartData(void)
{
   memset(com_data.buff,0,sizeof(com_data.buff));
   com_data.count = 0;
}

void FirmwareTransferInit(void)
{
   systemPar.FirmwareUpdate = 0;
   systemPar.PtrUpdateDDR = (u8*)Update_Firmware_BASE_ADDR;
   systemPar.FirmwareUpdateStatus =0;
   systemPar.FirmwareUpdateLen =0;
   systemPar.FirmwareLenCount = 0;
   systemPar.FirmwareOverTime = 0;
   VotTools.BIGDATA_FALG = 0;
   systemPar.FirmwareUpdateFlag = 0;
}
#endif

#ifdef SUPPORT_ZhongLin_UART_PROTOCOL

u8 calculateXOR(u8* data, u32 length) {
    uint8_t xorValue = 0;
    for(uint8_t i = 0; i < length; i++) {
        xorValue ^= data[i];
    }
    return xorValue;
}
u8 compare_arrays(u8 arr1[], u8 arr2[], u32 size) {
    for (int i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return 0; // 不相同
        }
    }
    return 1; // 相同
}
u8 CalcChecksum8(u8 *pBuf, u32 nLength)
{
	u8 checksum = 0;
    while (nLength--)
    {
        checksum += *pBuf++;
    }
    return checksum;
}
u32 CalcChecksum32(u8 *pBuf, u32 nLength)
{
	u32 checksum = 0;
	s32 i=0;
	u16 data = 0;
//	bsp_printf("nLength %x\n",nLength);
    for(i=nLength;i>0;i-=2)
    {

    	data = 0;
    	if(i<2)
    	{
    		data = *pBuf;
    	}
    	else
    	{
    		data = *(pBuf+1)<<8|*pBuf;
    	}
        checksum += data;
//        bsp_printf("i %x,data %x,checksum:%x\n",i,data,checksum);
        pBuf+=2;
    }
    return checksum;
}

//type 1-SOC  2-FPGA
void Write_Flash_UpdataFirmware(u8 type,u32 Len)
{
	uint16_t i,j,len;
	uint8_t * prt1;
	uint32_t  N25Q032A_address;
	uint8_t flash_wr_buf256[256];

	if(type!=1&&type!=2) return;
	DisplayBackground(32,400,11,BACKGROUND_LAYER,3);

	switch(type)
	{
		case 1:
			APB3_REGW_SPI_CS(0);
			DisPlayNewStr(32,400,"Saving-SOC",4,6);
#ifdef SUPPORT_ZhongLin_Multi_Firmware
			prt1 = (u8*)Update_Firmware_BASE_ADDR + 11+16;
#else
			prt1 = (u8*)Update_Firmware_BASE_ADDR + 11;
#endif
			break;
		case 2:
			APB3_REGW_SPI_CS(1);
			DisPlayNewStr(32,400,"Saving-FPGA",4,6);
#ifdef SUPPORT_ZhongLin_Multi_Firmware
			prt1 = (u8*)Update_Firmware_BASE_ADDR + 4+16;
#else
			prt1 = (u8*)Update_Firmware_BASE_ADDR + 4;
#endif
			break;
	}
	delay_1ms(50);
	N25Q032A_address = N25Q032A_Update_Firmware_addr;
#ifdef SUPPORT_ZhongLin_Multi_Firmware
	Len -=16;
#endif
	len = Len / (256 * 256);

	GlobalUnlock();
	for(i = 0; i < len; i++)
	{
		BlockErase(N25Q032A_address);
		for(j = 0; j < 256; j++)
		{
			memcpy(flash_wr_buf256,prt1,256);
			write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
			prt1 += 256;
			N25Q032A_address += 256;
		}
	}
	BlockErase(N25Q032A_address);
	len = Len % (256 * 256);
	len = len / 256;
	for(i = 0; i < len; i++)
	{
		memcpy(flash_wr_buf256,prt1,256);
		write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
		prt1 += 256;
		N25Q032A_address += 256;
	}
	len = Len % (256 * 256);
	len = len % 256;
	memcpy(flash_wr_buf256,prt1,len);
	write_flash_page(N25Q032A_address,flash_wr_buf256, len);
    GlobalLock();

    APB3_REGW_SPI_CS(0);
    delay_1ms(50);
	DisplayBackground(32,400,11,BACKGROUND_LAYER,3);
	switch(type)
	{
		case 1:
			DisPlayNewStr(32,400,"Done-SOC",4,6);
			break;
		case 2:
			DisPlayNewStr(32,400,"Done-FPGA",4,6);
			break;
	}
	delay_1s(1);
	DisplayBackground(32,400,11,BACKGROUND_LAYER,0);
}

s32 ContainsHead1(u8 arr[], int size) {
    for(int i = 0; i < size - 1; i++) {
        if(arr[i] == 0x55 && arr[i + 1] == 0xAA) {
            return i;
        }
    }
    return -1;
}
s32 ContainsHead2(u8 arr[], int size) {
    for(int i = 0; i < size - 1; i++) {
        if(arr[i] == 0xFF && arr[i + 1] == 0x10  && arr[i + 2] == 0x01 && arr[i + 3] == 0x01
        && arr[i + 4] == 0x01 && arr[i + 5] == 0x03  && arr[i + 6] == 0x16 )
        {
            return i;
        }
    }
    return -1;
}

//type 1-FPGA_RTD6 2-FPGA_PLA6     FPGA固件
//     3-SOC_RTD6  4-SOC_PLA6      SOC固件
s32 ContainsFPGA_SOC(u8 arr[], int size,u8 type) {
	for(int i = 0; i < size - 1; i++)
	{
		switch(type)
		{
			case 1:
				if(arr[i  ] == 0x46 && arr[i+1] == 0x50 && arr[i+2] == 0x47 && arr[i+3] == 0x41
				&& arr[i+4] == 0x52 && arr[i+5] == 0x54 && arr[i+6] == 0x44 && arr[i+7] == 0x36 )
				{
					return 1;
				}
				break;
			case 2:
				if(arr[i  ] == 0x46 && arr[i+1] == 0x50 && arr[i+2] == 0x47 && arr[i+3] == 0x41
				&& arr[i+4] == 0x50 && arr[i+5] == 0x4C && arr[i+6] == 0x41 && arr[i+7] == 0x36 )
				{
					return 2;
				}
				break;
			case 3:
				if(arr[i  ] == 0x53 && arr[i+1] == 0x4F && arr[i+2] == 0x43 && arr[i+3] == 0x00
				&& arr[i+4] == 0x52 && arr[i+5] == 0x54 && arr[i+6] == 0x44 && arr[i+7] == 0x36 )
				{
					return 3;
				}
				break;
			case 4:
				if(arr[i  ] == 0x53 && arr[i+1] == 0x4F && arr[i+2] == 0x43 && arr[i+3] == 0x00
				&& arr[i+4] == 0x50 && arr[i+5] == 0x4C && arr[i+6] == 0x41 && arr[i+7] == 0x36 )
				{
					return 4;
				}
				break;
		}
	}
	return -1;
}

void TimeOutORR(void) //接收超时操作
{
	u8 ReturnMegTest[15] ={0xFF,0x10,0x01,0x01,0x04,0xA4,0xBA};//最后一个校验位  8位和校验
	u16 FirmwareOverTime = 3000;
	if(systemPar.FirmwareOverTime>FirmwareOverTime && systemPar.FirmwareUpdateStatus)
	{
/*		ReturnMeg[5]=0xA4;
		ReturnMeg[6]=CalcChecksum8(ReturnMeg+1,5);
		UartTx(ReturnMeg,sizeof(ReturnMeg));*/
	   ReturnMegTest[7]=systemPar.FirmwareLenCount;
	   ReturnMegTest[8]=systemPar.FirmwareLenCount>>8;
	   ReturnMegTest[9]=systemPar.FirmwareLenCount>>16;
	   ReturnMegTest[10]=systemPar.FirmwareLenCount>>24;

	   ReturnMegTest[11]=systemPar.FirmwareUpdateLen;
	   ReturnMegTest[12]=systemPar.FirmwareUpdateLen>>8;
	   ReturnMegTest[13]=systemPar.FirmwareUpdateLen>>16;
	   ReturnMegTest[14]=systemPar.FirmwareUpdateLen>>24;
	   UartTx(ReturnMegTest,sizeof(ReturnMegTest));
	   FirmwareTransferInit();
	}
}

u8 CustomusizedOPR(void)
{
	u8* ReceivePtr,*ptr;
	//旧帧头
//	u8 StartMeg[10]    ={0x55,0xAA,0x05,0xA1,0x00,0x0D,0x00,0x10,0xB9,0xF0}; //倒数第2校验位 异或校验  最后帧尾
//	u8 StartMegFPGA[10]={0x55,0xAA,0x05,0xA1,0x00,0x0D,0x00,0x0D,0xA4,0xF0}; //倒数第2校验位 异或校验  最后帧尾
	//新帧头
	u8 StartMeg[10]    ={0x55,0xAA,0x05,0xA1,0x00,0x0D,0x00,0x11,0xB8,0xF0}; //倒数第2校验位 异或校验  最后帧尾
	u8 StartMegFPGA[10]={0x55,0xAA,0x05,0xA1,0x00,0x0D,0x00,0x0E,0xA7,0xF0}; //倒数第2校验位 异或校验  最后帧尾

	u8 ReturnMeg[7]    ={0xFF,0x10,0x01,0x01,0x04,0xA3,0xB9};//最后一个校验位  8位和校验
	u8 ReturnMegFPGA[6]={0x55,0xAA,0x01,0x00,0x01,0xF0};//最后一个校验位  8位和校验

	u8 SocDataHead[7]={0xFF,0x10,0x01,0x01,0x01,0x03,0x16};
	u8 ReturnMegTest[15] ={0xFF,0x10,0x01,0x01,0x04,0xA4,0xBA};//最后一个校验位  8位和校验
	u32 sum32 = 0,sum32cal = 0;
	s32 pos = 0;

	StartMeg[8] = calculateXOR(StartMeg+2,*(StartMeg+2)+1);
	ReturnMeg[5]=0xA3;
	ReturnMeg[6]=CalcChecksum8(ReturnMeg+1,5);

	ReceivePtr = com_data.buff;
	ptr = (u8*)Update_Firmware_BASE_ADDR;

	if(!com_data.count)  return 1;
	switch(systemPar.FirmwareUpdateStatus)
	{
		case 0:
					pos = ContainsHead1(ReceivePtr,com_data.count);
					if(compare_arrays(ReceivePtr+pos,StartMeg,sizeof(StartMeg)) && pos>=0)
					{
					   UartTx(ReturnMeg,sizeof(ReturnMeg));
					   ClearUartData();
					   FirmwareTransferInit();
					   systemPar.FirmwareUpdate = 1;
					   systemPar.FirmwareUpdateStatus =1;
					   VotTools.BIGDATA_FALG = 1;
					   systemPar.FirmwareUpdateFlag =1;
					   return 1;
					}
					else if(compare_arrays(ReceivePtr+pos,StartMegFPGA,sizeof(StartMegFPGA)) && pos>=0)
					{
					   UartTx(ReturnMegFPGA,sizeof(ReturnMegFPGA));
					   ClearUartData();
					   FirmwareTransferInit();
					   systemPar.FirmwareUpdate = 2;
					   systemPar.FirmwareUpdateStatus =3;
					   VotTools.BIGDATA_FALG = 1;
					   systemPar.FirmwareUpdateFlag =1;
					   return 1;
					}
				break;
		case 1: //接受第一组数据包，获取固件大小
				ReceivePtr = com_data.buff;
				pos = ContainsHead2(ReceivePtr,com_data.count);
				ReceivePtr += pos;
				if(compare_arrays(ReceivePtr,SocDataHead,sizeof(SocDataHead)) && pos>=0)
				{
#ifdef VOT
					if(ContainsFPGA_SOC(ReceivePtr+11,com_data.count,4)!=4) //固件不匹配
					{
						UartTx(ReturnMegTest,7);
						ClearUartData();
						FirmwareTransferInit();
						return 0;
					}
#else
					if(ContainsFPGA_SOC(ReceivePtr+11,com_data.count,3)!=3)//固件不匹配
					{
						UartTx(ReturnMegTest,7);
						ClearUartData();
						FirmwareTransferInit();
						return 0;
					}
#endif

				   systemPar.FirmwareOverTime = 0;
				   memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count-pos);
				   systemPar.FirmwareUpdateLen  = (*(systemPar.PtrUpdateDDR+10) << 24) | (*(systemPar.PtrUpdateDDR+9) << 16) |
												  (*(systemPar.PtrUpdateDDR+8) << 8) | *(systemPar.PtrUpdateDDR+7);
				   systemPar.PtrUpdateDDR+=com_data.count;

				   systemPar.FirmwareLenCount +=com_data.count-11;
				   systemPar.FirmwareUpdateStatus =2;
				   ClearUartData();
				   return 1;
				}
			   break;
		case 2:
			   systemPar.FirmwareOverTime = 0;
			   memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count);
			   systemPar.PtrUpdateDDR+=com_data.count;
			   systemPar.FirmwareLenCount +=com_data.count;

			   if(systemPar.FirmwareUpdateLen+4==systemPar.FirmwareLenCount)//传输完成
			   {
				   sum32  = (*(systemPar.PtrUpdateDDR-1) << 24) | (*(systemPar.PtrUpdateDDR-2) << 16) |
							(*(systemPar.PtrUpdateDDR-3) <<  8) | *(systemPar.PtrUpdateDDR-4);
				   systemPar.PtrUpdateDDR = (u8*)Update_Firmware_BASE_ADDR+11;
				   sum32cal = CalcChecksum32(systemPar.PtrUpdateDDR,systemPar.FirmwareUpdateLen);
				   if(sum32 == sum32cal)
				   {
					   //校验成功
						ReturnMeg[5]=0xA6;
						ReturnMeg[6]=CalcChecksum8(ReturnMeg+1,5);
						UartTx(ReturnMeg,sizeof(ReturnMeg));
        				Write_Flash_UpdataFirmware(systemPar.FirmwareUpdate,systemPar.FirmwareUpdateLen); //写固件
						ReturnMeg[5]=0xA7;
						ReturnMeg[6]=CalcChecksum8(ReturnMeg+1,5);
						UartTx(ReturnMeg,sizeof(ReturnMeg));
						FirmwareTransferInit();
				   }
				   else
				   {
					   ReturnMegTest[7]=sum32cal;
					   ReturnMegTest[8]=sum32cal>>8;
					   ReturnMegTest[9]=sum32cal>>16;
					   ReturnMegTest[10]=sum32cal>>24;

					   ReturnMegTest[11]=sum32;
					   ReturnMegTest[12]=sum32>>8;
					   ReturnMegTest[13]=sum32>>16;
					   ReturnMegTest[14]=sum32>>24;
					   UartTx(ReturnMegTest,sizeof(ReturnMegTest));
					   FirmwareTransferInit();
				   }
			   }
			   ClearUartData();
			   return 1;
			   break;

		case 3:
				ReceivePtr = com_data.buff;
			    memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count);

#ifdef VOT
				if(ContainsFPGA_SOC(ReceivePtr+4,com_data.count,2)!=2)//固件不匹配
				{
					UartTx(ReturnMegTest,7);
					ClearUartData();
					FirmwareTransferInit();
					return 0;
				}
#else
				if(ContainsFPGA_SOC(ReceivePtr+4,com_data.count,1)!=1)//固件不匹配
				{
					UartTx(ReturnMegTest,7);
					ClearUartData();
					FirmwareTransferInit();
					return 0;
				}
#endif

			   systemPar.FirmwareUpdateLen  = (*(systemPar.PtrUpdateDDR+3) << 24) | (*(systemPar.PtrUpdateDDR+2) << 16) |
											  (*(systemPar.PtrUpdateDDR+1) << 8) | *(systemPar.PtrUpdateDDR+0);
			   systemPar.PtrUpdateDDR+=com_data.count;
			   systemPar.FirmwareLenCount +=com_data.count-4;
			   systemPar.FirmwareUpdateStatus =4;
			   systemPar.FirmwareOverTime = 0;
			   ClearUartData();
			   return 1;
			   break;

		case 4:
			   memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count);
			   systemPar.PtrUpdateDDR+=com_data.count;
			   systemPar.FirmwareLenCount +=com_data.count;
			   systemPar.FirmwareOverTime = 0;
			   if(systemPar.FirmwareUpdateLen+4==systemPar.FirmwareLenCount)//传输完成
			   {
				   sum32  = (*(systemPar.PtrUpdateDDR-1) << 24) | (*(systemPar.PtrUpdateDDR-2) << 16) |
							(*(systemPar.PtrUpdateDDR-3) <<  8) | *(systemPar.PtrUpdateDDR-4);
				   systemPar.PtrUpdateDDR = (u8*)Update_Firmware_BASE_ADDR+4;
				   sum32cal = CalcChecksum32(systemPar.PtrUpdateDDR,systemPar.FirmwareUpdateLen);
				   ReturnMegFPGA[3]=0x00;
				   ReturnMegFPGA[4]=CalcChecksum8(ReturnMegFPGA+2,2);
				   UartTx(ReturnMegFPGA,sizeof(ReturnMegFPGA));

				   if(sum32 == sum32cal)
				   {
					   //校验成功
					   ReturnMegFPGA[3]=0xA2;
					   ReturnMegFPGA[4]=CalcChecksum8(ReturnMegFPGA+2,2);
					   UartTx(ReturnMegFPGA,sizeof(ReturnMegFPGA));
				       Write_Flash_UpdataFirmware(systemPar.FirmwareUpdate,systemPar.FirmwareUpdateLen); //写固件
						ReturnMegFPGA[3]=0x34;
						ReturnMegFPGA[4]=CalcChecksum8(ReturnMegFPGA+2,2);
						UartTx(ReturnMegFPGA,sizeof(ReturnMegFPGA));
						FirmwareTransferInit();
				   }
				   else
				   {
						ReturnMeg[5]=0xA4;
						ReturnMeg[6]=CalcChecksum8(ReturnMeg+1,5);
						UartTx(ReturnMeg,sizeof(ReturnMeg));
						FirmwareTransferInit();
				   }
			   }
			   ClearUartData();
			   return 1;
				break;
	}
 	 if(systemPar.FirmwareUpdate)  return 1;
 	 return 0;
}

#endif

//*******************************************************寰宇相关协议内容****************************************//
#ifdef SUPPORT_HuanYu_UART_PROTOCOL
u8 CalcChecksum8(u8 *pBuf, u32 nLength)
{
	u8 checksum = 0;
    while (nLength--)
    {
        checksum += *pBuf++;
    }
    return checksum;
}
u32 CalcChecksum32(u8 *pBuf, u32 nLength)
{
	u32 checksum = 0;
    while (nLength--)
    {
        checksum += *pBuf++;
    }
    return checksum;
}
s32 ContainsHeadSoc1(u8 arr[], int size) {
    for(int i = 0; i < size - 1; i++) {
        if(arr[i] == 0xF1 && arr[i + 1] == 0x01&& arr[i + 2] == 0x02 && arr[i + 3] == 0x03
                && arr[i + 4] == 0x04 && arr[i + 5] == 0x05  && arr[i + 6] == 0x01) {
            return i;
        }
    }
    return -1;
}
s32 ContainsHeadSoc2(u8 arr[], int size) {
    for(int i = 0; i < size - 1; i++) {
        if(arr[i] == 0xF1 && arr[i + 1] == 0x01&& arr[i + 2] == 0x02 && arr[i + 3] == 0x03
                && arr[i + 4] == 0x04 && arr[i + 5] == 0x05  && arr[i + 6] == 0x02) {
            return i;
        }
    }
    return -1;
}
s32 ContainsHeadFpga1(u8 arr[], int size) {
    for(int i = 0; i < size - 1; i++) {
    	 if(arr[i] == 0xF1 && arr[i + 1] == 0x01&& arr[i + 2] == 0x02 && arr[i + 3] == 0x03
    	           && arr[i + 4] == 0x04 && arr[i + 5] == 0x05  && arr[i + 6] == 0x03) {
    	            return i;
        }
    }
    return -1;
}
s32 ContainsHeadFpga2(u8 arr[], int size) {
    for(int i = 0; i < size - 1; i++) {
    	 if(arr[i] == 0xF1 && arr[i + 1] == 0x01&& arr[i + 2] == 0x02 && arr[i + 3] == 0x03
    	           && arr[i + 4] == 0x04 && arr[i + 5] == 0x05  && arr[i + 6] == 0x04) {
    	            return i;
        }
    }
    return -1;
}
void Write_Flash_UpdataFirmware_HuanYu(u8 type,u32 Len)
{
	uint16_t i,j,len;
	u8 count;
	uint8_t * prt1;
	uint32_t  N25Q032A_address;
	uint8_t flash_wr_buf256[256];
	u8 ReturnEraseMeg1[9]    ={0xF2,0x01,0x02,0x03,0x04,0x01,0x00,0x01,0xFF};
	u8 ReturnEraseMeg2[9]    ={0xF2,0x01,0x02,0x03,0x04,0x01,0x64,0x65,0xFF};
	u8 ReturnWriteMeg1[9]    ={0xF2,0x01,0x02,0x03,0x04,0x02,0x00,0x02,0xFF};
	u8 ReturnWriteMeg2[9]    ={0xF2,0x01,0x02,0x03,0x04,0x03,0x64,0x67,0xFF};

	if(type!=1&&type!=2) return;
	DisplayBackground(32,400,11,BACKGROUND_LAYER,3);
	prt1 = (u8*)Update_Firmware_BASE_ADDR;
	switch(type)
	{
		case 1:
			APB3_REGW_SPI_CS(0);
			DisPlayNewStr(32,400,"Saving-SOC",4,6);
			break;
		case 2:
			APB3_REGW_SPI_CS(1);
			DisPlayNewStr(32,400,"Saving-FPGA",4,6);
			break;
	}
	N25Q032A_address = N25Q032A_Update_Firmware_addr;
//	Len= 640*512*2; /////测试
	len = Len / (256 * 256);

	//********************擦除*********************//
	count = 100/len;
	UartTx(ReturnEraseMeg1,sizeof(ReturnEraseMeg1));
	delay_1ms(10);
	for(u8 m=1;m<count;m++)
	{
		ReturnEraseMeg1[6]= len* m;
		ReturnEraseMeg1[7] = ReturnEraseMeg1[6]+ReturnEraseMeg1[5];
		UartTx(ReturnEraseMeg1,sizeof(ReturnEraseMeg1));
		delay_1ms(10);
	}
	delay_1ms(10);
	UartTx(ReturnEraseMeg2,sizeof(ReturnEraseMeg2));//100%进度

	GlobalUnlock();
	for(i = 0; i < len; i++)
	{
		//********************写入*********************//
		ReturnWriteMeg1[6] = count*i;
		ReturnWriteMeg1[7] = ReturnWriteMeg1[6]+ReturnWriteMeg1[5];
		UartTx(ReturnWriteMeg1,sizeof(ReturnWriteMeg1));

		BlockErase(N25Q032A_address);
		for(j = 0; j < 256; j++)
		{
			memcpy(flash_wr_buf256,prt1,256);
			write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
			prt1 += 256;
			N25Q032A_address += 256;
		}
	}
	BlockErase(N25Q032A_address);
	len = Len % (256 * 256);
	len = len / 256;
	for(i = 0; i < len; i++)
	{
		memcpy(flash_wr_buf256,prt1,256);
		write_flash_page(N25Q032A_address,flash_wr_buf256, 256);
		prt1 += 256;
		N25Q032A_address += 256;
	}
	len = Len % (256 * 256);
	len = len % 256;
	memcpy(flash_wr_buf256,prt1,len);
	write_flash_page(N25Q032A_address,flash_wr_buf256, len);
    GlobalLock();
    UartTx(ReturnWriteMeg2,sizeof(ReturnWriteMeg2)); //100%进度

    APB3_REGW_SPI_CS(0);
	DisplayBackground(32,400,11,BACKGROUND_LAYER,3);
	switch(type)
	{
		case 1:
			DisPlayNewStr(32,400,"Done-SOC",4,6);
			break;
		case 2:
			DisPlayNewStr(32,400,"Done-FPGA",4,6);
			break;
	}
	delay_1s(1);
	DisplayBackground(32,400,11,BACKGROUND_LAYER,0);
}

u8 HuanYuCustomusizedOPR()
{
	u8* ReceivePtr,*ptr;
	u8 sum8 = 0;
	u8 StartMeg[10]    ={0xF1,0x01,0x02,0x03,0x04,0x05,0x01,0x10,0xB9,0xF0}; //倒数第2校验位 异或校验  最后帧尾
	u8 ReturnMeg[9]    ={0xF2,0x01,0x02,0x03,0x04,0x04,0x00,0x04,0xFF};//最后一个校验位  8位和校验

	u8 SocDataHead[7]={0xFF,0x10,0x01,0x01,0x01,0x03,0x16};
	u8 ReturnMegTest[15] ={0xFF,0x10,0x01,0x01,0x04,0xA4,0xBA};//最后一个校验位  8位和校验
	u32 sum32 = 0,sum32cal = 0;
	s32 pos = 0;
	s32 pos2 = 0;

	ReceivePtr = com_data.buff;
	ptr = (u8*)Update_Firmware_BASE_ADDR;

	if(!com_data.count)  return 1;
	switch(systemPar.FirmwareUpdateStatus)
	{
		case 0:
			pos = ContainsHeadSoc1(ReceivePtr,com_data.count);
			pos2 = ContainsHeadFpga1(ReceivePtr,com_data.count);
			sum8 = CalcChecksum8(ReceivePtr+7,4);
//			 bsp_printf("Sum8 = %x ,pos = %x ,pos2 = %x \n",sum8,pos,pos2);

			if(ReceivePtr[11] == sum8 && pos>=0 && pos2<0)
			{
			   UartTx(ReturnMeg,sizeof(ReturnMeg));

			   FirmwareTransferInit();
			   systemPar.FirmwareUpdate = 1;
			   systemPar.FirmwareUpdateStatus =1;
			   VotTools.BIGDATA_FALG = 1;
			   systemPar.FirmwareOverTime = 0;

			   memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count-pos);
			   systemPar.FirmwareUpdateLen  = (*(systemPar.PtrUpdateDDR+7) << 24) | (*(systemPar.PtrUpdateDDR+8) << 16) |
									  (*(systemPar.PtrUpdateDDR+9) << 8) | *(systemPar.PtrUpdateDDR+10);
			   ClearUartData();
			   return 1;
			}
			else if(ReceivePtr[11] == sum8 && pos2>=0 && pos<0)
			{
			   UartTx(ReturnMeg,sizeof(ReturnMeg));

			   FirmwareTransferInit();
			   systemPar.FirmwareUpdate = 2;
			   systemPar.FirmwareUpdateStatus =1;
			   VotTools.BIGDATA_FALG = 1;
			   systemPar.FirmwareOverTime = 0;

			   memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count-pos);
			   systemPar.FirmwareUpdateLen  = (*(systemPar.PtrUpdateDDR+7) << 24) | (*(systemPar.PtrUpdateDDR+8) << 16) |
									  (*(systemPar.PtrUpdateDDR+9) << 8) | *(systemPar.PtrUpdateDDR+10);
			   ClearUartData();
			   return 1;
			}
//			 bsp_printf("FirmwareUpdateLen = %x",systemPar.FirmwareUpdateLen);
		break;
		case 1: //接受第一组数据包，获取固件大小
			ReceivePtr = com_data.buff;
			   systemPar.FirmwareOverTime = 0;
			   memcpy(systemPar.PtrUpdateDDR,ReceivePtr,com_data.count);
			   systemPar.PtrUpdateDDR+=com_data.count;
			   systemPar.FirmwareLenCount +=com_data.count;
//			   bsp_printf("LenCount = %x ,UpdateLen =%x \n",systemPar.FirmwareLenCount,systemPar.FirmwareUpdateLen);

			   if(systemPar.FirmwareUpdateLen+5==systemPar.FirmwareLenCount)//传输完成
			   {
				   sum32  = (*(systemPar.PtrUpdateDDR-5) << 24) | (*(systemPar.PtrUpdateDDR-4) << 16) |
							(*(systemPar.PtrUpdateDDR-3) <<  8) | *(systemPar.PtrUpdateDDR-2);
				   systemPar.PtrUpdateDDR = (u8*)Update_Firmware_BASE_ADDR;
				   sum32cal = CalcChecksum32(systemPar.PtrUpdateDDR,systemPar.FirmwareUpdateLen);
//				   bsp_printf("Ssum32cal = %x ,Sum32 =%x \n",sum32cal,sum32);
				   if(sum32 == sum32cal)
				   {
//					   bsp_printf("ReturnMeg!!!!!"); //校验成功
						UartTx(ReturnMeg,sizeof(ReturnMeg));
						systemPar.FirmwareUpdateStatus =2;
				   }
			   }
			   ClearUartData();
			   return 1;
			   break;

		case 2:
			ReceivePtr = com_data.buff;
			if(systemPar.FirmwareUpdate==1)
				{pos = ContainsHeadSoc2(ReceivePtr,com_data.count);}
			else if(systemPar.FirmwareUpdate==2)
				{pos = ContainsHeadFpga2(ReceivePtr,com_data.count);}
			sum8 = CalcChecksum8(ReceivePtr+7,4);
			if(ReceivePtr[11] == sum8 && pos>=0 )
			{
//				bsp_printf("WriteFlashUpdataFirmware!!!");
				Write_Flash_UpdataFirmware_HuanYu(systemPar.FirmwareUpdate,systemPar.FirmwareUpdateLen); //写固件
				FirmwareTransferInit();
			}
			ClearUartData();
			return 1;
			break;
	}
 	 if(systemPar.FirmwareUpdate)  return 1;
 	 return 0;
}
#endif
//****************************************************************************************************************//

u8 RefrigerationUartOPR(void)
{
	u16 i,temp,head,num,count;
/*****************中波制冷*******************/
#if SUPPORT_Refrigeration_UART_PROTOCOL
	if(com_data.buff[i] == Refrigeration_command_head1 && com_data.buff[i+1] == Refrigeration_command_head2){
		if(com_data.buff[i+2] == Refrigeration_command_head3){
			if(i != 0){
				for(head = 0,temp = i;temp< i+15;temp++,head++)
				{com_data.buff[head] = com_data.buff[temp];}
			}
			UartRefrigeration();
			memset(com_data.buff,0,sizeof(com_data.buff));
			com_data.count = 0;
			return 1;
		}
		else return 0;
	}
	else
		return 0;

#else
	return 0;
#endif
}

u8 HuanYuUartOPR(void)
{
	u16 i,temp,head,num,count;
/********************寰宇项目********************/
#if (SUPPORT_HuanYu_UART_PROTOCOL || SUPPORT_HuanYu_UART_PROTOCOL_NET)
	if(com_data.buff[i] == HuanYu_Command_Head && com_data.buff[i+2] == HuanYu_Command_Address){
		if(com_data.buff[i+com_data.buff[1]+3] == HuanYu_Command_End){
			if(i != 0){
				for(head = 0,temp = i;temp< i+com_data.buff[1]+4;temp++,head++)
				{com_data.buff[head] = com_data.buff[temp];}
			}
			UartHuanYu();
			memset(com_data.buff,0,sizeof(com_data.buff));
			com_data.count = 0;
			return 1;
		}
		else return 0;
	}
	else return 0;
#else
	return 0;
#endif
}
u8 ZhongLinUartORR(void)
{
	u16 i,temp,head,num,count;
/********************中林项目********************/
#if SUPPORT_ZhongLin_UART_PROTOCOL
	 if(com_data.buff[i] == ZhongLin_Command_Head1 && com_data.buff[i+1] == ZhongLin_Command_Head2){
		if(com_data.buff[i+com_data.buff[2]+4] == ZhongLin_Command_Tail){
			if(i != 0){
				for(head = 0,temp = i;temp< i+com_data.buff[1]+5;temp++,head++)
				{com_data.buff[head] = com_data.buff[temp];}
			}
			UartZhongLin();
			memset(com_data.buff,0,sizeof(com_data.buff));
			com_data.count = 0;
			return 1;
		}
		else
		{
			ClearUartData();
			return 0;
		}
	}
	else if(com_data.buff[i] == ZhongLin_Servo_Head){
		if(ZhongLin_Servo_Data_Verify(&com_data.buff[i+1]) == com_data.buff[i+6]){
			if(i != 0){
				for(head = 0,temp = i;temp< i+7;temp++,head++)
				{com_data.buff[head] = com_data.buff[temp];}
			}
			UartZhongLinServo();
			memset(com_data.buff,0,sizeof(com_data.buff));
			com_data.count = 0;
			return 1;
		}
		else return 0;
	}
	else return 0;
#else
	return 0;
#endif
}

//**********************FPGA传输协议相关*********************//
#ifdef	SUPPORT_FPGA_USB_UART_PROTOCOL
void FPGACommand_Init(void)
{
	VotFPGAData.STATUS_FLAG = 0;
	VotFPGAData.DATA_LENGTH = 0;
	VotFPGAData.COMMAND_LENGTH = 0;
	VotFPGAData.RETURN_LENGTH = 0;
	VotFPGAData.HEAD_FLAG = 0;
	VotFPGAData.END_FLAG = 0;
}

void FPGACommandData_Packets(void)
{
	u32 dat;
//	u8 m=0;
//	if(ReadCommandFlag()==0x01)
	{
		FPGAcom_data.count = 64;

#if 0
		dat=read_u32(0xf8100300);
//			for(u8 n=2;n<=0;n--)
//			{
//				FPGAcom_data.buff[n] = dat>>m;
//				m+=8;
//			}
			FPGAcom_data.buff[0] = dat>>16;
			FPGAcom_data.buff[1] = dat>>8;
			FPGAcom_data.buff[2] = dat>>0;
//			bsp_printf("FPGAData111 = %x \n",dat);
			dat=read_u32(IO_APB_SLAVE_0_INPUT_UartCommand2);
//			m=0;
//			for(u8 n=6;n<2;n--)
//			{
//				FPGAcom_data.buff[n] = dat>>m;
//				m+=8;
//			}
			FPGAcom_data.buff[3] = dat>>24;
			FPGAcom_data.buff[4] = dat>>16;
			FPGAcom_data.buff[5] = dat>>8;
			FPGAcom_data.buff[6] = dat>>0;

//		bsp_printf("FPGAData222 = %x \n",dat);
			dat=read_u32(IO_APB_SLAVE_0_INPUT_UartCommand1);
//			m=0;
//			for(u8 n=10;n<6;n--)
//			{
//				FPGAcom_data.buff[n] = dat>>m;
//				m+=8;
//			}
			FPGAcom_data.buff[7] = dat>>24;
			FPGAcom_data.buff[8] = dat>>16;
			FPGAcom_data.buff[9] = dat>>8;
			FPGAcom_data.buff[10] = dat>>0;
#endif
		for(u8 k=0;k<16;k++)
		{
			dat=read_u32(0xf8100300+4*k);
			FPGAcom_data.buff[0+4*k] = dat>>0;
			FPGAcom_data.buff[1+4*k] = dat>>8;
			FPGAcom_data.buff[2+4*k] = dat>>16;
			FPGAcom_data.buff[3+4*k] = dat>>24;
		}

	 for(VotFPGAData.DATA_LENGTH=0;VotFPGAData.DATA_LENGTH < FPGAcom_data.count;VotFPGAData.DATA_LENGTH++)
	{
		 switch(VotFPGAData.STATUS_FLAG)
		 {
		 	 default:VotFPGAData.STATUS_FLAG = 0;break;
		 	 case 0:
		 		 if(FPGAcom_data.buff[VotFPGAData.DATA_LENGTH] == Vot_command_head1) VotFPGAData.STATUS_FLAG = 1;
		 		 break;
		 	 case 1:
		 		 if(FPGAcom_data.buff[VotFPGAData.DATA_LENGTH] == Vot_command_head2) VotFPGAData.STATUS_FLAG = 2;
		 		 else	{//MessageReturnState(UnframedHeader);
		 		 VotFPGAData.STATUS_FLAG = 0;VotFPGAData.DATA_LENGTH--;}
		 		 break;
		 	 case 2:
		 		 if(FPGAcom_data.buff[VotFPGAData.DATA_LENGTH] == Vot_command_head3)		{VotFPGAData.STATUS_FLAG = 3;}
		 		 else	{//MessageReturnState(UnframedHeader);
		 			 VotFPGAData.STATUS_FLAG = 0;VotFPGAData.DATA_LENGTH--;
		 			 return;
		 		 }
		 		 break;
		 	 case 3:
		 		VotFPGAData.HEAD_FLAG = VotFPGAData.DATA_LENGTH-3;
		 		VotFPGAData.COMMAND_LENGTH = ((FPGAcom_data.buff[VotFPGAData.DATA_LENGTH]<<8)|FPGAcom_data.buff[++VotFPGAData.DATA_LENGTH]);
		 		Data_Verification(&FPGAcom_data.buff[VotFPGAData.DATA_LENGTH-4],VotFPGAData.COMMAND_LENGTH+3);
		 		if((FPGAcom_data.buff[VotFPGAData.DATA_LENGTH+VotFPGAData.COMMAND_LENGTH-1] == VotArrays.Rcvbuf[0] &&\
		 				FPGAcom_data.buff[VotFPGAData.DATA_LENGTH+VotFPGAData.COMMAND_LENGTH] == VotArrays.Rcvbuf[1]))
		 			{VotFPGAData.STATUS_FLAG = 4;}
		 		else
		 		{
		 			if((VotFPGAData.DATA_LENGTH+VotFPGAData.COMMAND_LENGTH+1 <= FPGAcom_data.count) && (VotFPGAData.COMMAND_LENGTH != 0))
		 			{MessageReturnState(CalibrationFailure);VotFPGAData.END_FLAG = VotFPGAData.DATA_LENGTH+VotFPGAData.COMMAND_LENGTH+1;}
		 			VotFPGAData.STATUS_FLAG = 0;
		 			WriteCommandFlag(0,VerifyVal); //串口执行完后返回0
		 			delay_1ms(1);
		 			WriteCommandFlag(1,VerifyVal);
		 		}
		 		break;
		 	 case 4:
//		 		if(FPGAcom_data.buff[VotFPGAData.DATA_LENGTH+2] != 0x3A && FPGAcom_data.buff[VotFPGAData.DATA_LENGTH+2] != 0x48
//		 				&& FPGAcom_data.buff[VotFPGAData.DATA_LENGTH+2] != 0x55&& FPGAcom_data.buff[VotFPGAData.DATA_LENGTH+2] != 0x56)
//		 		{MessageReturnState(ReceivedSuccess);}

//函数不使用 先删除
//		 		Valid_Data_Handle(&FPGAcom_data.buff[VotFPGAData.DATA_LENGTH-5]);
		 		VotFPGAData.DATA_LENGTH = VotFPGAData.DATA_LENGTH+VotFPGAData.COMMAND_LENGTH-1;
		 		VotFPGAData.END_FLAG = VotFPGAData.DATA_LENGTH+1;
		 		VotFPGAData.STATUS_FLAG = 0;
		 		WriteCommandFlag(0,VerifyVal); //串口执行完后返回0
		 		delay_1ms(1);
		 		WriteCommandFlag(1,VerifyVal);
		 		memset(FPGAcom_data.buff,0,sizeof(FPGAcom_data.buff));FPGAcom_data.count = 0;VotFPGAData.END_FLAG=0;
		 		break;
		 	 }
		}
//		if((VotFPGAData.END_FLAG == FPGAcom_data.count)||(FPGAcom_data.count >= DATA_MAX_NUM))
//		{memset(FPGAcom_data.buff,0,sizeof(FPGAcom_data.buff));FPGAcom_data.count = 0;VotFPGAData.END_FLAG=0;}
	}
}
void FPGACommand_Tem(void)
{
//#ifdef Temperature_Version
	u8 CaptureState;
	u8 RxBuff[64]={0};
	u16 Data;
	u32 dat1;
	if(!SystemParaMenu2.Usermenu_SW)	return;
		write_u32(0x00544F56,0xf8100340);
		write_u32(0x3CCA003D,0xf8100344);
		RxBuff[0]=0x56;
		RxBuff[1]=0x4F;
		RxBuff[2]=0x54;
		RxBuff[3]=0x00;
		RxBuff[4]=0x3D;
		RxBuff[5]=0x00;
		RxBuff[6]=0xCA;
		RxBuff[7]=0x3C;
		if(UsermenuParameter.CaptureFlag == 1)			CaptureState=0x01;
		else if(UsermenuParameter.CaptureFlag == 2)		CaptureState=0x02;
		else if(UsermenuParameter.CaptureFlag == 3)		CaptureState=0x03;
		else if(UsermenuParameter.CaptureFlag == 0)		CaptureState=0x00;
		else if(UsermenuParameter.CaptureFlag == 4)		CaptureState=0x04; //平均温
		else if(UsermenuParameter.CaptureFlag == 5)		CaptureState=0x06; //平均温+最高温
		else if(UsermenuParameter.CaptureFlag == 6)		CaptureState=0x05; //平均温+最低温
		else if(UsermenuParameter.CaptureFlag == 7)		CaptureState=0x07; //平均温+最低温+最高温
		Data=(Spot_state&0x01FF)|((Area_state&0x07)<<9)|((CaptureState&0x07)<<12);  //9点+3区域+最高最低平均温
//		write_u32(((read_u32(0xf8100348)&0xFFFF0000)|Data),0xf8100348);
//		bsp_printf("Spot_state = %x, Area_state = %x, CaptureState = %x,\n",Spot_state,Area_state,CaptureState);
		RxBuff[8]=Data>>8;
		RxBuff[9]=Data;
		//*******************************************9个点******************************************//
		if(Spot_state&0x0001)	{
//			spot_temp1=10; //测试值
		dat1 = Data|((spot_temp1&0xFFFF)<<16);
		RxBuff[10]=spot_temp1>>8;RxBuff[11]=spot_temp1;}
		else								{dat1 = Data|((0&0xFFFF)<<16);RxBuff[10]=0;RxBuff[11]=0;}
		write_u32(dat1,0xf8100348); //0xf8100348

		if(Spot_state&0x0002)	{
//			spot_temp2=20; //测试值
		RxBuff[12]=spot_temp2>>8;RxBuff[13]=spot_temp2;}
		else								{spot_temp2=0;RxBuff[12]=0;RxBuff[13]=0;}
		if(Spot_state&0x0004)	{
//			spot_temp3=30; //测试值
		RxBuff[14]=spot_temp3>>8;RxBuff[15]=spot_temp3;}
		else								{spot_temp3=0;RxBuff[14]=0;RxBuff[15]=0;}
		dat1=(spot_temp2&0xFFFF)|((spot_temp3&0xFFFF)<<16);
		write_u32(dat1,0xf810034C); //0xf810034C

		if(Spot_state&0x0008)	{
//			spot_temp4=40; //测试值
		RxBuff[16]=spot_temp4>>8;RxBuff[17]=spot_temp4;}
		else								{spot_temp4=0;RxBuff[16]=0;RxBuff[17]=0;}
		if(Spot_state&0x0010)	{
//			spot_temp5=50;//测试值
		RxBuff[18]=spot_temp5>>8;RxBuff[19]=spot_temp5;}
		else								{spot_temp5=0;RxBuff[18]=0;RxBuff[19]=0;}
		dat1=(spot_temp4&0xFFFF)|((spot_temp5&0xFFFF)<<16);
		write_u32(dat1,0xf8100350); //0xf8100350

		if(Spot_state&0x0020)	{
//			spot_temp6=60; //测试值
		RxBuff[20]=spot_temp6>>8;RxBuff[21]=spot_temp6;}
		else								{spot_temp6=0;RxBuff[20]=0;RxBuff[21]=0;}
		if(Spot_state&0x0040)	{
//			spot_temp7=70;//测试值
		RxBuff[22]=spot_temp7>>8;RxBuff[23]=spot_temp7;}
		else								{spot_temp7=0;RxBuff[22]=0;RxBuff[23]=0;}
		dat1=(spot_temp6&0xFFFF)|((spot_temp7&0xFFFF)<<16);
		write_u32(dat1,0xf8100354); //0xf8100354

		if(Spot_state&0x0080)	{
//			spot_temp8=80; //测试值
		RxBuff[24]=spot_temp8>>8;RxBuff[25]=spot_temp8;}
		else								{spot_temp8=0;RxBuff[24]=0;RxBuff[25]=0;}
		if(Spot_state&0x0100)	{
//			spot_temp9=90;//测试值
		RxBuff[26]=spot_temp9>>8;RxBuff[27]=spot_temp9;}
		else								{spot_temp9=0;RxBuff[26]=0;RxBuff[27]=0;}
		dat1=(spot_temp8&0xFFFF)|((spot_temp9&0xFFFF)<<16);
		write_u32(dat1,0xf8100358); //0xf8100358

//		if(Spot_state&0x0008)	{spot_temp4=40;FPGAComDataSet_Low(spot_temp4,0xf8100350);RxBuff[16]=spot_temp4>>16;RxBuff[17]=spot_temp4;}
//		else								{FPGAComDataSet_Low(0,0xf8100350);RxBuff[16]=0;RxBuff[17]=0;}
//		if(Spot_state&0x0010)	{spot_temp5=50;FPGAComDataSet_High(spot_temp5,0xf8100350);RxBuff[18]=spot_temp5>>16;RxBuff[19]=spot_temp5;}
//		else								{FPGAComDataSet_High(0,0xf8100350);RxBuff[18]=0;RxBuff[19]=0;}
//		if(Spot_state&0x0020)	{spot_temp6=60;FPGAComDataSet_Low(spot_temp6,0xf8100354);RxBuff[20]=spot_temp6>>16;RxBuff[21]=spot_temp6;}
//		else								{FPGAComDataSet_Low(0,0xf8100354);RxBuff[20]=0;RxBuff[21]=0;	}
//		if(Spot_state&0x0040)	{spot_temp7=70;FPGAComDataSet_High(spot_temp7,0xf8100354);RxBuff[22]=spot_temp7>>16;RxBuff[23]=spot_temp7;}
//		else								{FPGAComDataSet_High(0,0xf8100354);RxBuff[22]=0;RxBuff[23]=0;}
//		if(Spot_state&0x0080)	{spot_temp8=80;FPGAComDataSet_Low(spot_temp8,0xf8100358);RxBuff[24]=spot_temp8>>16;RxBuff[25]=spot_temp8;}
//		else								{FPGAComDataSet_Low(0,0xf8100358);RxBuff[24]=0;RxBuff[25]=0;}
//		if(Spot_state&0x0100)	{spot_temp9=90;FPGAComDataSet_High(spot_temp9,0xf8100358);RxBuff[26]=spot_temp9>>16;RxBuff[27]=spot_temp9;}
//		else								{FPGAComDataSet_High(0,0xf8100358);RxBuff[26]=0;RxBuff[27]=0;}

//		bsp_printf("spot_temp =  %x, %x,%x,%x,%x,%x,%x,%x,%x \n",spot_temp1,spot_temp2,spot_temp3,spot_temp4,spot_temp5,spot_temp6,spot_temp7,spot_temp8,spot_temp9);//这里可以替换成错误码
		//*******************************************4区域******************************************//
		if(Area_state&0x0001) //Area1
		{
//			Temp0_max=130;
//			Temp0_avg=120;
//			Temp0_min = 110; //测试值
//			FPGAComDataSet_Low(Temp0_max,0xf810035C);
//			FPGAComDataSet_High(Temp0_min,0xf810035C);
//			FPGAComDataSet_Low(Temp0_avg,0xf8100360);
			RxBuff[28]=Temp0_max>>8;
			RxBuff[29]=Temp0_max;
			RxBuff[30]=Temp0_min>>8;
			RxBuff[31]=Temp0_min;
			RxBuff[32]=Temp0_avg>>8;
			RxBuff[33]=Temp0_avg;
		}
		else
		{
			Temp0_max=0;
			Temp0_avg=0;
			Temp0_min = 0;
			RxBuff[28]=0;
			RxBuff[29]=0;
			RxBuff[30]=0;
			RxBuff[31]=0;
			RxBuff[32]=0;
			RxBuff[33]=0;
		}
		dat1=(Temp0_max&0xFFFF)|((Temp0_min&0xFFFF)<<16);
		write_u32(dat1,0xf810035C);  //0xf810035C

		if(Area_state&0x0002)//Area2
		{
//			Temp1_max=230;
//			Temp1_avg=220;
//			Temp1_min = 210; //测试值
//			FPGAComDataSet_High(Temp1_max,0xf8100360);
//			FPGAComDataSet_Low(Temp1_min,0xf8100364);
//			FPGAComDataSet_High(Temp1_avg,0xf8100364);
			RxBuff[34]=Temp1_max>>8;
			RxBuff[35]=Temp1_max;
			RxBuff[36]=Temp1_min>>8;
			RxBuff[37]=Temp1_min;
			RxBuff[38]=Temp1_avg>>8;
			RxBuff[39]=Temp1_avg;
		}
		else
		{
			Temp1_max=0;
			Temp1_avg=0;
			Temp1_min = 0;
			RxBuff[34]=0;
			RxBuff[35]=0;
			RxBuff[36]=0;
			RxBuff[37]=0;
			RxBuff[38]=0;
			RxBuff[39]=0;
		}
		dat1=(Temp0_avg&0xFFFF)|((Temp1_max&0xFFFF)<<16);
		write_u32(dat1,0xf8100360); //0xf8100360
		dat1=(Temp1_min&0xFFFF)|((Temp1_avg&0xFFFF)<<16);
		write_u32(dat1,0xf8100364); //0xf8100364

		if(Area_state&0x0004) //Area3
		{
//			Temp2_max=330;
//			Temp2_avg=320;
//			Temp2_min = 310; //测试值
//			FPGAComDataSet_Low(Temp2_max,0xf8100368);
//			FPGAComDataSet_High(Temp2_min,0xf8100368);
//			FPGAComDataSet_Low(Temp2_avg,0xf810036C);
			RxBuff[40]=Temp2_max>>8;
			RxBuff[41]=Temp2_max;
			RxBuff[42]=Temp2_min>>8;
			RxBuff[43]=Temp2_min;
			RxBuff[44]=Temp2_avg>>8;
			RxBuff[45]=Temp2_avg;
		}
		else
		{
			Temp2_max=0;
			Temp2_avg=0;
			Temp2_min = 0;
			RxBuff[40]=0;
			RxBuff[41]=0;
			RxBuff[42]=0;
			RxBuff[43]=0;
			RxBuff[44]=0;
			RxBuff[45]=0;
		}
		dat1=(Temp2_max&0xFFFF)|((Temp2_min&0xFFFF)<<16);
		write_u32(dat1,0xf8100368); //0xf8100368
#if 0
		if(Area_state&0x0008) //Area4
		{
//			Temp3_max=430;
//			Temp3_avg=420;
//			Temp3_min = 410; //测试值
//			FPGAComDataSet_High(Temp3_max,0xf810036C);
//			FPGAComDataSet_Low(Temp3_min,0xf8100370);
//			FPGAComDataSet_High(Temp3_avg,0xf8100370);
			RxBuff[46]=Temp3_max>>8;
			RxBuff[47]=Temp3_max;
			RxBuff[48]=Temp3_min>>8;
			RxBuff[49]=Temp3_min;
			RxBuff[50]=Temp3_avg>>8;
			RxBuff[51]=Temp3_avg;
		}
		else
		{
			Temp3_max=0;
			Temp3_avg=0;
			Temp3_min = 0;
			RxBuff[46]=0;
			RxBuff[47]=0;
			RxBuff[48]=0;
			RxBuff[49]=0;
			RxBuff[50]=0;
			RxBuff[51]=0;
		}

		dat1=(Temp2_avg&0xFFFF)|((Temp3_max&0xFFFF)<<16);
		write_u32(dat1,0xf810036C); //	0xf810036C
		dat1=(Temp3_min&0xFFFF)|((Temp3_avg&0xFFFF)<<16);
		write_u32(dat1,0xf8100370);//	0xf8100370
#endif
		if(CaptureState)
		{
			//*******************************************全屏平均温******************************************//
			if(CaptureState&0x04) //Avg
			{
	//				capture_temp_min = -100;
	//				spot_min.spot_X_Pos = 240;
	//				spot_min.spot_Y_Pos = 320; //测试值
					APB3_REGW(IO_APB_SLAVE_0_INPUT_Area4_Left_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area4_Left_XY__SET,0)&0x00000000));//设置测温区域左上X位置，设置测温区域左上Y位置
					APB3_REGW(IO_APB_SLAVE_0_INPUT_Area4_Right_XY__SET,0,(APB3_REGR(IO_APB_SLAVE_0_INPUT_Area4_Right_XY__SET,0)&0x00000000)|((0x1FF<<16)+0x27F));//设置测温区域右下X位置，设置测温区域右下Y位置
					Areainit[3].Area_temp_AVG = read_u32(IO_APB_SLAVE_0_INPUT_AREAN4_TEMPAVG_START)/(639*511);
					Temp3_avg = AreaTemp_convert_AVG(Areainit[3].Area_temp_AVG);
					RxBuff[46]=0;
					RxBuff[47]=0;
					RxBuff[48]=0;
					RxBuff[49]=0;
					RxBuff[50]=Temp3_avg>>8;
					RxBuff[51]=Temp3_avg;
				}
				else
				{
					Temp3_avg=0;
					RxBuff[46]=0;
					RxBuff[47]=0;
					RxBuff[48]=0;
					RxBuff[49]=0;
					RxBuff[50]=0;
					RxBuff[51]=0;
				}
			dat1=(Temp2_avg&0xFFFF)|((0&0xFFFF)<<16);
			write_u32(dat1,0xf810036C); //	0xf810036C 高16位预留
			dat1=(0&0xFFFF)|((Temp3_avg&0xFFFF)<<16);
			write_u32(dat1,0xf8100370);//	0xf8100370 低高16位预留
		//*******************************************全屏最高最低温******************************************//

			if(CaptureState&0x02) //Max
			{
//				capture_temp_int=1000;
//				spot_max.spot_X_Pos = 320;
//				spot_max.spot_Y_Pos = 240; //测试值
//				FPGAComDataSet_Low(capture_temp_int,0xf8100374);
//				FPGAComDataSet_High(spot_max.spot_X_Pos,0xf8100374);
//				FPGAComDataSet_Low(spot_max.spot_Y_Pos,0xf8100378);
				RxBuff[52]=capture_temp_int>>8;
				RxBuff[53]=capture_temp_int;
				RxBuff[54]=spot_max.spot_X_Pos>>8;
				RxBuff[55]=spot_max.spot_X_Pos;
				RxBuff[56]=spot_max.spot_Y_Pos>>8;
				RxBuff[57]=spot_max.spot_Y_Pos;
			}
			else
			{
				capture_temp_int=0;
				spot_max.spot_X_Pos = 0;
				spot_max.spot_Y_Pos = 0;
				RxBuff[52]=0;
				RxBuff[53]=0;
				RxBuff[54]=0;
				RxBuff[55]=0;
				RxBuff[56]=0;
				RxBuff[57]=0;
			}
			dat1=(capture_temp_int&0xFFFF)|((spot_max.spot_X_Pos&0xFFFF)<<16);
			write_u32(dat1,0xf8100374); //0xf8100374

			if(CaptureState&0x01) //Min
			{
//				capture_temp_min = -100;
//				spot_min.spot_X_Pos = 240;
//				spot_min.spot_Y_Pos = 320; //测试值
//				FPGAComDataSet_High(capture_temp_min,0xf8100378);
//				FPGAComDataSet_Low(spot_min.spot_X_Pos,0xf810037C);
//				FPGAComDataSet_High(spot_min.spot_Y_Pos,0xf810037C);
				RxBuff[58]=capture_temp_min>>8;
				RxBuff[59]=capture_temp_min;
				RxBuff[60]=spot_min.spot_X_Pos>>8;
				RxBuff[61]=spot_min.spot_X_Pos;
				RxBuff[62]=spot_min.spot_Y_Pos>>8;
				RxBuff[63]=spot_min.spot_Y_Pos;
			}
			else
			{
				capture_temp_min = 0;
				spot_min.spot_X_Pos = 0;
				spot_min.spot_Y_Pos = 0;
				RxBuff[58]=0;
				RxBuff[59]=0;
				RxBuff[60]=0;
				RxBuff[61]=0;
				RxBuff[62]=0;
				RxBuff[63]=0;
			}
			dat1=(spot_max.spot_Y_Pos&0xFFFF)|((capture_temp_min&0xFFFF)<<16);
			write_u32(dat1,0xf8100378); //0xf8100378
			dat1=(spot_min.spot_X_Pos&0xFFFF)|((spot_min.spot_Y_Pos&0xFFFF)<<16);
			write_u32(dat1,0xf810037C); //0xf810037C
		}
		else //OFF
		{
			dat1=(Temp2_avg&0xFFFF)|((0&0xFFFF)<<16);
			write_u32(dat1,0xf810036C); //	0xf810036C
			write_u32(0,0xf8100370);//	0xf8100370
			write_u32(0,0xf8100374);
			write_u32(0,0xf8100378);
			write_u32(0,0xf810037C);
			RxBuff[46]=0;
			RxBuff[47]=0;
			RxBuff[48]=0;
			RxBuff[49]=0;
			RxBuff[50]=0;
			RxBuff[51]=0;
			RxBuff[52]=0;
			RxBuff[53]=0;
			RxBuff[54]=0;
			RxBuff[55]=0;
			RxBuff[56]=0;
			RxBuff[57]=0;
			RxBuff[58]=0;
			RxBuff[59]=0;
			RxBuff[60]=0;
			RxBuff[61]=0;
			RxBuff[62]=0;
			RxBuff[63]=0;
		}
		Data_Verification(&RxBuff[0],64);
//		for(u8 k=0;k<64;k++)
//		{
//			bsp_printf("RxBuff[%d] =  %x  \n",k,RxBuff[k]);
//		}
		VerifyVal=VotArrays.Rcvbuf[1]|(VotArrays.Rcvbuf[0]<<8);
		dat1 = VerifyVal|0x10000;
//		bsp_printf("dat1 =  %x ,%x \n",dat1,VerifyVal);//这里可以替换成错误码
		write_u32(dat1,0xf8100380);

//		write_u32(0x00544F56,0xf8100340);
//		write_u32(0x3CCA003A,0xf8100344);
//		write_u32(0x03020100,0xf8100340);
//		write_u32(0x07060504,0xf8100344);
//		write_u32(0x3CCA003A,0xf8100348);
//		write_u32(0x11100908,0xf810034C);
//		write_u32(0x19181716,0xf8100350);
//		write_u32(0x23222120,0xf8100354);
//		write_u32(0x27262524,0xf8100358);
//		write_u32(0x31302928,0xf810035C);
//		write_u32(0x35343332,0xf8100360);
//		write_u32(0x39383736,0xf8100364);
//		write_u32(0x43424140,0xf8100368);
//		write_u32(0x47464544,0xf810036C);
//		write_u32(0x51504948,0xf8100370);
//		write_u32(0x55545352,0xf8100374);
//		write_u32(0x59585756,0xf8100378);
//		write_u32(0x63626160,0xf810037C);
//		write_u32(0x67666564,0xf8100380);
//		for(u8 k=0;k<17;k++)
//		{
//			write_u32(0,0xf8100340+4*k);
//		}
//#endif
}
#endif
//*****************************************************************//
#ifdef SUPPORT_BoYu_UART_PROTOCOL

u8 BoYucalculateXOR(u8* data, u32 length) {
    uint8_t xorValue = 0;
    for(uint8_t i = 0; i < length; i++) {
        xorValue ^= data[i];
    }
    return xorValue;
}
void BoYuUartTx(u8*ptr,u32 len)
{
	for(int i=0;i<len;i++)
	{
		bsp_printf("%c",*ptr++);
	}
}
void BoYuUartOPR(u16 command,u32 data)
{
	u8 senddata[8]={0};
	senddata[0] = 0x24;
/*	switch(command) //按测试需要增加即可
	{
		case _CLIENT_FOC_SPD_STEP_ADD:
			senddata[1] = (u8)(_CLIENT_FOC_SPD_STEP_ADD>>8);
			senddata[2] = (u8)(_CLIENT_FOC_SPD_STEP_ADD>>0);
			break;
	}
*/
	senddata[1] = (u8)(command>>8);
	senddata[2] = (u8)(command>>0);
	memcpy(senddata+3,&data,4);
	senddata[7] = BoYucalculateXOR(senddata,7);
	BoYuUartTx(senddata,8);
}
#endif



void uart_command(void)
{
	u16 i,temp,head,num,count;

#ifdef SUPPORT_ZhongLin_UART_PROTOCOL
	TimeOutORR();  //中林项目在线升级超时判断
#endif
	if(com_data.time < 10)
	{
		return;
	}
	com_data.time = 0;
	com_data.start_flag = 0;

#ifdef SUPPORT_ZhongLin_UART_PROTOCOL
	if(CustomusizedOPR()) return;				//中林项目 在线升级
	if(ZhongLinUartORR()) return ;				//中林项目
#else
	#ifdef	SUPPORT_HuanYu_UART_PROTOCOL
		if(HuanYuCustomusizedOPR()) return;				// 在线升级
	#endif
	if(VotTools.BIGDATA_FALG == 0)
	{
		if(IrayAgreement()) 		return; 	//iray协议 -测距
		if(TESTCC()) 				return; 	//测试使用
		if(RefrigerationUartOPR())  return ;  	//中波制冷
		if(HuanYuUartOPR()) 		return ;	//寰宇项目
	}
#endif
	Data_Packets_Receive_And_Process();			//Vot 全新vot串口协议 需支持分包传输
}




