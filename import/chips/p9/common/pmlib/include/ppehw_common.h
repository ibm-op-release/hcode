/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/common/pmlib/include/ppehw_common.h $         */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2015,2017                                                    */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#ifndef __PPEHW_COMMON_H__
#define __PPEHW_COMMON_H__

/// \defgroup be64_bits Bit manipulation for 64-bit Big-Endian values
///
/// \note These macros only work in the assembler context because we build our
/// assemblers to do 64-bit arithmetic, which is required for PORE assembly.
///
/// @{

/// Create a multi-bit mask of \a n bits starting at bit \a b
#define BITS64(b, n) ((0xffffffffffffffffull << (64 - (n))) >> (b))
#define BITS32(b, n) ((0xffffffff            << (32 - (n))) >> (b))
#define BITS16(b, n) ((0xffff                << (16 - (n))) >> (b))
#define BITS8(b, n)  ((0xff                  << (8  - (n))) >> (b))

/// Create a single bit mask at bit \a b
#define BIT64(b) BITS64((b), 1)
#define BIT32(b) BITS32((b), 1)
#define BIT16(b) BITS16((b), 1)
#define BIT8(b)  BITS8((b), 1)

/// Create a amount of shift to bit location \a b
#define SHIFT64(b) (63-b)
#define SHIFT32(b) (31-b)
#define SHIFT16(b) (15-b)
#define SHIFT8(b)  (7-b)


/// Mark and Tag

#define CODE2REGA(code)     ((code & 0x1F00) >> 8)
#define CODE2REGB(code)     ((code & 0xF8) >> 3)
#define CODE2TAG(code,tag)  ((tag << 10) | (code >> 3))
#define TAG_SPRG0(tag)      {ppe42_app_ctx_set(tag);}
#define MARK_TRAP(code) \
    {asm volatile ("tw 0, %0, %1" : : \
                   "i" (CODE2REGA(code)), \
                   "i" (CODE2REGB(code)));}
#define MARK_TAG(code, tag) \
    TAG_SPRG0(CODE2TAG(code, tag)) \
    MARK_TRAP(code)


/// Wait Macro

#define PPE_CORE_CYCLE_RATIO       8 // core is 8 times faster than cme
#define PPE_FOR_LOOP_CYCLES        4 // fused compare branch(3), addition(1)
#define PPE_CORE_CYCLE_DIVIDER     (PPE_CORE_CYCLE_RATIO*PPE_FOR_LOOP_CYCLES)
#define PPE_WAIT_CORE_CYCLES(l,cc) for(l=0;l<cc/PPE_CORE_CYCLE_DIVIDER;l++);


/// IRQ Setup
enum PK_IRQ_SHORT_PARAMETER_NAMES
{
    POLARITY_LOW  = PK_IRQ_POLARITY_ACTIVE_LOW,
    POLARITY_HIGH = PK_IRQ_POLARITY_ACTIVE_HIGH,
    TRIGGER_EDGE  = PK_IRQ_TRIGGER_EDGE_SENSITIVE,
    TRIGGER_LEVEL = PK_IRQ_TRIGGER_LEVEL_SENSITIVE
};

#define PK_IRQ_SETUP(irq, polarity, trigger)                           \
    rc = pk_irq_setup(irq, polarity, trigger);                         \
    if (rc) {                                                          \
        PK_TRACE("pk_irq_setup(irq) failed w/rc=0x%08x", rc);          \
        pk_halt();                                                     \
    }

#define PK_IRQ_HANDLER_SET(irq, handler, sem)                          \
    rc = pk_irq_handler_set(irq,                                       \
                            handler,                                   \
                            (void*)&sem);                              \
    if (rc) {                                                          \
        PK_TRACE("pk_irq_handler_set(irq) failed w/rc=0x%08x", rc);    \
        pk_halt();                                                     \
    }


#endif  /* __PPEHW_COMMON_H__ */