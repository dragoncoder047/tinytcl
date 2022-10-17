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
# 2 serial ports are setup as /dev/serial0 and /dev/serial1 -- take baud rate
# SPI port is setup as /dev/spi -- takes nothing
# other files on SD card -- take "r" for read and "w" for write
puts ?-nonewline? ?stream? string
# if no stream specified uses /dev/serial (which is default serial port)
close stream
# closing a serial port is a noop
```
