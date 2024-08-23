typedef enum {
    STATE_DISABLED,
    STATE_SEARCH,
    STATE_ROTATE,
    STATE_TRACK,
} State_t;

typedef enum {
    TARGET_FOUND,
    TARGET_LOST,
} Event_t;