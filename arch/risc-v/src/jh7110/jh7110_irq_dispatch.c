/****************************************************************************
 * arch/risc-v/src/jh7110/jh7110_irq_dispatch.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <assert.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <sys/types.h>

#include "riscv_internal.h"
#include "hardware/jh7110_memorymap.h"
#include "hardware/jh7110_plic.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RV_IRQ_MASK 59

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * riscv_dispatch_irq
 ****************************************************************************/

void *riscv_dispatch_irq(uintptr_t vector, uintptr_t *regs)
{
  int irq = (vector >> RV_IRQ_MASK) | (vector & 0xf);

  /* Firstly, check if the irq is machine external interrupt */

  if (RISCV_IRQ_EXT == irq)
    {
      uintptr_t val = getreg32(JH7110_PLIC_CLAIM);

      ////Begin
      _info("irq=%d, claim=%p\n", irq, val);////
      _info("*0xe0201004=%p\n", (uintptr_t)getreg32((uintptr_t)0xe0201004));////
      infodumpbuffer("PLIC Interrupt Pending", 0xe0001000, 2 * 4);////
      uintptr_t ip0 = getreg32(0xe0001000);  // PLIC_IP0: Interrupt Pending for interrupts 1 to 31
      uintptr_t ip1 = getreg32(0xe0001004);  // PLIC_IP1: Interrupt Pending for interrupts 32 to 63
      if (val == 0) {
        if (ip0 & (1 << 20)) { val = 20; }
      }
      ////End

      /* Add the value to nuttx irq which is offset to the mext */

      irq += val;
    }

  /* EXT means no interrupt */

  if (RISCV_IRQ_EXT != irq)
    {
      /* Deliver the IRQ */

      _info("Do irq=%d\n", irq);////
      regs = riscv_doirq(irq, regs);
    }

  if (RISCV_IRQ_EXT <= irq)
    {
      /* Then write PLIC_CLAIM to clear pending in PLIC */

      putreg32(irq - RISCV_IRQ_EXT, JH7110_PLIC_CLAIM);
      infodumpbuffer("After Claim", 0xe0001000, 2 * 4);////
    }

  return regs;
}
