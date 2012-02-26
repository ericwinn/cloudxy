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
#include "logger.h"

CTRL_REGION_T CTRL_REGION;
//extern int log_write_task(struct hlfs_ctrl * ctrl);

int read_fs_superblock(struct back_storage *storage,struct super_block *sb)
{
	HLOG_DEBUG("enter func %s", __func__);
	if ((NULL == storage) || (NULL == sb)) {
		HLOG_ERROR("read fs superblock error");
		return -1;
	}
    int ret = read_fs_meta(storage,&(sb->seg_size),&(sb->block_size),&(sb->max_fs_size));
    g_strlcpy(sb->fsname,g_basename(storage->uri),MAX_FILE_NAME_LEN);
	HLOG_DEBUG("leave func %s", __func__);
    return ret;
}

/*
 * init_hlfs - initial the hlfs
 * @param uri   fs localtion
 * @return Return a handle to the lhdfs
 */
struct hlfs_ctrl *
init_hlfs(const char *uri)
{
	HLOG_DEBUG("enter func %s", __func__);
    if(uri == NULL){
	HLOG_ERROR("uri is null");
      return NULL;  
    }
    int ret;
    g_thread_init(NULL);

    struct hlfs_ctrl *ctrl = (struct hlfs_ctrl*)g_malloc0(sizeof(struct hlfs_ctrl));
    if (NULL == ctrl) {
	    HLOG_ERROR("ctrl allocate error!");
	    return NULL;
    }
    ctrl->write_req_aqueue = g_async_queue_new();
    ctrl->write_rsp_aqueue = g_async_queue_new();
    HLOG_DEBUG("uri %s", uri);
#if 0
	g_message("run here");
#endif
    struct back_storage *storage = init_storage_handler(uri);
    if( storage == NULL){
        HLOG_ERROR("[uri:%s] can not accessable",uri);
        g_free(ctrl);
        return NULL;
    }
#if 0
	g_message("run here");
	g_message("storage name:%s, uri: %s", (char *)storage->storage_name, \
			storage->uri);
#endif

    HLOG_DEBUG("storage name:%s,uri %s\n",
    (char *) storage->storage_name,storage->uri);
    ctrl->storage = storage;
    if(0!= read_fs_superblock(ctrl->storage,&ctrl->sb)){
            HLOG_ERROR("[uri:%s] read superblock failed",uri);
            g_free(ctrl);
            ctrl = NULL;
            goto out;
    }

    HLOG_DEBUG("superblock read over\n");
    uint32_t segno=0;
    uint32_t offset = 0;
#if 0
	g_message("run here");
#endif

    if(0 != get_cur_latest_segment_info(ctrl->storage,&segno,&offset)){
        g_free(ctrl);
        ctrl = NULL;
        goto out;
    }
#if 0
	g_message("run here");
#endif

    ctrl->write_task_run = 1;
#if 0
	g_message("run here");
#endif
    GThread * log_write_thread = g_thread_create((GThreadFunc) log_write_task,ctrl,TRUE,NULL);
    ctrl->log_write_thread = log_write_thread;
    ctrl->ctrl_region = &CTRL_REGION;
    ctrl->hlfs_access_mutex = g_mutex_new();

    HLOG_DEBUG("segno %d,offset %d\n",segno,offset);
    ctrl->last_segno = segno;
    ctrl->last_offset = offset;
    if(ctrl->last_segno != 0 || ctrl->last_offset != 0){
        if(0!=load_latest_inode_map_entry(ctrl->storage,ctrl->last_segno,ctrl->last_offset,&ctrl->imap_entry)){
            HLOG_ERROR("load inode map entry failed");
            ret = -1;
            goto out;
        }
    }
#if 0
	g_message("inode_addr : %d", ctrl->imap_entry.inode_addr);
#endif
out:
	HLOG_DEBUG("leave func %s", __func__);
    return ctrl;
} 