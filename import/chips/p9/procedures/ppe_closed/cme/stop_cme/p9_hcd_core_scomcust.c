/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/cme/stop_cme/p9_hcd_core_scomcust.c $ */
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

#include "p9_cme_stop_exit_marks.h"

inline __attribute__((always_inline))
void
p9_hcd_core_scomcust(uint32_t core)
{
    cmeHeader_t* pCmeImgHdr  = (cmeHeader_t*)(CME_SRAM_HEADER_ADDR);
    ScomEntry_t* pCmeScomRes = 0;

    int i, core_mask;

    for (core_mask = 2; core_mask != 0; core_mask--)
    {

        if (core & core_mask)
        {
            pCmeScomRes = (ScomEntry_t*)(CME_SRAM_BASE_ADDR +
                                         (pCmeImgHdr->g_cme_scom_offset << 5) + ((pCmeImgHdr->g_cme_scom_length >> 1) * (core_mask & 1)));

            PK_TRACE("p9_hcd_core_scomcust core[%d] scom_offset [%08X] size = %d", core, (CME_SRAM_BASE_ADDR +
                     (pCmeImgHdr->g_cme_scom_offset << 5) + ((pCmeImgHdr->g_cme_scom_length >> 1) * (core_mask & 1))), sizeof(ScomEntry_t));

            for(i = 0; pCmeScomRes->scomEntryAddress; i++, pCmeScomRes++)
            {
                PK_TRACE("scom[%d] addr[%x] data[%08X%08X]",
                         i, pCmeScomRes->scomEntryAddress,
                         (pCmeScomRes->scomEntryData >> 32), (pCmeScomRes->scomEntryData & 0xFFFFFFFF));

                // Only scom the intended core
                CME_PUTSCOM(pCmeScomRes->scomEntryAddress, core_mask, pCmeScomRes->scomEntryData);
            }
        }
    }

    PK_TRACE("Drop chiplet fence via NC0INDIR[18]");
    CME_PUTSCOM(CPPM_NC0INDIR_CLR, core, BIT64(18));

#if !EPM_P9_TUNING

    data64_t scom_data = {0};

    for(core_mask = 2; core_mask; core_mask--)
    {
        if (core & core_mask)
        {
            PK_TRACE("Check Global Xstop FIR of Core Chiplet After Scom Restore");
            CME_GETSCOM(C_XFIR, core_mask, scom_data.value);

            if (scom_data.words.upper & BITS32(0, 27))
            {
                PK_TRACE_ERR("Core[%d] Chiplet Global Xstop FIR[%x] Detected After Scom Restore. HALT CME!",
                             core_mask, scom_data.words.upper);
                PK_PANIC(CME_STOP_EXIT_SCOM_RES_XSTOP_ERROR);
            }
        }
    }

#endif

}
