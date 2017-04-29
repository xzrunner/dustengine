#ifndef dust_statistics_h
#define dust_statistics_h

#include <stdbool.h>

void stat_init();

void stat_enable(bool enable);

void stat_update();
void stat_print();

void stat_set_mem(float tot, float lua);

void stat_add_overdraw_area(float area);

#endif // dust_statistics_h