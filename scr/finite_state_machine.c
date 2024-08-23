#include "finite_state_machine.h"

State_t mainState = STATE_DISABLE;

void update_state(Event_t event) {
    switch (mainState) {
        case STATE_DISABLE:
            if (event == PI_ON) {
                mainState = STATE_SEARCH;
            }
            break;
        case STATE_SEARCH:
            switch (event) {
                case PI_OFF:
                    mainState = STATE_DISABLE;
                    break;
                case TARGET_FOUND:
                    mainState = STATE_TRACK;
                    break;
                case TIME_REACHED:
                    mainState = STATE_ROTATE;
                default:
                    break;
            }
            break;
        case STATE_ROTATE:
            switch (event) {
                case PI_OFF:
                    mainState = STATE_DISABLE;
                    break;
                case ANGLE_REACHED:
                    mainState = STATE_SEARCH;
                default:
                    break;
            }
            break;
        case STATE_TRACK:
            switch (event) {
                case PI_OFF:
                    mainState = STATE_DISABLE;
                    break;
                case TARGET_LOST:
                    mainState = STATE_SEARCH;
                    break;
                default:
                    break;
            }
            break;
    }
}