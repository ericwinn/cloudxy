/*
 *  hlfs_close.c
 *  Kanghua <kanghua151@msn.com> (C) 2011
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <glib.h>
#include "hlfs_ctrl.h"
#include "hlfs_log.h"
#include "comm_define.h"
#include "misc.h"
#include "logger.h"
#include "cache.h"
/*
 * hlfs_close: close a file. s
 * @param ctrl: The global control structure.
 * return: 0 is returned on success, else -1 is returned.
 */
int hlfs_close(struct hlfs_ctrl *ctrl){
    HLOG_DEBUG("enter func:%s",__func__);
    if (NULL == ctrl) {
	    HLOG_ERROR("hlfs_close error!");
	    return -1;
    }
    int ret =0;
    if(ctrl->cur_write_file_handler!=NULL){
       ret = ctrl->storage->bs_file_close(ctrl->storage,(bs_file_t)ctrl->cur_write_file_handler);
	   ctrl->cur_write_file_handler = NULL;
    }
    if(ctrl->cur_read_file_handler!=NULL){
       ret = ctrl->storage->bs_file_close(ctrl->storage,(bs_file_t)ctrl->cur_read_file_handler);
	   ctrl->cur_read_file_handler = NULL;
    }
    ctrl->usage_ref--;
    if(ctrl->cctrl!=NULL){
       HLOG_DEBUG("before close hlfs,sync all dirty block");
       cache_sync(ctrl->cctrl); 
    }
    HLOG_DEBUG("leave func:%s",__func__);
    return ret;
}        
