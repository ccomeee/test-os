#ifndef MOUSE_H
#define MOUSE_H

typedef struct {
    int x;
    int y;
    int left_button;
    int right_button;
} mouse_state_t;

void mouse_init();
void mouse_poll(mouse_state_t* state);

#endif // MOUSE_H
