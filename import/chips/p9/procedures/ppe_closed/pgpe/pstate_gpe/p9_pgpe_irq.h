/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/pgpe/pstate_gpe/p9_pgpe_irq.h $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2015,2018                                                    */
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
//-----------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2015
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
//-----------------------------------------------------------------------------

/// \file   p9_pgpe_irq.h
/// \brief  Shared and global definitions for PGPE (GPE2) H-codes.
///


// Notes:
// - The only define names that should be changed/added/removed
//   in this file are:
//   - IRQ_VEC_PRTY(n>0)_GPE(x)
//   - IDX_PRTY_LVL_(task_abbr) and reflect in relevant H-code as well
//   - All other define names are used in H-codes
// - The variable names and actions in this file must perfectly match associated

#ifndef _P9_PGPE_IRQ_H_
#define _P9_PGPE_IRQ_H_

extern uint32_t G_OCB_OIMR1_OR;
extern uint32_t G_OCB_OIMR1_CLR;
extern uint32_t G_OCB_OIMR0_OR;
extern uint32_t G_OCB_OIMR0_CLR;

// We define four levels of TRACE outputs:
// _INF:  Trace level used for main informational events.
// _DBG:  Trace level used for expanded debugging.
// _WARN: Trace level used when suspecious event happens.
// _ERR:  Trace level at time of an error that leads to a halt.
#define MY_TRACE_INF(...)   PK_TRACE("INF: "__VA_ARGS__);
#ifdef DEV_DEBUG
    #define MY_TRACE_DBG(...)   PK_TRACE("DBG: "__VA_ARGS__);
#else
    #define MY_TRACE_DBG(...)
#endif
#define MY_TRACE_WARN(...)  PK_TRACE("WARN: "__VA_ARGS__);
#define MY_TRACE_ERR(...)   PK_TRACE("ERR: "__VA_ARGS__);

#define TRUE  1
#define FALSE 0

#define IDX_PRTY_LVL_HIPRTY        0
#define IDX_PRTY_LVL_OCBERR        1
#define IDX_PRTY_LVL_XSTOP_GPE2    2
#define IDX_PRTY_LVL_IPI2HI        3
#define IDX_PRTY_LVL_TYPE4         4
#define IDX_PRTY_LVL_TYPE1         5
#define IDX_PRTY_LVL_DISABLED      6
#define IDX_PRTY_VEC  0
#define IDX_MASK_VEC  1
#define NUM_EXT_IRQ_PRTY_LEVELS  (uint8_t)(7)
extern const uint64_t ext_irq_vectors_gpe[NUM_EXT_IRQ_PRTY_LEVELS][2];

#define IRQ_VEC_PRTY0_GPE2   (uint64_t)(0x0000000000000000) // Non-task hi-prty IRQs
// Shared between all instances
#define IRQ_VEC_PRTY1_GPE2  (uint64_t)(0x2080080000002000) // Task1-OCB_ERROR(HeartBeat Loss)/GPE3_ERROR/PCB_TYPE5/PVREF
#define IRQ_VEC_PRTY2_GPE2  (uint64_t)(0x0001000000000000) // Task2-CHECK_STOP_GPE2
#define IRQ_VEC_PRTY3_GPE2  (uint64_t)(0x0000001000000000) // Task3-IPI2-HI(IPC from OCC/SGPE)
#define IRQ_VEC_PRTY4_GPE2  (uint64_t)(0x0000000000004000) // Task4-PCB_INTR_TYPE4(PCB Type4 from CME)
#define IRQ_VEC_PRTY5_GPE2  (uint64_t)(0x0000000000020000) // Task5-PCB_INTR_TYPE1(PCB Type1 from CME)
#if OVERRIDE_OTHER_ENGINES_IRQS == 1
    #define IRQ_VEC_PRTY6_GPE2  (uint64_t)(0xDF7EFF03BFFDFFF5) // Other instances' IRQs
#else
    #define IRQ_VEC_PRTY6_GPE2  (uint64_t)(0x0000000000000000) // Other instances' IRQs
#endif
// Unique to each instance
// We should never detect these

#define IRQ_VEC_ALL_OUR_IRQS  ( IRQ_VEC_PRTY0_GPE2  | \
                                IRQ_VEC_PRTY1_GPE2 | \
                                IRQ_VEC_PRTY2_GPE2 | \
                                IRQ_VEC_PRTY3_GPE2 | \
                                IRQ_VEC_PRTY4_GPE2 | \
                                IRQ_VEC_PRTY5_GPE2 )      // Note, we do not incl PRTY6 here!

extern uint8_t    g_current_prty_level;
extern uint8_t    g_oimr_stack[NUM_EXT_IRQ_PRTY_LEVELS];
extern int        g_oimr_stack_ctr;
extern uint64_t   g_oimr_override_stack[NUM_EXT_IRQ_PRTY_LEVELS];
extern uint64_t   g_oimr_override;

void pk_irq_save_and_set_mask(uint32_t iPrtyLvl);



/// Restore a vector of interrupts by overwriting OIMR.
UNLESS__PPE42_IRQ_CORE_C__(extern)
inline void
pk_irq_vec_restore( PkMachineContext* context)
{
    pk_critical_section_enter(context);

    if (g_oimr_stack_ctr >= 0)
    {
        out32( G_OCB_OIMR0_CLR, (uint32_t)((IRQ_VEC_ALL_OUR_IRQS |
                                            g_oimr_override_stack[g_oimr_stack_ctr]) >> 32));
        out32( G_OCB_OIMR1_CLR, (uint32_t)(IRQ_VEC_ALL_OUR_IRQS |
                                           g_oimr_override_stack[g_oimr_stack_ctr]));
        out32( G_OCB_OIMR0_OR,
               (uint32_t)((ext_irq_vectors_gpe[g_oimr_stack[g_oimr_stack_ctr]][IDX_MASK_VEC] |
                           g_oimr_override) >> 32));
        out32( G_OCB_OIMR1_OR,
               (uint32_t)(ext_irq_vectors_gpe[g_oimr_stack[g_oimr_stack_ctr]][IDX_MASK_VEC] |
                          g_oimr_override));
        // Restore the prty level tracker to the task that was interrupted, if any.
        g_current_prty_level = g_oimr_stack[g_oimr_stack_ctr];
        g_oimr_stack_ctr--;
        PK_TRACE_INF("IRQ RS: prty_lvl=%d,  g_oimr_stack_ctr=0x%x", g_current_prty_level, g_oimr_stack_ctr);
    }
    else
    {
        PK_TRACE_ERR("ERR: Code bug: Messed up OIMR book keeping: g_oimr_stack_ctr=%d",
                     g_oimr_stack_ctr);
        PK_PANIC(PGPE_UIH_EIMR_STACK_UNDERFLOW);
    }

    pk_critical_section_exit(context);
}

//This masks all UIH external interrupts(EXCEPT for Severe Errors in the Power
//Management Engines, Chip Voltage Reference, or System Checkstop), but leaves
//timer interrupts enableda
//
//Note: Care must be taken to call equal number of pk_irq_sub_critical_enter and
//pk_irq_sub_critical_exit calls
//
//Timer interrupts FIT and DEC are lower in priority than External Interrupts, and
//also need EE bit set for interrupt handler to be called. This function is useful
//in case timer interrupts should be enabled, but external interrupts should be masked
UNLESS__PPE42_IRQ_CORE_C__(extern)
inline void pk_irq_sub_critical_enter(PkMachineContext* ctx)
{

    pk_critical_section_enter(ctx);
    pk_irq_save_and_set_mask(3);
    pk_critical_section_exit(ctx);
}

//This will unmask the UIH external interrupts masked off by previous pk_irq_sub_critical
//_enter call.
UNLESS__PPE42_IRQ_CORE_C__(extern)
inline void pk_irq_sub_critical_exit(PkMachineContext* ctx)
{

    pk_irq_vec_restore(ctx);
}
#endif // _P9_PGPE_IRQ_H_
