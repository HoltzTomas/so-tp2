#ifndef _IDTLOADER_H_
#define _IDTLOADER_H_

#include <stdint.h>

void setup_IDT_entry(int index, uint64_t offset);
void load_idt(void);

#endif
