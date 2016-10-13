/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/sgpe/stop_gpe/p9_sgpe_stop_exit.c $ */
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

#include "p9_sgpe_stop.h"
#include "p9_sgpe_stop_exit_marks.h"
#include "p9_hcd_sgpe_boot_cme.h"
#include "p9_dd1_doorbell_wr.h"

extern SgpeStopRecord G_sgpe_stop_record;

#if HW386311_DD1_PBIE_RW_PTR_STOP11_FIX

    extern uint64_t G_ring_save[MAX_QUADS][8];
    extern uint64_t G_ring_spin[10][2];

#endif

#if FUSED_CORE_MODE_SCAN_FIX

uint32_t G_fcm_spin[4] = {0, 435, 1402, 2411};

#endif

int
p9_sgpe_stop_exit()
{
    uint32_t     m_l2;
    uint32_t     m_pg;
    uint32_t     cloop;
    uint32_t     qloop;
    uint32_t     cexit;
    uint32_t     qspwu;
    int          cme;
    uint32_t     core;
    uint64_t     scom_data = 0;
    uint64_t     cme_flags;
    ppm_sshsrc_t hist;
    ocb_ccsr_t   ccsr;
#if HW386311_DD1_PBIE_RW_PTR_STOP11_FIX
    int          spin;
#endif

    //===============================
    MARK_TAG(BEGINSCOPE_STOP_EXIT, 0)
    //===============================

    PK_TRACE_INF("Core Exit Vectors:    X[%x] X0[%x] X1[%x] Q[%x]",
                 G_sgpe_stop_record.group.ex_b[VECTOR_EXIT],
                 G_sgpe_stop_record.group.ex_l[VECTOR_EXIT],
                 G_sgpe_stop_record.group.ex_r[VECTOR_EXIT],
                 G_sgpe_stop_record.group.quad[VECTOR_EXIT]);



    for(cexit = G_sgpe_stop_record.group.core[VECTOR_EXIT],
        qspwu = G_sgpe_stop_record.group.qswu[VECTOR_EXIT],
        qloop = 0, m_l2 = 0, m_pg = 0;
        (cexit > 0 || qspwu > 0) && (qloop < MAX_QUADS);
        cexit = cexit << 4, qspwu = qspwu << 1, qloop++, m_l2 = 0, m_pg = 0)
    {

        if (!((cexit & BITS32(0, 4)) || (qspwu & BIT32(0))))
        {
            continue;
        }

        //--------------------------------------------------------------------------
        PK_TRACE_INF("+++++ +++++ BEGIN OF STOP EXIT +++++ +++++");
        //--------------------------------------------------------------------------

        if (G_sgpe_stop_record.group.ex_l[VECTOR_CONFIG] & BIT32(qloop))
        {
            m_pg |= FST_EX_IN_QUAD;
        }

        if (G_sgpe_stop_record.group.ex_r[VECTOR_CONFIG] & BIT32(qloop))
        {
            m_pg |= SND_EX_IN_QUAD;
        }

        if ((((cexit & BITS32(0, 2)) || (qspwu & BIT32(0))) &&
             (G_sgpe_stop_record.state[qloop].act_state_x0 >= STOP_LEVEL_8)))
        {
            m_l2 |= FST_EX_IN_QUAD;
        }

        if ((((cexit & BITS32(2, 2)) || (qspwu & BIT32(0))) &&
             (G_sgpe_stop_record.state[qloop].act_state_x1 >= STOP_LEVEL_8)))
        {
            m_l2 |= SND_EX_IN_QUAD;
        }

        m_l2 = m_l2 & m_pg;

        PK_TRACE_INF("Check: quad[%d] m_l2[%d] m_pg[%d]", qloop, m_l2, m_pg);



        PK_TRACE("Update QSSR: stop_exit_ongoing");
        out32(OCB_QSSR_OR, BIT32(qloop + 26));

        for(cloop = 0; cloop < CORES_PER_QUAD; cloop++)
        {
            if(!(cexit & BIT32(cloop)))
            {
                continue;
            }

            PK_TRACE("Update STOP history on core[%d]: in transition of exit",
                     ((qloop << 2) + cloop));
            SGPE_STOP_UPDATE_HISTORY(((qloop << 2) + cloop),
                                     CORE_ADDR_BASE,
                                     STOP_CORE_IS_GATED,
                                     STOP_TRANS_EXIT,
                                     STOP_LEVEL_0,
                                     STOP_LEVEL_0,
                                     STOP_REQ_DISABLE,
                                     STOP_ACT_DISABLE);
        }



        if(m_l2 && G_sgpe_stop_record.state[qloop].act_state_q >= STOP_LEVEL_11)
        {

            //--------------------------------------------------------------------------
            PK_TRACE_INF("+++++ +++++ QUAD STOP EXIT [LEVEL 11-15] +++++ +++++");
            //--------------------------------------------------------------------------

            PK_TRACE("Update STOP history on quad[%d]: in transition of exit",
                     qloop);
            SGPE_STOP_UPDATE_HISTORY(qloop,
                                     QUAD_ADDR_BASE,
                                     STOP_CACHE_IS_GATED,
                                     STOP_TRANS_EXIT,
                                     STOP_LEVEL_0,
                                     STOP_LEVEL_0,
                                     STOP_REQ_DISABLE,
                                     STOP_ACT_DISABLE);

            //=================================
            MARK_TAG(SX_POWERON, (32 >> qloop))
            //=================================

            // NOTE: it is required to serialize power on quad PFETs to prevent droop
            PK_TRACE_INF("SX11.A: Cache[%d] Poweron", qloop);
            p9_hcd_cache_poweron(qloop);

            //=========================
            MARK_TRAP(SX_CHIPLET_RESET)
            //=========================

            PK_TRACE_INF("SX11.B: Cache[%d] Chiplet Reset with PG[%d]", qloop, m_pg);
            p9_hcd_cache_chiplet_reset(qloop, m_pg);

#if !STOP_INITF

            PK_TRACE_INF("SX11.C: Cache[%d] Chiplet L3 DCC Setup", qloop);
            p9_hcd_cache_chiplet_l3_dcc_setup(qloop);

#endif


#if !STOP_PRIME
#if !STOP_INITF

            PK_TRACE_INF("SX11.D: Cache[%d] Gptr Time Initf", qloop);
            p9_hcd_cache_gptr_time_initf(qloop);

#endif

            //====================================
            MARK_TAG(SX_DPLL_SETUP, (32 >> qloop))
            //====================================

#if !STOP_INITF

            PK_TRACE_INF("SX11.E: Cache[%d] Dpll Initf", qloop);
            p9_hcd_cache_dpll_initf(qloop);

#endif

            PK_TRACE_INF("SX11.F: Cache[%d] Dpll Setup", qloop);
            p9_hcd_cache_dpll_setup(qloop);

#if !STOP_INITF

            PK_TRACE_INF("SX11.G: Cache[%d] DCC Skewadjust Setup", qloop);
            p9_hcd_cache_dcc_skewadjust_setup(qloop);

#endif

            // set 20, 22, 24 and 26 during Stop11 Exit after setting up the DPLL
            scom_data = BIT64(20) | BIT64(22) | BIT64(24) | BIT64(26);

            // set 21, 23, 25, and 27 if EX0 is bad (not partial good)
            if ((~m_pg) & FST_EX_IN_QUAD)
            {
                scom_data |= BIT64(21) | BIT64(23) | BIT64(25) | BIT64(27);
            }

            PK_TRACE("Assert inter-ppm settings via QPMMR[22,24,26,EX0PB:21,23,25]");
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(QPPM_QPMMR_OR, qloop), scom_data);

            //=======================================
            MARK_TAG(SX_CHIPLET_INITS, (32 >> qloop))
            //=======================================

            PK_TRACE_INF("SX11.H: Cache[%d] Chiplet Init", qloop);
            p9_hcd_cache_chiplet_init(qloop);

#if !STOP_INITF

            PK_TRACE_INF("SX11.I: Cache[%d] Repair Initf", qloop);
            p9_hcd_cache_repair_initf(qloop);

#endif

            //====================================
            MARK_TAG(SX_ARRAY_INIT, (32 >> qloop))
            //====================================

#if !SKIP_ARRAYINIT

            PK_TRACE_INF("SX11.J: Cache[%d] Arrayinit", qloop);
            p9_hcd_cache_arrayinit(qloop, m_pg);

#endif

#if FUSED_CORE_MODE_SCAN_FIX

            PK_TRACE_INF("FCMS: Engage with Fused Mode Scan Workaround");

            // bit8/9 = l2-0/1, bit49 = cfg
            PK_TRACE("FCMS: Setup scan register to select the ring");
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x10030005, qloop), BITS64(8, 2) | BIT64(49));

            PK_TRACE("FCMS: checkword set");
            scom_data = 0xa5a5a5a5a5a5a5a5;
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);

            for(spin = 1;; spin++)
            {
                PK_TRACE("FCMS: spin ring loop%d", spin);
                scom_data = (G_fcm_spin[spin] - G_fcm_spin[spin - 1]) << 32;
                GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x10039000, qloop), scom_data);

                PK_TRACE("FCMS: Poll OPCG done for ring spin");

                do
                {
                    GPE_GETSCOM(GPE_SCOM_ADDR_QUAD(0x10000100, qloop), scom_data);
                }
                while(~scom_data & BIT64(8));

                if (spin == 3)
                {
                    PK_TRACE("FCMS: checkword check");
                    GPE_GETSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);

                    if (scom_data != 0xa5a5a5a5a5a5a5a5)
                    {
                        PK_TRACE("ERROR: checkword[%x%x] failed. HALT SGPE!",
                                 UPPER32(scom_data), LOWER32(scom_data));
                        pk_halt();
                    }

                    break;
                }

                PK_TRACE("FCMS: restore fused core mode bit");
                GPE_GETSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);
                RESTORE_RING_BITS(BIT64(0), scom_data, BIT64(0));
                GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);
            }

            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x10030005, qloop), 0);

#endif

#if HW386311_DD1_PBIE_RW_PTR_STOP11_FIX

            PK_TRACE_INF("FCMS: Engage with PBIE Read/Write Pointer Scan Workaround");

            // bit4,5,11 = perv/eqpb/pbieq, bit59 = inex
            PK_TRACE("PBRW: Setup scan register to select the ring");
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x10030005, qloop), BITS64(4, 2) | BIT64(11) | BIT64(59));

            PK_TRACE("PBRW: checkword set");
            scom_data = 0xa5a5a5a5a5a5a5a5;
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);

            for(spin = 1;; spin++)
            {
                PK_TRACE("PBRW: spin ring loop%d", spin);
                scom_data = (G_ring_spin[spin][0] - G_ring_spin[spin - 1][0]) << 32;
                GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x10039000, qloop), scom_data);

                PK_TRACE("PBRW: Poll OPCG done for ring spin");

                do
                {
                    GPE_GETSCOM(GPE_SCOM_ADDR_QUAD(0x10000100, qloop), scom_data);
                }
                while(~scom_data & BIT64(8));

                if (spin == 9)
                {
                    PK_TRACE("PBRW: checkword check");
                    GPE_GETSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);

                    if (scom_data != 0xa5a5a5a5a5a5a5a5)
                    {
                        PK_TRACE_INF("ERROR: checkword[%x%x] failed. HALT SGPE!",
                                     UPPER32(scom_data), LOWER32(scom_data));
                        pk_halt();
                    }

                    break;
                }

                PK_TRACE("PBRW: restore pbie read ptr");
                GPE_GETSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);
                PK_TRACE("PBRW: mask: %8x %8x",
                         UPPER32(G_ring_spin[spin][1]),
                         LOWER32(G_ring_spin[spin][1]));
                PK_TRACE("PBRW: ring: %8x %8x",
                         UPPER32(scom_data),
                         LOWER32(scom_data));
                PK_TRACE("PBRW: save: %8x %8x",
                         UPPER32(G_ring_save[qloop][spin - 1]),
                         LOWER32(G_ring_save[qloop][spin - 1]));
                RESTORE_RING_BITS(G_ring_spin[spin][1], scom_data, G_ring_save[qloop][spin - 1]);
                GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x1003E000, qloop), scom_data);
                PK_TRACE("PBRW: mask: %8x %8x",
                         UPPER32(G_ring_spin[spin][1]),
                         LOWER32(G_ring_spin[spin][1]));
                PK_TRACE("PBRW: ring: %8x %8x",
                         UPPER32(scom_data),
                         LOWER32(scom_data));
                PK_TRACE("PBRW: save: %8x %8x",
                         UPPER32(G_ring_save[qloop][spin - 1]),
                         LOWER32(G_ring_save[qloop][spin - 1]));

            }

            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(0x10030005, qloop), 0);
#endif

            //=====================
            MARK_TRAP(SX_FUNC_INIT)
            //=====================

#if !SKIP_INITF

            PK_TRACE_INF("SX11.K: Cache[%d] Func Scan", qloop);
            p9_hcd_cache_initf(qloop);

#endif
#endif

            //===========================================
            MARK_TAG(SX_CACHE_STARTCLOCKS, (32 >> qloop))
            //===========================================

            PK_TRACE_INF("SX11.L: Cache[%d] Startclocks with PG[%d]", qloop, m_pg);
            p9_hcd_cache_startclocks(qloop, m_pg);

        }

        if (m_l2)
        {

            //--------------------------------------------------------------------------
            PK_TRACE_INF("+++++ +++++ EX STOP EXIT [LEVEL 8-10] +++++ +++++");
            //--------------------------------------------------------------------------

            //========================================================
            MARK_TAG(SX_L2_STARTCLOCKS, ((m_l2 << 6) | (32 >> qloop)))
            //========================================================

            // do this again here for stop8 in addition to dpll_setup
            PK_TRACE("Switch L2 glsmux select to DPLL output via EXCGCR[34,35]");
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(EQ_QPPM_EXCGCR_OR, qloop), BITS64(34, 2));

            PK_TRACE_INF("SX8.A: Cache[%d] L2[%d] Startclocks with PG[%d]",
                         qloop, m_l2, m_pg);
            p9_hcd_cache_l2_startclocks(qloop, m_l2, m_pg);

            // reset ex actual state if ex is exited.
            if (m_l2 & FST_EX_IN_QUAD)
            {
                G_sgpe_stop_record.state[qloop].act_state_x0 = 0;
            }

            if (m_l2 & SND_EX_IN_QUAD)
            {
                G_sgpe_stop_record.state[qloop].act_state_x1 = 0;
            }

            PK_TRACE("Update QSSR: drop l2_stopped");
            out32(OCB_QSSR_CLR, (m_l2 << SHIFT32((qloop << 1) + 1)));
        }

        if(m_l2 && G_sgpe_stop_record.state[qloop].act_state_q >= STOP_LEVEL_11)
        {

            //--------------------------------------------------------------------------
            PK_TRACE_INF("+++++ +++++ QUAD STOP EXIT CONTINUE +++++ +++++");
            //--------------------------------------------------------------------------

            for(cme = 0; cme < EXES_PER_QUAD; cme += 2)
            {

                core = ((cexit & BITS32(cme, 2)) >> SHIFT32((cme + 1)));

                if(!core)
                {
                    continue;
                }

                PK_TRACE("Assert core[%d]-L2/CC quiesces via SICR[6/7,8/9]", core);
                GPE_PUTSCOM(GPE_SCOM_ADDR_CME(CME_SCOM_SICR_OR,
                                              qloop, (cme >> 1)),
                            ((uint64_t)core << SHIFT64(7) |
                             (uint64_t)core << SHIFT64(9)));

                PK_TRACE("Poll for interface quiesced via CME[%d] SISR[30,31]", cme);

                do
                {
                    GPE_GETSCOM(GPE_SCOM_ADDR_CME(CME_SCOM_SISR,
                                                  qloop, (cme >> 1)), scom_data);
                }
                while(((scom_data >> 32) & core) != core);
            }

#if !STOP_PRIME

            //====================================
            MARK_TAG(SX_SCOM_INITS, (32 >> qloop))
            //====================================

            PK_TRACE_INF("SX11.M: Cache[%d] Scom Init with PG[%d]", qloop, m_pg);
            p9_hcd_cache_scominit(qloop, m_pg);

            PK_TRACE_INF("SX11.N: Cache[%d] Scom Cust", qloop);
            p9_hcd_cache_scomcust(qloop);

            //==================================
            MARK_TAG(SX_CME_BOOT, (32 >> qloop))
            //==================================

            // doorbell cme to let rebooted cme knows about ongoing stop11
            for(cloop = 0; cloop < CORES_PER_QUAD; cloop++)
            {
                if(!(cexit & BIT32(cloop)))
                {
                    continue;
                }

                PK_TRACE_INF("SX11.O: Core[%d] DB1 to CME", ((qloop << 2) + cloop));
                p9_dd1_db_unicast_wr(GPE_SCOM_ADDR_CORE(CPPM_CMEMSG,
                                                        ((qloop << 2) + cloop)), BIT64(0));
                p9_dd1_db_unicast_wr(GPE_SCOM_ADDR_CORE(CPPM_CMEDB1_OR,
                                                        ((qloop << 2) + cloop)), BIT64(7));
            }

            // Setting up cme_flags
            do
            {
                ccsr.value = in32(OCB_CCSR);
            }
            while (ccsr.fields.change_in_progress);

            if (m_pg & FST_EX_IN_QUAD)
            {
                cme_flags = 0;

                if (m_pg & SND_EX_IN_QUAD)
                {
                    cme_flags |= CME_SIBLING_FUNCTIONAL;
                }

                if (ccsr.value & BIT32((qloop << 2)))
                {
                    cme_flags |= CME_CORE0_ENABLE;
                    GPE_GETSCOM(GPE_SCOM_ADDR_CORE(C_NET_CTRL0,
                                                   ((qloop << 2))),     scom_data);

                    if (!(scom_data & BIT64(18)))
                    {
                        cme_flags |= CME_CORE0_ENTRY_FIRST;
                    }
                }

                if (ccsr.value & BIT32((qloop << 2) + 1))
                {
                    cme_flags |= CME_CORE1_ENABLE;
                    GPE_GETSCOM(GPE_SCOM_ADDR_CORE(C_NET_CTRL0,
                                                   ((qloop << 2) + 1)), scom_data);

                    if (!(scom_data & BIT64(18)))
                    {
                        cme_flags |= CME_CORE1_ENTRY_FIRST;
                    }
                }

                GPE_PUTSCOM(GPE_SCOM_ADDR_CME(CME_SCOM_FLAGS_OR, qloop, 0),
                            cme_flags);
            }

            if (m_pg & SND_EX_IN_QUAD)
            {
                cme_flags = CME_EX1_INDICATOR;

                if (m_pg & FST_EX_IN_QUAD)
                {
                    cme_flags |= CME_SIBLING_FUNCTIONAL;
                }

                if (ccsr.value & BIT32(((qloop << 2) + 2)))
                {
                    cme_flags |= CME_CORE0_ENABLE;
                    GPE_GETSCOM(GPE_SCOM_ADDR_CORE(C_NET_CTRL0,
                                                   ((qloop << 2) + 2)), scom_data);

                    if (!(scom_data & BIT64(18)))
                    {
                        cme_flags |= CME_CORE0_ENTRY_FIRST;
                    }
                }

                if (ccsr.value & BIT32(((qloop << 2) + 3)))
                {
                    cme_flags |= CME_CORE1_ENABLE;
                    GPE_GETSCOM(GPE_SCOM_ADDR_CORE(C_NET_CTRL0,
                                                   ((qloop << 2) + 3)), scom_data);

                    if (!(scom_data & BIT64(18)))
                    {
                        cme_flags |= CME_CORE1_ENTRY_FIRST;
                    }
                }

                GPE_PUTSCOM(GPE_SCOM_ADDR_CME(CME_SCOM_FLAGS_OR, qloop, 1),
                            cme_flags);
            }

            if (in32(OCB_OCCFLG) & BIT32(13))
            {
                PK_TRACE_INF("BREAK: Trap Before CME Boot");
                asm volatile ("trap");
            }

#if !SKIP_CME_BOOT_STOP11

            uint16_t cmeBootList = (m_pg << SHIFT16(((qloop << 1) + 1)));
            PK_TRACE_INF("SX11.P: Boot CME[%x]", cmeBootList);
            boot_cme( cmeBootList );

#endif

            //=======================================
            MARK_TAG(SX_RUNTIME_INITS, (32 >> qloop))
            //=======================================

            PK_TRACE_INF("SX11.Q: Cache[%d] RAS Runtime Scom", qloop);
            p9_hcd_cache_ras_runtime_scom(qloop);

            PK_TRACE_INF("SX11.R: Cache[%d] OCC Runtime Scom", qloop);
            p9_hcd_cache_occ_runtime_scom(qloop);

            //=========================
            MARK_TRAP(SX_ENABLE_ANALOG)
            //=========================

#endif

            G_sgpe_stop_record.state[qloop].act_state_q = 0;

            PK_TRACE("Update STOP history on quad[%d]: \
                     STOP exit completed, cache ready", qloop);
            SGPE_STOP_UPDATE_HISTORY(qloop,
                                     QUAD_ADDR_BASE,
                                     STOP_CACHE_READY_RUN,
                                     STOP_TRANS_COMPLETE,
                                     STOP_LEVEL_0,
                                     STOP_LEVEL_0,
                                     STOP_REQ_DISABLE,
                                     STOP_ACT_DISABLE);

            PK_TRACE("Update QSSR: drop quad_stopped");
            out32(OCB_QSSR_CLR, BIT32(qloop + 14));
        }

        //--------------------------------------------------------------------------
        PK_TRACE_INF("+++++ +++++ END OF STOP EXIT +++++ +++++");
        //--------------------------------------------------------------------------

        if (qspwu & BIT32(0))
        {
            PK_TRACE_INF("SX0.A: Assert Quad[%d] SPWU Done", qloop);
            GPE_PUTSCOM(GPE_SCOM_ADDR_QUAD(PPM_GPMMR_OR, qloop), BIT64(0));

            G_sgpe_stop_record.group.qswu[VECTOR_CONFIG] |= BIT32(qloop);
            G_sgpe_stop_record.group.qswu[VECTOR_EXIT]   &= ~BIT32(qloop);
        }

        // process core portion of core request
        for(cloop = 0; cloop < CORES_PER_QUAD; cloop++)
        {
            if(!(cexit & BIT32(cloop)))
            {
                continue;
            }

            // reset clevel to 0 if core is going to wake up
            G_sgpe_stop_record.level[qloop][cloop] = 0;

            PK_TRACE_INF("SX0.B: Switch Core[%d]PPM Wakeup back to CME via CPMMR[13]",
                         ((qloop << 2) + cloop));
            GPE_PUTSCOM(GPE_SCOM_ADDR_CORE(CPPM_CPMMR_CLR,
                                           ((qloop << 2) + cloop)), BIT64(13));
        }

        PK_TRACE("Update QSSR: drop stop_exit_ongoing");
        out32(OCB_QSSR_CLR, BIT32(qloop + 26));
    }

    //===========================
    MARK_TRAP(ENDSCOPE_STOP_EXIT)
    //===========================

    return SGPE_STOP_SUCCESS;
}
