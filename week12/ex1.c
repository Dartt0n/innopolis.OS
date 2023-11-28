#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#define KEYBOARD_DEVICE "/dev/input/by-path/platform-i8042-serio-0-event-kbd"
// #define KEYBOARD_DEVICE "/dev/input/by-path/pci-0000:00:04.0-usb-0:3:1.0-event-kbd"

#define EVENT_SIZE sizeof(struct input_event)

#define KEYBOARD_PRESSED 1
#define KEYBOARD_RELEASED 0
#define KEYBOARD_REPEATED 2

int main()
{
    struct input_event event;

    int fd = open(KEYBOARD_DEVICE, O_RDONLY);
    if (fd == -1)
    {
        printf("failed to open device\n");
        return EXIT_FAILURE;
    }

    char IS_PRESSED[256];       // array of pressed buttons
    memset(IS_PRESSED, 0, 256); // set all to false

    printf("Available shortcuts\n");
    printf("- X+E   - exit\n");
    printf("- P+E   - print message about exam\n");
    printf("- C+A+P - print message about coffee\n");
    printf("- C+V   - custom shortcut\n");

    for (read(fd, &event, EVENT_SIZE); 1; read(fd, &event, EVENT_SIZE))
    {
        if (event.type != EV_KEY) // process only keyboard events
        {
            continue;
        }

        const char *press_type;

        if (event.value == KEYBOARD_PRESSED)
        {
            press_type = "PRESSED";
            IS_PRESSED[event.code] = 1;
        }
        else if (event.value == KEYBOARD_RELEASED)
        {
            press_type = "RELEASED";
            IS_PRESSED[event.code] = 0;
        }
        else if (event.value == KEYBOARD_REPEATED)
        {
            press_type = "REPEATED";
        }

        printf("%s 0x%#04x (%d)\n", press_type, event.code, event.code);

        char keys_pressed = 0;
        for (int i = 0; i < 256; i++)
        {
            keys_pressed += IS_PRESSED[i];
        }

        if (keys_pressed == 2 && IS_PRESSED[KEY_X] && IS_PRESSED[KEY_E])
        {
            break;
        }

        if (keys_pressed == 2 && IS_PRESSED[KEY_P] && IS_PRESSED[KEY_E])
        {
            printf("I passed the Exam!\n");
        }

        if (keys_pressed == 3 && IS_PRESSED[KEY_C] && IS_PRESSED[KEY_A] && IS_PRESSED[KEY_P])
        {
            printf("Get some cappuccino!\n");
        }

        if (keys_pressed == 2 && IS_PRESSED[KEY_C] && IS_PRESSED[KEY_V])
        {
            printf("Copy-pasta!\n");
        }
    }

    return EXIT_SUCCESS;
}