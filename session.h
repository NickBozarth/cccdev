/* LIST INCLUDES */
#include <sys/queue.h>
/* MUTEX INCLUDES */
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
/* DEVFS_XXX_CDEVPRIV */
// #include <sys/param.h>
#include <sys/conf.h>
/* UIO */
#include <sys/types.h>
#include <sys/uio.h>
/* INT TYPE DEFS */
#include <sys/systm.h>

#include "settings.h"



/*
 * TODO REMOVE ME BEFORE PROD
 * Its a catch-all that helps my lsp
 */
#ifndef M_CCCDEV

#include <sys/kernel.h>
#include <sys/malloc.h>
MALLOC_DECLARE(M_CCCDEV);
MALLOC_DEFINE(M_CCCDEV, "malloc_cccdev", "malloc c crypto character device");

#endif /* M_CCCDEV */




#define MAX_BLOCK_SIZE 512


/* BLOCKS THAT ARE INTERACTED WITH BY ALGORITHMS */
struct data_block {
    char data[MAX_BLOCK_SIZE];
    size_t block_size;
    int is_processed;

    LIST_ENTRY(data_block) entries;
};
LIST_HEAD(data_block_head, data_block);


/* 
 * When a open call is made to the cdev, a session is created
 * Sessions hold data about algorithms and the data associated
 *  with the r/w calls
 *
* open  -> creates session
 * write -> writes data to session
 * read  -> reads data once processed by session
 * close -> clears all data and closes session
 */
struct session {
    uint64_t session_id;
    struct data_block_head data_blocks;
    struct settings settings;

    struct mtx block_mtx;
};





void session_dtor(void *);
struct session *create_new_session();
struct session *get_session();

int uio_to_session(struct uio *, struct session *);
int session_to_uio(struct session *, struct uio *);
