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

#include <machine/atomic.h>


#include "cccdev_settings.h"



/*
 * TODO REMOVE ME BEFORE PROD
 * Its a catch-all that helps my lsp
 */
#ifdef _LSP

#include <sys/kernel.h>
#include <sys/malloc.h>
MALLOC_DECLARE(M_CCCDEV);
MALLOC_DEFINE(M_CCCDEV, "malloc_cccdev", "malloc c crypto character device");

#endif /* _LSP */




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









static void session_dtr(void *data) {
    struct session *session = (struct session *)data;

    struct data_block *cursor, *temp;

    LIST_FOREACH_SAFE(cursor, &session->data_blocks, entries, temp) {
        LIST_REMOVE(cursor, entries);
        free(cursor, M_CCCDEV);
    }
}
static struct session *create_new_session(void) {
    static uint64_t static_session_counter;

    struct session *session = malloc(
        sizeof(*session), 
        M_CCCDEV, 
        M_WAITOK | M_ZERO
    );

    LIST_INIT(&session->data_blocks);
    session->session_id = atomic_fetchadd_64(&static_session_counter, 1);
    mtx_init(&session->block_mtx, "cccdev_session_mtx", NULL, MTX_DEF);

    devfs_set_cdevpriv(session, session_dtr);
    return (session);
}
static struct session *get_session(void) {
    struct session *session;
    int error;

    error = devfs_get_cdevpriv((void **)&session);
    if (error != 0)
        return (NULL);

    return (session);
}

static int uio_to_session(struct uio *uio, struct session *session) {
    int error;
    struct data_block *prev_block = NULL;

    /* Delete all unused session block data */
    session_dtr(session);

    mtx_lock(&session->block_mtx);
    while (uio->uio_resid != 0) {
        struct data_block *block = malloc(sizeof(*block), M_CCCDEV, M_WAITOK | M_ZERO);
        if (prev_block == NULL)
            LIST_INSERT_HEAD(&session->data_blocks, block, entries);
        else
            LIST_INSERT_AFTER(prev_block, block, entries);

        block->block_size = MIN(uio->uio_resid, MAX_BLOCK_SIZE);
        error = uiomove(block->data, block->block_size, uio);
        if (error != 0) {
            uprintf("There was an error uiomove\n");
            session_dtr(session);
            return (error);
        }

        uprintf("Moved data [%.*s]\n", (int)block->block_size, block->data);

        prev_block = block;
    }
    mtx_unlock(&session->block_mtx);

    return (0);
}
static int session_to_uio(struct session *session, struct uio *uio) {
    int error;

    mtx_lock(&session->block_mtx);
    struct data_block *block = LIST_FIRST(&session->data_blocks);

    struct data_block *cursor;
    int count = 0;
    LIST_FOREACH(cursor, &session->data_blocks, entries) {
        count++;
    }

    uprintf("DATA BLOCK SIZE: [%i]\n", count);

    if (block == NULL)
        return (0);

    error = uiomove(block->data, block->block_size, uio);
    LIST_REMOVE(block, entries);

    mtx_unlock(&session->block_mtx);

    return (error);
}
