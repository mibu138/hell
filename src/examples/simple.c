#include <hell/hell.h>

int main(int argc, char *argv[])
{
    hell_Init(true, 0, 0);
    hell_OpenWindow(500, 500, NULL);
    hell_Loop();
    return 0;
}
