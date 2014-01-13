#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"

#include "core.h"

#include "driv_rtc.h"

/*******************************************************************************
* Function Name  : driv_rtc_reconfig
* Description    : Reconfigure RTC and BKP, only used when detecting loss of
*                   BKP data.
* Input          : None
* Return         : None
*******************************************************************************/
static void driv_rtc_reconfig(void)
{
	//start PWR, BKP clock(from APB1)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);//unlock BKP
	BKP_DeInit();//BKP default init

	RCC_LSEConfig(RCC_LSE_ON);//open erternal 32768Hz clock
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);//wait for clock stable
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//32768Hz used as RTC source
	RCC_RTCCLKCmd(ENABLE);//open RTC
	RTC_WaitForSynchro();//wait for APB1 synchronize with RTC

	RTC_WaitForLastTask();//confirm last operation is over
	RTC_SetPrescaler(32767);//RTC period = RTCCLK/RTC_PR = (32768Hz)/(32767+1)
	RTC_WaitForLastTask();//wait for operation is over
	RTC_ITConfig(RTC_IT_SEC, ENABLE);//enable second interrupt    
	RTC_WaitForLastTask();//wait for operation is over

	return;
}
//end function RTC_ReConfig

/***********************************************************************
* Function Name  : IsLeapYear
* Description    : Check whether the past year is leap or not.
* Input          : 4 digits year number
* Return         : 1: leap year. 0: not leap year
***********************************************************************/
static bool IsLeapYear(int16_t nYear)
{
    if(nYear % 4 != 0)      return false;
    if(nYear % 100 != 0)    return true;
    return (nYear % 400 == 0) ? true:false;
}
//end function IsLeapYear

/*******************************************************************************
* Function Name  : MyLocalTime
* Description    : Form a calendar from 32 bit second counting.
* Input          : pointer to a 32 bit second value
* Return         : Calendar structure
*******************************************************************************/
static void MyLocalTime(const uint32_t *pTotalSeconds, Calendar_Def *Calendar)
{
	uint32_t TotalDays, RemainSeconds;
	int16_t DaysInYear=365;
	int16_t DaysInMonth=30;
	
	Calendar->year = START_YEAR;
	Calendar->month = 1;  Calendar->mday = 1;
	Calendar->yday = 1;  
	//Calendar->wday = Ymd2Wday(START_YEAR, 1, 1);

	TotalDays = *pTotalSeconds / SEC_IN_DAY;
	RemainSeconds = *pTotalSeconds % SEC_IN_DAY;
	Calendar->hour = (int16_t)(RemainSeconds / 3600);//calculate hour
	Calendar->min = (int16_t)((RemainSeconds / 60) % 60);//calculate minute
	Calendar->sec = (int16_t)(RemainSeconds % 60);//calculate second
	Calendar->wday = (int16_t)((TotalDays + Calendar->wday) % 7);//calculate wday

	while(1) {//calculate year
		if(IsLeapYear(Calendar->year)) DaysInYear = 366;
		else DaysInYear = 365;

		if(TotalDays >= DaysInYear) {
			TotalDays -= DaysInYear;
			Calendar->year ++;
		}//continue while
		else
			break;//forced to end while
	}//finish year calculation

	Calendar->yday += TotalDays;//calculate yday

	while(1) {//calculate month
		if(Calendar->month == 1||Calendar->month == 3||Calendar->month == 5
		||Calendar->month == 7||Calendar->month == 8
		||Calendar->month == 10||Calendar->month == 12)
			DaysInMonth = 31;
		else if(Calendar->month == 2) {
			if(IsLeapYear(Calendar->year)) DaysInMonth = 29;
			else DaysInMonth = 28;
		}
		else DaysInMonth = 30;

		if(TotalDays >= DaysInMonth) {
			TotalDays -= DaysInMonth;
			Calendar->month ++;
		}//continue while
		else
			break;//forced to end while
	}//finish month calculation

	Calendar->mday += (int16_t)TotalDays;//calculate mday

	//return Calendar;
}
//end function MyLocalTime

/*******************************************************************************
* Function Name  : MyMakeTime
* Description    : Form a 32 bit second counting value from calendar.
* Input          : pointer to a calendar struct
* Return         : 32 bit second counting value
*******************************************************************************/
static uint32_t MyMakeTime(Calendar_Def *pCalendar)
{
	uint32_t TotalSeconds = pCalendar->sec;
	int16_t nYear = pCalendar->year;
	int16_t DaysInYear = 365;
	int16_t nMonth = pCalendar->month;
	int16_t DaysInMonth = 30;

	if((nYear < START_YEAR) || (nYear > (START_YEAR + 135))) 
		return 0;//out of year range

	TotalSeconds += (uint32_t)pCalendar->min * 60;//contribution of minutes
	TotalSeconds += (uint32_t)pCalendar->hour * 3600;//contribution of hours
	//contribution of mdays
	TotalSeconds += (uint32_t)(pCalendar->mday - 1) * SEC_IN_DAY;

	while(nMonth > 1) {//contribution of months
		nMonth --;
		if(nMonth == 1||nMonth == 3||nMonth == 5||nMonth == 7
		||nMonth == 8||nMonth == 10||nMonth == 12)
			DaysInMonth = 31;
		else if(nMonth == 2) {
			if(IsLeapYear(nYear)) DaysInMonth = 29;
			else DaysInMonth = 28;
		}
		else DaysInMonth = 30;

		TotalSeconds += (uint32_t)DaysInMonth * SEC_IN_DAY;
	}

	while(nYear > START_YEAR) {//contribution of years
		nYear --;
		if(IsLeapYear(nYear)) DaysInYear = 366;
		else DaysInYear = 365;

		TotalSeconds += (uint32_t)DaysInYear * SEC_IN_DAY;
	}

	return TotalSeconds;
} 
//end function MyMakeTime

/*******************************************************************************
* Function Name  : driv_rtc_init
* Description    : It is called when power on. Detect if it needs reconfigure.
* Input          : None
* Return         : None
*******************************************************************************/
void driv_rtc_init(void)
{
	//In BKP DR1, we stored a special character 0xA5A5. When forst power on, or
	//lost battery power, this charater will be lost. RTC needs reconfiguration.
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
//		driv_rtc_reconfig();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);//restore special character
	}
	else {
		//RTC is still running, doesn't need reconfiguration.
		//We can get reset type by RCC_GetFlagStatus()
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {//this is power on reset
		
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {//this is external RST pin reset
		
		}

		RCC_ClearFlag();
		//repeat some operations to ensure RTC is okay.
		RCC_RTCCLKCmd(ENABLE);//open RTC
		RTC_WaitForSynchro();//wait for APB1 synchronize with RTC

		RTC_ITConfig(RTC_IT_SEC, ENABLE);//enable second interrupt  
		RTC_WaitForLastTask();//wait for operation is over
	}

	RCC_ClearFlag();

	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);/* Allow access to BKP Domain */
	// Disable the Tamper Pin To output RTCCLK/64 on Tamper pin
	BKP_TamperPinCmd(DISABLE); 
	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);

	return;
}
//end function RTC_UserInit

/*******************************************************************************
* Function Name  : driv_rtc_get_time
* Description    : Get time to struct *pCalendar from RTC.
* Input          : pointer to a calendar struct
* Return         : 
*******************************************************************************/
void driv_rtc_get_time(Calendar_Def *pTimeNow)
{
	//if(SetTimeFlag)    return;//lock RTC for set time

	uint32_t t_t = RTC_GetCounter();//call function in stm32f10x lib
	MyLocalTime(&t_t, pTimeNow);//call function in local file
}
//end function GetTime

/*******************************************************************************
* Function Name  : driv_rtc_set_time
* Description    : set time to RTC as demand in menu.
* Input          : pointer to a calendar struct 
* Return         : 
*******************************************************************************/
void driv_rtc_set_time(Calendar_Def *pTimeNow)
{
	uint64_t ltick;
	
	//if(!SetTimeFlag)    return;//no application for set time 
	//SetTimeFlag = 0;//set time to RTC as demand in menu

	uint32_t t_t = MyMakeTime(pTimeNow);//call function in local file

	ltick = ticks + 5;
	while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET) {//Loop until RTOFF flag is set 
		if(ltick <= ticks) break;//timing out
	}

	//  RTC_WaitForLastTask();
	RTC_SetCounter(t_t);//call function in stm32f10x lib
	//  RTC_WaitForLastTask();

	ltick = ticks + 5;
	while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET) {//Loop until RTOFF flag is set
		if(ltick <= ticks) break;//timing out
	}
}
//end function SetTime
