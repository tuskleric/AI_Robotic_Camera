typedef enum {
    STATE_DISABLE,
    STATE_SEARCH,
    STATE_ROTATE,
    STATE_TRACK,
} State_t;

typedef enum {
    PI_ON,
    PI_OFF,
    TARGET_FOUND,
    TARGET_LOST,
    ANGLE_REACHED,
    TIME_REACHED,
} Event_t;

State_t getState(void);
void update_state(Event_t event);