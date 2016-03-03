#include <stdio.h>
#include "cspec.h"

describe(Calculator)

it(Calculator, can add numbers together, {
    expect(1 + 0 == 1)
    expect(1 + 1 == 2)
})

it(Calculator, cannot subtract numbers, {
    expect(2 - 1 == 0)
})
