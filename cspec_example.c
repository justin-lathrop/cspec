#include <stdio.h>
#include "cspec.h"

describe(Calculator)
before(Calculator, before, { printf("BEFORE\n"); })
it(Calculator, can add numbers together, {  })
it(Calculator, can turn off, {  })
after(Calculator, after, { printf("BEFORE\n"); })
