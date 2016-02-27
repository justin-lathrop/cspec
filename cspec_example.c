#include <stdio.h>
#include "cspec.h"

before(,, { printf("before everything\n"); })
after(,, { printf("after everything\n"); })

describe(Calculator)
before(Calculator,, { printf("before describe\n"); })
after(Calculator,, { printf("after describe\n"); })

it(Calculator, can add numbers together, { printf("it\n"); })
before(Calculator, can add numbers together, { printf("before it\n"); })
after(Calculator, can add numbers together, { printf("after it\n"); })
