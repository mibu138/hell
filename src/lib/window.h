#ifndef HELL_WINDOW_H
#define HELL_WINDOW_H


typedef struct Hell_Window Hell_Window;
typedef struct Hell_EventQueue Hell_EventQueue;

const Hell_Window* hell_CreateWindow(const unsigned short width, const unsigned short height, const char* name, Hell_Window* window);
void               hell_DestroyWindow(Hell_Window*);
// does nothing if window is not active
void               hell_DrainWindowEvents(Hell_EventQueue*, Hell_Window*);

#endif /* end of include guard: DISPLAY_H */
