#include<iostream>
#include<tool_helpful.h>

th_mempool *th_mempool_create(int32_t size)
{
	uint8_t *prawbuf = NULL;
	size += sizeof(th_mempool);
	size = _V_ALIGN_WORD(size);

	prawbuf = new uint8_t[size]{};
	if (prawbuf)
	{
		th_mempool *pmpool = (th_mempool *)&prawbuf[0];
		pmpool->prawbuf = &prawbuf[sizeof(th_mempool)];
		pmpool->pendbuf = prawbuf + size;
		pmpool->pcurptr = pmpool->prawbuf;
		return pmpool;
	}

	return NULL;
}

void th_mempool_config_raw(th_mempool *pmempool, uint8_t *praw, int32_t len)
{
	pmempool->psurbuf = praw;
	pmempool->prawbuf = praw;
	pmempool->pendbuf = praw + len;
	pmempool->pcurptr = praw;
}

void th_mempool_free(th_mempool *pmempool)
{
	if (!pmempool->psurbuf)
		free(pmempool);
}

void th_mempool_reset(th_mempool *pmempool)
{
	pmempool->pcurptr = pmempool->prawbuf;
}

uint8_t *th_mempool_malloc(th_mempool *pmempool, int32_t size)
{
	uint8_t *pnext = pmempool->pcurptr;
	size = _V_ALIGN_WORD(size);
	pnext += size;
	if (pnext > pmempool->pendbuf)
	{
		std::cout << "qmempool buffer overflow" << std::endl;
	}
	else
	{
		uint8_t *ptr = pmempool->pcurptr;
		memset(ptr, 0, size);
		pmempool->pcurptr = pnext;
		return ptr;
	}
	return NULL;
}

int32_t th_mempool_get_pointer(th_mempool *pmempool, uint8_t **ptrbuf)
{
	int32_t size = (int32_t)(pmempool->pendbuf - pmempool->pcurptr);
	*ptrbuf = pmempool->pcurptr;

	return size;
}
