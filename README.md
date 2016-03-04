# cspec
Behavior driven test framework written for C.  In order to use this testing framework you must include the 'cspec.h' header file and compile linking with the libcspec.so library.  The shared object can then be loaded by the 'cspec' executable and run the tests within it.

Cspec currently handles the following functionality:
* describe
* before
* after
* it
* pending

Example test file (cspec_example.c):
``` c
#include "cspec.h"

describe(Calculator)

it(Calculator, can add numbers, {
    expect(1 + 0 == 1)
    expect(1 + 1 == 2)
})

it(Calculator, can subtract numbers, {
    expect(2 - 1 == 0)
})

it(Calculator, can divide numbers, {
    pending()
})
```
In order to build the above test you will need a line similar to below in your Makefile:
``` Makefile
cspec_example.so: cspec_example.o
$(CC) -fPIC -shared $^ -o $@ -lcspec
```

Contributing:  Do It!!  There are probably bugs in this.  Or features that are wanted.  Just submit a request for new features or send a pull request.
