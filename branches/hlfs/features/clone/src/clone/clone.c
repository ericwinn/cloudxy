/*
  *  Copyright (C) 2012 KangHua <kanghua151@gmail.com>
  *
  *  This program is free software; you can redistribute it and/or modify it
  *  under the terms of the GNU General Public License version 2 as published by
  *  the Free Software Foundation.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <glib.h>
#include "hlfs_ctrl.h"
#include "hlfs_log.h"
#include "misc.h"
#include "comm_define.h"
#include "storage.h"
#include "storage_helper.h"
#include "bs_local.h"
#include "seg_clean.h"
#include "logger.h"
#include "clone.h"

FAMILY_CTRL * family_new(){
    return NULL;
}
int faimly_init(FAMILY_CTRL *family_ctrl,char* father_uri,uint64_t base_inode,uint32_t from_segno){
	return 0;
}

struct back_storage * get_parent_storage(FAMILY_CTRL *fctrl, uint32_t segno){
           struct  back_storage * storage = NULL;
	    return storage;
}

int family_destroy(FAMILY_CTRL *fctrl){
    return 0;
}