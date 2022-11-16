# ABANDONED
# tinyTcl

tinyTcl is a small header-only Tcl interpreter forked from <https://github.com/zserge/partcl> and optimized to run on the ESP32.

## Available commands

### Basic

```tcl
# any text you like after comment
subst arg
set var ?val?
while cond loop
if cond branch ?cond? ?branch? ?other?
proc name args body
return ?val?
break
continue
```

### Math

```tcl
+ num1 num2
- num1 num2
* num1 num2
/ num1 num2
< num1 num2
> num1 num2
<= num1 num2
>= num1 num2
== num1 num2
!= num1 num2
# these commands only take 2 arguments for now
```

### I/O

```tcl
open stream ?details?
# serial ports are setup as /dev/serial0.../dev/serial3 -- take baud rate
# SPI port is setup as /dev/spi -- takes nothing
# other files on SD card -- take "r" for read and "w" for write
puts ?-nonewline? ?stream? string
# if no stream specified uses /dev/serial (which is default serial port)
read stream ?amount?
# read ALL available bytes from specified stream
# for SPI available is unknown, so amount is required
close stream
# closing a serial port is a noop
```

### Pins

```tcl
# check arduino core files for numbering of analog pins
pin mode -i|-o|-iu|-id pinnum
# sets pin mode: input, output, input-pullup, input-pulldown (if supported)
pin read -a|-d|-t pinnum
# reads pin value: analog, digital, touch (if supported)
pin write -a|-d pinnum value
# writes value to pin: analog or digital
```

## Arduino usage

From an SD card:

```cpp
#include <SD.h>
#include "tinytcl.h"
struct tcl tcl;
void setup() {
    // other setup code
    SD.begin();
    File f = SD.open("main.tcl");
    int a = f.available();
    tcl_value_t *s = malloc(a);
    f.readBytes(s, a);

    tcl_init(&tcl);
    tcl_result_t r = tcl_eval(&tcl, s, a);
    // now do something with tcl.result and r
}
```

Interactive Serial REPL:

```cpp
#include <stdlib.h>
#include "tinytcl.h"
#define CHUNK 64
void loop() {
    struct tcl tcl;
    int buflen = CHUNK;
    char *buf = malloc(buflen);
    int i = 0;
    tcl_init(&tcl);
    while (true) {
        char inp = Serial.read();
        if (i > buflen - 1) buf = realloc(buf, buflen += CHUNK);
        if (inp == 0 || inp == -1) continue;
        buf[i++] = inp;
        tcl_each(buf, i, true) {
            if (p.token == TOK_ERROR && (p.to - buf) != i) {
                memset(buf, 0, buflen);
                i = 0;
                Serial.println("Syntax error");
                break;
            } else if (p.token == TOK_COMMAND && *(p.from) != '\0') {
                tcl_result_t r = tcl_eval(&tcl, buf, strlen(buf));
                if (r != TCL_ERROR) {
                    Serial.print("result> ");
                } else {
                    Serial.print("?! ");
                }
                Serial.println(tcl_string(tcl.result));
                memset(buf, 0, buflen);
                i = 0;
                break;
            }
        }
    }
}
```
