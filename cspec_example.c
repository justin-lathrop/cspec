#include <stdio.h>
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
