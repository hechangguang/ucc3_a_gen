/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#include <stdio.h>
#include <cpl/ts_cpl.h>


static ts_cpl_mutex_s_t vot_elog_mutex;

static ts_cpl_timeval_s_t  vot_elog_st_time_start; 
/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
    ts_cpl_mutex_init(&vot_elog_mutex);
    ts_cpl_gettimeofday(&vot_elog_st_time_start);
    
    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

    /* add your code here */
    ts_cpl_mutex_destory(&vot_elog_mutex);
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
    /* add your code here */
    
    printf("%.*s",(int)size,log);
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
    // ts_cpl_mutex_lock(&vot_elog_mutex);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */
    // ts_cpl_mutex_unlock(&vot_elog_mutex);
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    
    static char time_str_buf[64];
    /* add your code here */
    ts_cpl_timeval_s_t  st_time_cur;   
    ts_cpl_gettimeofday(&st_time_cur);

    ts_u32 ts = ts_cpl_get_timedif(&vot_elog_st_time_start, &st_time_cur);
    ts = ts/1000;
    // snprintf(time_str_buf,64,"%u.%u",st_time_cur.tv_sec,st_time_cur.tv_usec);
    snprintf(time_str_buf,64,"%u",ts);
    return &time_str_buf[0];

}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
    return "";
    
}