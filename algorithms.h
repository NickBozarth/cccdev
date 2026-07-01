#ifndef ALGORITHMS
#define ALGORITHMS

#include "cccdev_settings.h"
#include "session.h"
#include <sys/queue.h>



static void caesar(struct session *session) {
    struct data_block *block;
    int direction = (session->settings.function == FUNC_ENCRYPT) ? 1 : -1;
    int rot = direction * session->settings.settings.caesar.rot;
    mtx_lock(&session->block_mtx);

    int i = 0;

    LIST_FOREACH(block, &session->data_blocks, entries) {
        if (!block->is_processed)
            continue;

        for (int i = 0; i < block->block_size; i++) {
            int alpha_offset = 0;
            char current_char = block->data[i];
            if (current_char >= 'a' && current_char <= 'z')
                alpha_offset = 'a';
            if (current_char >= 'A' && current_char <= 'Z')
                alpha_offset = 'Z';

            if (alpha_offset == 0)
                continue;

            block->data[i] = (current_char - alpha_offset + rot) % 26 + alpha_offset;
            uprintf("OLD -> NEW [%c -> %c]\n", current_char, block->data[i]);
        }

        block->is_processed = 1;

        uprintf("block #%i [%.*s]\n", i, (int)block->block_size, block->data);
    }

    mtx_unlock(&session->block_mtx);
}


#endif /* ALGORITHMS */
