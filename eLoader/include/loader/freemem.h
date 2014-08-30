#ifndef LOADER_FREEMEM
#define LOADER_FREEMEM

void preload_free_game_memory();

/* Free memory allocated by The game */
/* Warning: MUST NOT be called from the game's main thread */
void free_game_memory();
#endif
