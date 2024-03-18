#include<iostream>
#include<tool_helpful.h>

th_circlebuf *th_circlebuf_create(int32_t slicelen, int32_t nums)
{
	int32_t len = _V_ALIGN_WORD(sizeof(th_circlebuf) + (slicelen * nums));
	uint8_t *praw = new uint8_t[len];

	if (praw)
	{
		th_circlebuf *pcbuf = (th_circlebuf *)&praw[0];
		memset(pcbuf, 0, len);
		pcbuf->prawbuf = &praw[sizeof(th_circlebuf)];
		pcbuf->slicenums = nums;
		pcbuf->slicetotal = slicelen * nums;
		pcbuf->slicelen = slicelen;
		return pcbuf;
	}
	return NULL;
}

th_circlebuf *th_circlebuf_create_pool(th_mempool *pmempool, int32_t slicelen, int32_t nums)
{
	int32_t len = _V_ALIGN_WORD(sizeof(th_circlebuf) + (slicelen * nums));
	uint8_t *praw = (uint8_t *)th_mempool_malloc(pmempool, len);
	if (praw)
	{
		th_circlebuf *pcbuf = (th_circlebuf *)&praw[0];
		pcbuf->prawbuf = &praw[sizeof(th_circlebuf)];
		pcbuf->slicenums = nums;
		pcbuf->slicetotal = slicelen * nums;
		pcbuf->slicelen = slicelen;
		pcbuf->pmempool = pmempool;
		return pcbuf;
	}
	return NULL;
}

void th_circlebuf_free(th_circlebuf *pcirclebuf)
{
	if (!pcirclebuf->pmempool)
		free(pcirclebuf);
}

void th_circlebuf_reset(th_circlebuf *pcirclebuf)
{
	pcirclebuf->slicevalid = 0;
	pcirclebuf->nwptr = 0;
	pcirclebuf->nrptr = 0;
}

uint8_t *th_circlebuf_get_wptr(th_circlebuf *pcirclebuf)
{
	if (pcirclebuf->slicevalid < pcirclebuf->slicenums)
		return &pcirclebuf->prawbuf[pcirclebuf->nwptr];
	return NULL;
}

void th_circlebuf_commit(th_circlebuf *pcirclebuf)
{
	pcirclebuf->nwptr += pcirclebuf->slicelen;
	if (pcirclebuf->nwptr == pcirclebuf->slicetotal)pcirclebuf->nwptr = 0;
	pcirclebuf->slicevalid++;
	if (pcirclebuf->slicevalid > pcirclebuf->slicenums)
	{
		std::cout << "circlebuffer overflow" << std::endl;
	}
}

uint8_t *th_circlebuf_get_rptr(th_circlebuf *pcirclebuf)
{
	uint8_t *pbuf = NULL;
	if (pcirclebuf->slicevalid)
		pbuf = &pcirclebuf->prawbuf[pcirclebuf->nrptr];
	return pbuf;
}

void th_circlebuf_finish(th_circlebuf *pcirclebuf)
{
	pcirclebuf->nrptr += pcirclebuf->slicelen;
	if (pcirclebuf->nrptr == pcirclebuf->slicetotal)pcirclebuf->nrptr = 0;
	pcirclebuf->slicevalid--;
	if (pcirclebuf->slicevalid < 0)
	{
		std::cout << "circlebuffer underflow" << std::endl;
	}
}
