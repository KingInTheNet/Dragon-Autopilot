NRF_LOG_INFO("TIME: 2020-06-04 08:37:55");
  uint64_t test = 1591259875;//878;                  
  unsigned char blah[8];
  memcpy(blah,&test,sizeof(uint64_t));
  uint64_t res;
  memcpy(&res,&blah[0],sizeof(blah));
  struct tm taiTime;
  const time_t timeT = res;
  gmtime_r(&timeT,&taiTime);
  NRF_LOG_INFO("To TAI time: year %d  month %d day %d hour %d minute %d  second %d",taiTime.tm_year,taiTime.tm_mon+1,taiTime.tm_mday,taiTime.tm_hour,taiTime.tm_min,taiTime.tm_sec);
  uint32_t starting_time = 28800;
  uint32_t ending_time = 57600;
  get_time_sec_to_hr(starting_time,&hr_start, &mnt_start);
  get_time_sec_to_hr(ending_time,&hr_end, &mnt_end);
  NRF_LOG_INFO("Start time: %lu hr,%lu minutes",hr_start, mnt_start);
  NRF_LOG_INFO("End time: %lu hr,%lu minutes",hr_end, mnt_end);


/**
 * @brief BLE task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */
STATIC FUNC(void, TASK) BLE_task_function(void *pvParameter)
{

  UNUSED_PARAMETER(pvParameter);
  NRF_LOG_INFO("Enter BLE task");
  uint32_t err_code;
  uint16_t size;
  prev_timeStamp.l = 0;
  for (;;)
  {
    if (!handshake_success && ble_connected)
    {
      //if(m_rtc.p_reg->COUNTER*1000/1024-prev_timeStamp.l>=20){
      //	prev_timeStamp.l=m_rtc.p_reg->COUNTER*1000/1024;
      //	size = sizeof(realtime_package);
      //send_message((unsigned char *)&realtime_package,&size);
      //}
      handshake_commence();
      nrf_delay_ms(50);
    }
    if (result != prev_result)
    {
      prev_result = result;
      NRF_LOG_INFO("Result updated: %x", result);
    }
    if(handshake_success)
    {
        switch (result)
        {
        case HUB_GET_REALTIME:
          //	//if(m_rtc.p_reg->COUNTER*1000/1024-prev_timeStamp.l>=freq){
          //			//prev_timeStamp.l=m_rtc.p_reg->COUNTER*1000/1024;
          //			if(index_send>=DATA_SIZE_REAL_TIME){
          //				memcpy(&realtime_package.data,data_send,sizeof(data_send));
          //				realtime_package.pack_id++;
          //				countess.value++;
          //				size = sizeof(realtime_frame_t);
          //				send_message((unsigned char *)&realtime_package,&size);
          //				//index_send=0;
          //			}
          //	//}
          general_ack.id = HUB_GET_REALTIME;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          break;
        case HUB_SET_FREQ:
          NRF_LOG_INFO("Setting frequency\r\n");
          NRF_LOG_INFO("Frequency set at: %hu", freq);
          general_ack.id = HUB_SET_FREQ;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result = 0x00;
          break;
        case HUB_GET_STATUS:
          size = (uint16_t)sizeof(power_level);
          send_message((unsigned char *)&power_level, &size);
          size = (uint16_t)sizeof(store);
          send_message((unsigned char *)&store, &size);
          size = sizeof(report_firmware);
          send_message((unsigned char *)&report_firmware,&size);
          size = sizeof(selftest_ack);
          send_message((unsigned char *)&selftest_ack,&size);
          size = sizeof(report_timeStamp);                 
          send_message((unsigned char *)&report_timeStamp,&size);
          size = sizeof(report_config);                 
          send_message((unsigned char *)&report_config,&size);
          size = sizeof(report_offset);                 
          send_message((unsigned char *)&report_offset,&size);
          NRF_LOG_INFO("Status sent\r\n");
          result = 0x00;
          break;
        case HUB_REQUEST_STOP:
          NRF_LOG_INFO("Stopped");
          i = 0;
          size = sizeof(countess);
          send_message((unsigned char *)&countess, &size);
          countess.value = 0;
          //index_send=0;
          general_ack.id = HUB_REQUEST_STOP;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result = 0x00;
          break;
        case HUB_REQUEST_FLASH:
          if (!initTimeStamp){
              timeStamp.l = m_rtc.p_reg->COUNTER*1000/1024;
              start = timeStamp.l;
              initTimeStamp=true;
          }
          
          if (i < 11250)
          {
            i++;
            if (k == 128)
            {
              k = 0;
              //timeStamp.l +=170*20;
              //memcpy(&data_buffer[0], (unsigned char *)&timeStamp, sizeof(timeStamp));
              memset(&data_buffer[0], random_range(0,i%100), DATA_SIZE_READ_REAL_FLASH);
            }
            //memcpy(&data_package.dat[0], data_buffer, DATA_SIZE_SEND_LOG);
            data_package.pack_id = i;
            //data_package.timeStamp = (uint32_t)data_buffer[3] << 24 | (uint32_t)data_buffer[2] << 16 | (uint32_t)data_buffer[1] << 8 | (uint32_t)data_buffer[0];
            memcpy(&data_package.dat[0], &data_buffer[k * 16 ], DATA_SIZE_SEND_LOG);
            uint16_t size = sizeof(data_package);
            send_chunk_data(data_package, &size);
            k += 14;
            if(k==126)
            {
                i++;
                data_package.pack_id=i;
                memset(&data_package.dat[0], random_range(0,i%100), 32);
                data_package.record_size=2;
                size = sizeof(data_package);
                send_chunk_data(data_package, &size);
                k+=2;
            }
            //NRF_LOG_INFO("Sent %d data lines",i*17);
          }
          else
          {
            eof.value = i;
            i = 0;
            result = 0x00;
            uint16_t size = sizeof(eof);
            send_message((unsigned char *)&eof, &size);
            initTimeStamp=false;
            NRF_LOG_INFO("Flash sent\r\n");
            NRF_LOG_INFO("Speed: %lu",(m_rtc.p_reg->COUNTER*1000/1024-start)/11250);
              //NRF_LOG_INFO("Total data sent: %d",eof.value*17);
          }
          break;
        case HUB_HANDSHAKE:
          NRF_LOG_INFO("Handshake success\r\n");
          result = 0x00;
          break;
        case HUB_SETUP_ACCEL:
          NRF_LOG_INFO("Setting ACCEL\r\n");
          general_ack.id = HUB_SETUP_ACCEL;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result = 0x00;
          break;
        case HUB_SETUP_GYRO:
          NRF_LOG_INFO("Setting GYRO\r\n");
          general_ack.id = HUB_SETUP_GYRO;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result = 0x00;
          break;
        case HUB_SETUP_START_TIME:
          NRF_LOG_INFO("Setting start time\r\n");
          general_ack.id = HUB_SETUP_START_TIME;
          get_time_sec_to_hr(start_time,&hr_start,&mnt_start);
          get_time_sec_to_hr(end_time,&hr_end,&mnt_end);
          NRF_LOG_INFO("Start time: %lu hr,%lu minutes",hr_start, mnt_start);
          NRF_LOG_INFO("End time: %lu hr,%lu minutes",hr_end, mnt_end);
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result = 0x00;
          break;
        case HUB_SETUP_END_TIME:
          NRF_LOG_INFO("Setting end time\r\n");
          general_ack.id = HUB_SETUP_END_TIME;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result = 0x00;
          break;
        case HUB_SETUP_DEVICE_ID:
          general_ack.id = HUB_SETUP_DEVICE_ID;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          update_id();
          NRF_LOG_INFO("Setting device id\r\n");
          result = 0x00;
          break;
        case HUB_REPORT_PACKAGE_LOST:
          if (idx < lost_len)
          {
            data_package.pack_id = lost_packages[idx];
            //app_uart_put(random_range(0, lost_packages[i] % 100));
            memset(&data_package.dat[0], random_range(0, lost_packages[idx] % 100), DATA_SIZE_SEND_LOG);
            uint16_t size = sizeof(data_package);
            send_chunk_data(data_package, &size);
            idx++;
          }
          else
          {
            NRF_LOG_INFO("Patch sent\r\n");
            //memset(lost_packages, NULL, 80 * sizeof(unsigned short));
            //*lost_packages-=idx;
            idx = 0;
            result = 0x00;
            uint16_t size = sizeof(eof);
            send_message((unsigned char *)&eof, &size);
          }
          break;
        case HUB_SET_TIME:
          general_ack.id = HUB_SET_TIME;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          NRF_LOG_INFO("Setting time\r\n");
          offset_timeStamp.l = m_rtc.p_reg->COUNTER/1024;
          NRF_LOG_INFO("Got time stamp: %llu",start_timeStamp.ll);
          struct tm taiTime;
          const time_t timeT = start_timeStamp.ll/1000;
          gmtime_r(&timeT,&taiTime);
          NRF_LOG_INFO("TIME ZONE: GMT %d",timeZone);
          local_time(&taiTime,timeZone);
          NRF_LOG_INFO("To TAI time: %d %d %d %d %d %d",taiTime.tm_year,taiTime.tm_mon+1,taiTime.tm_mday,(signed int)taiTime.tm_hour,taiTime.tm_min,taiTime.tm_sec);
          get_time_hr_to_sec((time_t *)&during_day_timeStamp.l,taiTime.tm_hour,taiTime.tm_min,taiTime.tm_sec);
          result = 0x00;
          break;
        case HUB_UPDATE_CREDENTIALS:
          general_ack.id = HUB_UPDATE_CREDENTIALS;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          NRF_LOG_INFO("Setting credentials\r\n");
          result = 0x00;
          break;
        case HUB_UNRECOGNIZED:
          NRF_LOG_INFO("Idk wtf this is\r\n");
          result = 0x00;
          break;
        case HUB_UPDATE_FIRMWARE:
          NRF_LOG_INFO("Switching to bootloader\r\n");
          general_ack.id = HUB_UPDATE_FIRMWARE;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          NRF_POWER->GPREGRET = BOOTLOADER_DFU_START;
          NVIC_SystemReset();
          break;
        case HUB_SETUP_DEVICE_TYPE:
          NRF_LOG_INFO("Device type set\r\n");
          general_ack.id = HUB_SETUP_DEVICE_TYPE;
          size = sizeof(ack);
          send_message((unsigned char *)&general_ack,&size);
          result=0x00;
          break;
        }
    }
    GulTicksAcc = m_rtc.p_reg->COUNTER*1000/1024;
    GulTicksAcc = GulTicksAcc/1000;
    if(check_work_period((time_t )GulTicksAcc,hr_start,hr_end,mnt_start,mnt_end))
    {
        //vTaskResume(m_ble_thread);
        NRF_LOG_INFO("Enter working time. Starting BLE Task");
        vTaskSuspend(NULL);
    }else{
          nrf_delay_ms(5);;
    }
    nrf_delay_ms(5);
  }

  /* Tasks must be implemented to never return... */
}


#ifndef __TIME_H__
#define __TIME_H__

#include "types.h"
#include "vbf_board.h"
/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};

/*
 * Structure defined by POSIX.1b to be like a timeval.
 */
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* and nanoseconds */
};

struct timezone {
  int tz_minuteswest;	/* minutes west of Greenwich */
  int tz_dsttime;	/* type of dst correction */
};

struct tm {
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year - 1900. */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};

struct t_hrminsec {
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
};

struct tm *get_formatted_time_sec(time_t time,struct tm *time_format);
struct tm *get_daily_time(time_t time, struct t_hrminsec *time_daily);
void get_time_sec_to_hr(time_t time, int *hr, int *mnt);
bool check_work_period(time_t currentTime, int hr_start, int hr_end, int mnt_start, int mnt_end);
void get_time_hr_to_sec(time_t *time,int hr, int mnt, int sec);

int gettimeofday(struct timeval *tp, void *ignore);
struct tm *gmtime_r(const time_t *timep, struct tm *r);

#endif




#include "time.h"

/* days per month -- nonleap! */
const short __spm[13] =
  { 0,
    (31),
    (31+28),
    (31+28+31),
    (31+28+31+30),
    (31+28+31+30+31),
    (31+28+31+30+31+30),
    (31+28+31+30+31+30+31),
    (31+28+31+30+31+30+31+31),
    (31+28+31+30+31+30+31+31+30),
    (31+28+31+30+31+30+31+31+30+31),
    (31+28+31+30+31+30+31+31+30+31+30),
    (31+28+31+30+31+30+31+31+30+31+30+31),
  };
static long int timezone;
static const char days[] = "Sun Mon Tue Wed Thu Fri Sat ";
static const char months[] = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";

/* seconds per day */
#define SPD 86400 //24*60*60

int __isleap(int year)
{
	/* every fourth year is a leap year except for century years that are
	 * not divisible by 400. */
	/*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

struct tm *gmtime_r(const time_t *timep, struct tm *r)
{
	time_t i;
	register time_t work = *timep % (SPD);
	r->tm_sec = work % 60;
	work /= 60;
	r->tm_min = work % 60;
	r->tm_hour = work / 60;
	work = *timep / (SPD);
	r->tm_wday = (4 + work) % 7;
	for (i = 1970;; ++i)
	{
		register time_t k = __isleap(i) ? 366 : 365;
		if (work >= k)
			work -= k;
		else
			break;
	}
	r->tm_year = i;
	r->tm_yday = work;

	r->tm_mday = 1;
	if (__isleap(i) && (work > 58))
	{
		if (work == 59)
			r->tm_mday = 2; /* 29.2. */
		work -= 1;
	}

	for (i = 11; i && (__spm[i] > work); --i)
		;
	r->tm_mon = i;
	r->tm_mday += work - __spm[i];
	return r;
}

struct tm* localtime_r(const time_t* t, struct tm* r)
{
	struct timezone tz = {0};
	time_t tmp;
	gettimeofday(0, &tz);
	timezone = tz.tz_minuteswest * 60L;
	tmp = *t + timezone;
	return gmtime_r(&tmp, r);
}

struct tm* localtime(const time_t* t)
{
	static struct tm tmp;
	return localtime_r(t, &tmp);
}

time_t mktime(struct tm * const t)
{
	register time_t day;
	register time_t i;
	register time_t years = t->tm_year - 70;

	if (t->tm_sec > 60)
	{
		t->tm_min += t->tm_sec / 60;
		t->tm_sec %= 60;
	}
	if (t->tm_min > 60)
	{
		t->tm_hour += t->tm_min / 60;
		t->tm_min %= 60;
	}
	if (t->tm_hour > 24)
	{
		t->tm_mday += t->tm_hour / 24;
		t->tm_hour %= 24;
	}
	if (t->tm_mon > 12)
	{
		t->tm_year += t->tm_mon / 12;
		t->tm_mon %= 12;
	}
	while (t->tm_mday > __spm[1 + t->tm_mon])
	{
		if (t->tm_mon == 1 && __isleap(t->tm_year + 1900))
		{
			--t->tm_mday;
		}
		t->tm_mday -= __spm[t->tm_mon];
		++t->tm_mon;
		if (t->tm_mon > 11)
		{
			t->tm_mon = 0;
			++t->tm_year;
		}
	}

	if (t->tm_year < 70)
		return (time_t) -1;

	/* Days since 1970 is 365 * number of years + number of leap years since 1970 */
	day = years * 365 + (years + 1) / 4;

	/* After 2100 we have to substract 3 leap years for every 400 years
	 This is not intuitive. Most mktime implementations do not support
	 dates after 2059, anyway, so we might leave this out for it's
	 bloat. */
	if (years >= 131)
	{
		years -= 131;
		years /= 100;
		day -= (years >> 2) * 3 + 1;
		if ((years &= 3) == 3)
			years--;
		day -= years;
	}

	day += t->tm_yday = __spm[t->tm_mon] + t->tm_mday - 1 +
			(__isleap(t->tm_year + 1900) & (t->tm_mon > 1));

	/* day is now the number of days since 'Jan 1 1970' */
	i = 7;
	t->tm_wday = (day + 4) % i; /* Sunday=0, Monday=1, ..., Saturday=6 */

	i = 24;
	day *= i;
	i = 60;
	return ((day + t->tm_hour) * i + t->tm_min) * i + t->tm_sec;
}

static void num2str(char *c, int i)
{
	c[0] = i / 10 + '0';
	c[1] = i % 10 + '0';
}

char *asctime_r(const struct tm *t, char *buf)
{
	/* "Wed Jun 30 21:49:08 1993\n" */
	*(int*) buf = *(int*) (days + (t->tm_wday << 2));
	*(int*) (buf + 4) = *(int*) (months + (t->tm_mon << 2));
	num2str(buf + 8, t->tm_mday);
	if (buf[8] == '0')
		buf[8] = ' ';
	buf[10] = ' ';
	num2str(buf + 11, t->tm_hour);
	buf[13] = ':';
	num2str(buf + 14, t->tm_min);
	buf[16] = ':';
	num2str(buf + 17, t->tm_sec);
	buf[19] = ' ';
	num2str(buf + 20, (t->tm_year + 1900) / 100);
	num2str(buf + 22, (t->tm_year + 1900) % 100);
	buf[24] = '\n';
	return buf;
}

char *asctime(const struct tm *timeptr)
{
	static char buf[25];
	return asctime_r(timeptr, buf);
}

char *ctime(const time_t *timep)
{
	return asctime(localtime(timep));
}

int gettimeofday(struct timeval *tp, void *ignore)
{
	time_t time = 0;

		//rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time);
		if (tp != NULL)
		{
			tp->tv_sec = time;
			tp->tv_usec = 0;
		}

		return time;
}
struct tm *get_formatted_time_sec(time_t time,struct tm *time_format)
{
    time_t temp;
    uint32_t seconds,minutes,hours,days,year,month;
    uint32_t dayOfWeek;
    uint32_t milliseconds = time%1000;
    seconds = time/1000;
    
    /* calculate minutes */
    minutes  = seconds / 60;
    seconds -= minutes * 60;
    /* calculate hours */
    hours    = minutes / 60;
    minutes -= hours   * 60;
    /* calculate days */
    days     = hours   / 24;
    hours   -= days    * 24;    /* Unix time starts in 1970 on a Thursday */
    year      = 1970;
    dayOfWeek = 4;

    while(1)
    {
        bool     leapYear   = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        uint16_t daysInYear = leapYear ? 366 : 365;
        if (days >= daysInYear)
        {
          dayOfWeek += leapYear ? 2 : 1;
          days      -= daysInYear;
          if (dayOfWeek >= 7)
            dayOfWeek -= 7;
          ++year;
        }
        else
        {
          time_format->tm_yday = days;
          dayOfWeek  += days;
          dayOfWeek  %= 7;

          /* calculate the month and day */
          static const uint8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
          for(month = 0; month < 12; ++month)
          {
            uint8_t dim = daysInMonth[month];

            /* add a day to feburary if this is a leap year */
            if (month == 1 && leapYear)
              ++dim;

            if (days >= dim)
              days -= dim;
            else
              break;
          }
          break;
        }
    }
    time_format->tm_sec = seconds;
    time_format->tm_min = minutes;
    time_format->tm_hour = hours;
    time_format->tm_mday = days+1;
    time_format->tm_mon = month;
    time_format->tm_year = year;
    time_format->tm_wday = dayOfWeek;

}

void get_time_hr_to_sec(time_t *time,int hr, int mnt, int sec)
{
    time_t temp =0;
    temp = hr*3600 +mnt*60+sec;
    *time = temp;
}
void get_time_sec_to_hr(time_t time, int *hr, int *mnt)
{
    int seconds, minutes, hours;
    seconds = time%60;
    time = time/60;
    minutes = time%60;
    time = time/60;
    hours = time;
    *hr = hours;
    *mnt = minutes;
}

struct tm *get_daily_time(time_t time, struct t_hrminsec *time_daily)
{
    struct tm time_format;
    get_time_sec_to_hr(time,&time_format.tm_hour,&time_format.tm_min);
    time_daily->tm_hour = time_format.tm_hour;
    time_daily->tm_min = time_format.tm_min;
    //time_daily->tm_sec = time_format.tm_sec;
}

bool check_work_period(time_t currentTime, int hr_start, int hr_end, int mnt_start, int mnt_end)
{
    struct t_hrminsec time_daily;
    get_daily_time(currentTime,&time_daily);
    if(hr_start<= time_daily.tm_hour && time_daily.tm_hour<=hr_end)
    {
        if(mnt_start<=time_daily.tm_min && time_daily.tm_min<=mnt_end)
        {
            return true;
        }else{
            return false;
        }        
    }else{
        return false;
    }
}    
