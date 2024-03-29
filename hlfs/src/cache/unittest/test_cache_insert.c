/*
 * Unit test for cache insert;
 * Copyright (c) XUPT
 * By Kelvin Wang<senwang@linux.vnet.ibm.com> (c) 2012
 */

#include <glib.h>
#include <stdlib.h>
#include "cache.h"
#include "cache_helper.h"
#include "comm_define.h"
#include "api/hlfs.h"

#define BLOCK_SIZE 8192
typedef struct { 
	CACHE_CTRL *cache_ctrl;
} Fixture;

int my_cb(void *user_data, char *buf, uint32_t start_no, uint32_t end_no)
{
	return 0;
}

Fixture fixture;
void case_setup()
{ 
	int ret;
	g_message("--enter fun %s", __func__);
	fixture.cache_ctrl = dbcache_new();
	g_assert(fixture.cache_ctrl != NULL);
	ret = dbcache_init(fixture.cache_ctrl, BLOCK_SIZE, 1024, 10, 80, 100);
	g_assert(ret == 0);
}
/*  base insert  */
void test_cache_insert_1()
{
	g_message("--enter fun %s", __func__);
	int ret = 0;
	char *_block_buf = NULL;
	_block_buf = (char *)g_malloc0(BLOCK_SIZE);
	sprintf(_block_buf, "hello cache mine");
	ret = dbcache_insert_block(fixture.cache_ctrl, 1, _block_buf);
	printf("ret is :%d\n", ret);
	g_assert(ret == 0);
	sprintf(_block_buf, "hello cache you");
	ret = dbcache_insert_block(fixture.cache_ctrl, 2, _block_buf);
	printf("ret is :%d\n", ret);
	g_assert(ret == 0);
	sprintf(_block_buf, "hello cache him");
	ret = dbcache_insert_block(fixture.cache_ctrl, 4, _block_buf);
	g_assert(ret == 0);

	/* test cache free size  */
	g_assert(get_cache_free_size(fixture.cache_ctrl) == 1024 - 3);
	/* test get continue blocks  */
	GSList *continue_block_list = NULL;
	get_continues_blocks(fixture.cache_ctrl, &continue_block_list);
	uint32_t con_blocks_count = g_slist_length(continue_block_list);
	printf("con block count:%d\n", con_blocks_count);
	g_assert(con_blocks_count == 2);
	/* test free from cache blocks  */
	free_from_cache(fixture.cache_ctrl, continue_block_list);
	g_slist_free(continue_block_list);
	/* test get continue blocks again  */
	g_assert(get_cache_free_size(fixture.cache_ctrl) == 1024 - 1);
	int i = 0;
	for (i = 5; i < 200; i++){
		ret = dbcache_insert_block(fixture.cache_ctrl, i, _block_buf);
		g_assert(ret == 0);
	}
	continue_block_list = NULL;
	get_continues_blocks(fixture.cache_ctrl, &continue_block_list);
	con_blocks_count = g_slist_length(continue_block_list);
	printf("con block count:%d\n", con_blocks_count);
	g_assert(con_blocks_count == 100);
	/* test trigger level + */
	dbcache_set_write_cb(fixture.cache_ctrl, my_cb, NULL);
	for (i = 0; i < 1024 * 90 / 100; i++) {
		ret = dbcache_insert_block(fixture.cache_ctrl, i, _block_buf);
		g_message("insert_block:%d", i);
		g_assert(ret == 0);
	}
	g_free(_block_buf);
}

void case_teardown()
{
	if (0 > dbcache_destroy(fixture.cache_ctrl)) {
		g_message("destroy cache error");
		return;
	}	
}

int main(int argc, char **argv){
	if (log4c_init()) {
		g_message("log4c init error!");
	}
	g_test_init(&argc, &argv, NULL);
	g_test_add("/misc/cache_insert_1", Fixture, NULL, case_setup, \
			test_cache_insert_1, case_teardown);

	return g_test_run();
}
