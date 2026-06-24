/* MALLOC & FREE */
#include <sys/types.h>
#include <sys/malloc.h>
/* ATOMIC_FETCHADD_XXX */
#include <atomic.h>
#include <threads.h>

#include "session.h"
#include "sys/queue.h"



void session_dtr(void *data) {
    struct session *session = (struct session *)data;

    struct data_block *cursor, *temp;

    LIST_FOREACH_SAFE(cursor, &session->data_blocks, entries, temp) {
        LIST_REMOVE(cursor, entries);
        free(cursor, M_CCCDEV);
    }
}

struct session *create_new_session() {
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

struct session *get_session() {
    struct session *session;
    int error;

    error = devfs_get_cdevpriv((void **)&session);
    if (error != 0)
        return (NULL);

    return (session);
}

int uio_to_session(struct uio *uio, struct session *session) {
    int error;
    struct data_block *prev_block = NULL;

    /* Delete all unused session block data */
    session_dtr(session);

    mtx_lock(&session->block_mtx);
    while (uio->uio_resid != 0) {
        ssize_t bytes_to_process;

        struct data_block *block = malloc(sizeof(*block), M_CCCDEV, M_WAITOK | M_ZERO);
        if (LIST_EMPTY(&session->data_blocks))
            LIST_INSERT_HEAD(&session->data_blocks, block, entries);
        else
            LIST_INSERT_AFTER(prev_block, block, entries);

        block->block_size = MIN(uio->uio_resid, MAX_BLOCK_SIZE);
        error = uiomove(block->data, block->block_size, uio);
        if (error != 0) {
            session_dtr(session);
            return (error);
        }

        prev_block = block;
    }


    mtx_unlock(&session->block_mtx);

    return (0);
}

int session_to_uio(struct session *session, struct uio *uio) {
    int error;

    mtx_lock(&session->block_mtx);
    struct data_block *block = LIST_FIRST(&session->data_blocks);

    if (block == NULL)
        return (0);

    error = uiomove(block->data, block->block_size, uio);
    LIST_REMOVE(block, entries);

    mtx_unlock(&session->block_mtx);

    return (error);
}
