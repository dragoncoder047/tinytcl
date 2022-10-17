#include "tinytcl.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

/* File Identifier format:
SDCard File: 0x1C + 4 bytes pointer to file object
Serial port: 0x11 + port number
SPI port:    0x12

*/

static tcl_result_t tcl_cmd_puts(struct tcl *tcl, tcl_value_t *args, void *arg) {
    (void)arg;
    tcl_value_t *text;
    uintptr_t fp;
    int portnum = 0;
    bool newline = true;
    i = 1;
    File f;
    text = tcl_list_at(args, i);
    if (strcmp(text, "-nonewline") == 0) {
        newline = false;
        i++;
        tcl_free(text);
        text = tcl_list_at(args, i);
    }
    if (text[0] == 0x1C) { // ASCII file separator ==> file pointer
        fp = (uintptr_t)*(text + 1);
        f = (File)*fp; // pointers
        tcl_free(text);
        text = tcl_list_at(args, i + 1);
        if (newline) f.println(tcl_string(text));
        else f.print(tcl_string(text));
    }
    else if (text[0] == 0x11) { // ASCII Device Control 1 ==> serial port
        portnum = text[1] - '0';
        tcl_free(text);
        text = tcl_list_at(args, i + 1);
        if (portnum == 1) {
            if (newline) Serial1.println(tcl_string(text));
            else Serial1.print(tcl_string(text));
        }
        else { // portnum == 0
            if (newline) Serial.println(tcl_string(text));
            else Serial.print(tcl_string(text));
        }
    }
    else if (text[0] == 0x12) { // ASCII Device Control 2 ==> SPI port
        tcl_free(text);
        text = tcl_list_at(args, i + 1);
        SPI.transfer(tcl_string(text), tcl_length(text));
    }
    else {
        // default to serial
        if (newline) Serial.println(tcl_string(text));
        else Serial.print(tcl_string(text));
    }
    tcl_free(text);
    return tcl_result(tcl, TCL_OK, text);
}

static tcl_result_t tcl_cmd_open(struct tcl *tcl, tcl_value_t *args, void *arg) {
    tcl_value_t *filename = tcl_list_at(args, 1);
    if (strcmp(filename, "/dev/serial") == 0 || strcmp(filename, "/dev/serial0") == 0) {
        tcl_value_t *bauds = tcl_list_at(args, 2);
        int baud = (int)tcl_num(bauds);
        if (baud == 0) baud = 9600;
        Serial.begin(baud);
        tcl_free(bauds);
        tcl_free(filename);
        return tcl_result(tcl, TCL_OK, tcl_alloc("\x11\x30", 2)); // \x30 is ASCII '0'
    }
    if (strcmp(filename, "/dev/serial1") == 0) {
        tcl_value_t *bauds = tcl_list_at(args, 2);
        int baud = (int)tcl_num(bauds);
        if (baud == 0) baud = 9600;
        Serial1.begin(baud);
        tcl_free(bauds);
        tcl_free(filename);
        return tcl_result(tcl, TCL_OK, tcl_alloc("\x11\x31", 2)); // \x31 is ASCII '1'
    }
    if (strcmp(filename, "/dev/spi") == 0) {
        SPI.begin();
        tcl_free(filename);
        return tcl_result(tcl, TCL_OK, tcl_alloc("\x12", 1));
    }
    // it's a filename
    int mode = FILE_READ;
    tcl_value_t *m = tcl_list_at(args, 2);
    if (strcmp(m, "w") == 0) mode = FILE_WRITE;
    tcl_free(m);
    File f = SD.open(tcl_string(filename), mode);
    if (!f) return tcl_result(tcl, TCL_ERROR, tcl_alloc("file not found", 14));
    char out[5];
    uintptr_t fp = &f;
    out[0] = 0x1C;
    memcpy(out + 1, &fp, 4); // 4 because 32-bits architecture
    tcl_free(filename);
    return tcl_result(tcl, TCL_OK, tcl_alloc(out, sizeof(out)));
}

static tcl_result_t tcl_cmd_close(struct tcl *tcl, tcl_value_t *args, void *arg) {
    tcl_value_t fd = tcl_list_at(args, 0);
    if (fd[0] == 0x11) { // Serial ports do not need to be closed
        tcl_free(fd);
        return tcl_result(tcl, TCL_OK, tcl_alloc("", 0));
    }
    if (fd[0] == 0x12) {
        SPI.end();
        tcl_free(fd);
        return tcl_result(tcl, TCL_OK, tcl_alloc("", 0));
    }
    uintptr_t fp = (uintptr_t)*(fd + 1);
    File f = (File)*fp;
    f.close();
    free(f);
    tcl_free(fd);
    return tcl_result(tcl, TCL_OK, tcl_alloc("", 0));
}

void tcl_init_io(struct tcl *tcl) {
    tcl_register(tcl, "puts", tcl_cmd_puts, 0);
    tcl_register(tcl, "open", tcl_cmd_open, 0);
    tcl_register(tcl, "close", tcl_cmd_close, 2);
}