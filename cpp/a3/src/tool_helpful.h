#ifndef _TOOL_HELPFUL_
#define _TOOL_HELPFUL_

#include<vatek_base.h>

#define _V_ALIGN_WORD(n)	(((n + 3) >> 2)<< 2)

struct th_mempool
{
	uint8_t *psurbuf;
	uint8_t *prawbuf;
	uint8_t *pendbuf;
	uint8_t *pcurptr;
};

typedef struct _th_fifo
{
	th_mempool *pmempool;
	int32_t len;
	int32_t pos;
	void **fifo_list;
}th_fifo, *Pth_fifo;

struct th_circlebuf
{
	th_mempool *pmempool;
	uint8_t *prawbuf;
	int32_t slicetotal;
	int32_t slicelen;
	int32_t slicenums;
	int32_t slicevalid;
	uint32_t nwptr;
	uint32_t nrptr;
};

/* simple fifo */
Pth_fifo th_fifo_create_pool(int32_t len, th_mempool *pmpool);
Pth_fifo th_fifo_create(int32_t len);
void th_fifo_reset(Pth_fifo pfifo);
void th_fifo_free(Pth_fifo pfifo);
void th_fifo_push(Pth_fifo pfifo, void *ptrobj);
void *th_fifo_pop(Pth_fifo pfifo);
void *th_fifo_peek(Pth_fifo pfifo);

/* simple memory pool */
void th_mempool_config_raw(th_mempool *pmempool, uint8_t *praw, int32_t len);
th_mempool *th_mempool_create(int32_t size);
void th_mempool_free(th_mempool *pmempool);
void th_mempool_reset(th_mempool *pmempool);
uint8_t *th_mempool_malloc(th_mempool *pmempool, int32_t size);
int32_t th_mempool_get_pointer(th_mempool *pmempool, uint8_t **ptrbuf);

/* circle buffer */
th_circlebuf *th_circlebuf_create_pool(th_mempool *pmempool, int32_t slicelen, int32_t nums);
th_circlebuf *th_circlebuf_create(int32_t slicelen, int32_t nums);
void th_circlebuf_free(th_circlebuf *pcirclebuf);
void th_circlebuf_reset(th_circlebuf *pcirclebuf);

uint8_t *th_circlebuf_get_wptr(th_circlebuf *pcirclebuf);
void th_circlebuf_commit(th_circlebuf *pcirclebuf);

uint8_t *th_circlebuf_get_rptr(th_circlebuf *pcirclebuf);
void th_circlebuf_finish(th_circlebuf *pcirclebuf);

#endif
