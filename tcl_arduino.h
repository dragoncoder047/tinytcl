#include "tinytcl.h"
#include <Arduino.h>

tcl_result_t tcl_cmd_pin(struct tcl *tcl, tcl_value_t *args, void *arg) {
    (void)arg;
    tcl_value_t *action = tcl_list_at(args, 1);
    tcl_value_t *tnumber = tcl_list_at(args, 3);
    int number = (int)tcl_num(tnumber);
    tcl_value_t *value = tcl_list_at(args, 2);
    tcl_value_t *result;
    tcl_result_t r = TCL_OK;
    // pin mode inputmode N
    if (strcmp(tcl_string(action), "mode") == 0) {
        int m;
        if (strcmp(tcl_string(value), "-i") == 0) m = INPUT;
        else if (strcmp(tcl_string(value), "-o") == 0) m = OUTPUT;
        else if (strcmp(tcl_string(value), "-iu") == 0) m = INPUT_PULLUP;
#ifdef INPUT_PULLDOWN
        else if (strcmp(tcl_string(value), "-id") == 0) m = INPUT_PULLDOWN;
#endif
        else m = INPUT;
        pinMode(number, m);
    // pin read analog|digital N
    } else if (strcmp(tcl_string(action), "read") == 0) {
        int value;
        if (strcmp(tcl_string(value), "-d") == 0) value = digitalRead(number);
        else if (strcmp(tcl_string(value), "-a") == 0) value = analogRead(number);
#ifdef touchRead
        else if (strcmp(tcl_string(value), "-t") == 0) value = touchRead(number);
#endif
        char buf[16];
        sprintf(buf, "%d", value);
        result = tcl_alloc(buf, strlen(buf));
    // pin write analog|digital N value
    } else if (strcmp(tcl_string(action), "write") == 0) {
        tcl_value_t *tval = tcl_list_at(args, 4);
        int out = (int)tcl_num(tval);
        if (strcmp(tcl_string(value), "-d") == 0) {
            if (strcmp(tcl_string(tval), "high") == 0) out = HIGH;
            else if (strcmp(tcl_string(tval), "low") == 0) out = LOW;
            digitalWrite(number, out);
        }
        else if (strcmp(tcl_string(value), "-a") == 0) analogWrite(number, out);
        tcl_free(tval);
    } else {
        result = tcl_alloc("pin what?", 9);
        r = TCL_ERROR;
    }
    tcl_free(action);
    tcl_free(tnumber);
    tcl_free(value);
    return tcl_result(tcl, r, result ? result : tcl_alloc("", 0));
}

void tcl_init_arduino(struct tcl *tcl) {
    tcl_register(tcl, "pin", tcl_cmd_pin, 0);
}