//
// Created by Dolores on 2026/8/10.
//

#ifndef SUMMER_SELF_TEST_H
#define SUMMER_SELF_TEST_H

#include <stdint.h>

void SelfTest_Start(void);
void SelfTest_Update(void);
uint8_t SelfTest_IsActive(void);
uint8_t SelfTest_ConsumeBlink(void);

#endif //SUMMER_SELF_TEST_H
