/* LIST INCLUDES */
#include <sys/queue.h>
/* MUTEX INCLUDES */
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>

#include <sys/systm.h>

#include "settings.h"


#define MAX_BLOCK_SIZE 512


/* BLOCKS THAT ARE INTERACTED WITH BY ALGORITHMS */
struct data_block {
    char data[MAX_BLOCK_SIZE];
    size_t block_size;
    int is_processed;
    struct mtx block_mutex;

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
    struct mtx session_mutex;

    LIST_ENTRY(session) entries;
};
LIST_HEAD(session_list_head, session);
static struct session_list_head session_list;
static struct mtx global_session_mutex;




