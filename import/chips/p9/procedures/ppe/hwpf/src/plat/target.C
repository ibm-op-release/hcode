/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe/hwpf/src/plat/target.C $       */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2012,2017                                                    */
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


#include <fapi2.H>
//#include <plat_target_pg_attributes.H>
#include <assert.h>
#include <fapi2_target.H>
#include <p9_misc_scom_addresses.H>

using namespace fapi2;

#if 0

    uint32_t CHIPLET_PG_ARRAY_ENTRIES = sizeof(CHIPLET_PG_ARRAY) /
    sizeof(chiplet_pg_entry_t);
#endif
// Global Vector containing ALL targets.  This structure is referenced by
// getChildren to produce the resultant returned vector from that
// call.
std::vector<plat_target_handle_t> G_vec_targets;

#if 0
    // Global variable for fixed section in pibmem
    G_sbe_attrs_t G_sbe_attrs;

    fapi2attr::SystemAttributes_t*    G_system_attributes_ptr;
    fapi2attr::ProcChipAttributes_t*  G_proc_chip_attributes_ptr;
    fapi2attr::PervAttributes_t*      G_perv_attributes_ptr;
    fapi2attr::CoreAttributes_t*      G_core_attributes_ptr;
    fapi2attr::EQAttributes_t*        G_eq_attributes_ptr;
    fapi2attr::EXAttributes_t*        G_ex_attributes_ptr;
#endif

namespace fapi2
{

#ifndef __noRC__
    ReturnCode current_err;
#endif


// Not a fan of the switch technique;  I would prefer an array lookup
// but the attritute lookup is done via compile time macros that are
// resolved to attribute Ids (hashes).
ReturnCode plat_PervPGTargets(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
                              const TargetTypes_t i_chiplet_num,
                              bool& o_present)
{

    o_present = true;
#if 0
    // TODO: Attribute supprt for SGPE?
    uint16_t attr_value = 0;

    switch (i_chiplet_num)
    {
        case 0x00:   // Nop
            break;

        case 0x01:
            FAPI_ATTR_GET(ATTR_PG_PRV, i_target, *(&attr_value));
            FAPI_DBG("ATTR_PG_PRV value = %x", attr_value);
            break;

        case 0x02:
            FAPI_ATTR_GET(ATTR_PG_N0, i_target, attr_value);
            FAPI_DBG("ATTR_PG_N0 value = %x", attr_value);
            break;

        case 0x03:
            FAPI_ATTR_GET(ATTR_PG_N1, i_target, attr_value);
            FAPI_DBG("ATTR_PG_N1 value = %x", attr_value);
            break;

        case 0x04:
            FAPI_ATTR_GET(ATTR_PG_N2, i_target, attr_value);
            FAPI_DBG("ATTR_PG_N2 value = %x", attr_value);
            break;

        case 0x05:
            FAPI_ATTR_GET(ATTR_PG_N3, i_target, attr_value);
            FAPI_DBG("ATTR_PG_N0 value = %x", attr_value);
            break;

        case 0x06:
            FAPI_ATTR_GET(ATTR_PG_XB, i_target, attr_value);
            FAPI_DBG("ATTR_PG_XB value = %x", attr_value);
            break;

        case 0x07:
            FAPI_ATTR_GET(ATTR_PG_MC01, i_target, attr_value);
            FAPI_DBG("ATTR_PG_MC01 value = %x", attr_value);
            break;

        case 0x08:
            FAPI_ATTR_GET(ATTR_PG_MC23, i_target, attr_value);
            FAPI_DBG("ATTR_PG_MC23 value = %x", attr_value);
            break;

        case 0x09:
            FAPI_ATTR_GET(ATTR_PG_OB0, i_target, attr_value);
            FAPI_DBG("ATTR_PG_OB0 value = %x", attr_value);
            break;

        case 0x0A:
            FAPI_ATTR_GET(ATTR_PG_OB1, i_target, attr_value);
            FAPI_DBG("ATTR_PG_OB1 value = %x", attr_value);
            break;

        case 0x0B:
            FAPI_ATTR_GET(ATTR_PG_OB2, i_target, attr_value);
            FAPI_DBG("ATTR_PG_OB2 value = %x", attr_value);
            break;

        case 0x0C:
            FAPI_ATTR_GET(ATTR_PG_OB3, i_target, attr_value);
            FAPI_DBG("ATTR_PG_OB3 value = %x", attr_value);
            break;

        case 0x0D:
            FAPI_ATTR_GET(ATTR_PG_PCI0, i_target, attr_value);
            FAPI_DBG("ATTR_PG_PCI0 value = %x", attr_value);
            break;

        case 0x0E:
            FAPI_ATTR_GET(ATTR_PG_PCI1, i_target, attr_value);
            FAPI_DBG("ATTR_PG_PCI1 value = %x", attr_value);
            break;

        case 0x0F:
            FAPI_ATTR_GET(ATTR_PG_PCI2, i_target, attr_value);
            FAPI_DBG("ATTR_PG_PCI2 value = %x", attr_value);
            break;

        case 0x10:
            FAPI_ATTR_GET(ATTR_PG_EQ0, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EQ0 value = %x", attr_value);
            break;

        case 0x11:
            FAPI_ATTR_GET(ATTR_PG_EQ1, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EQ1 value = %x", attr_value);
            break;

        case 0x12:
            FAPI_ATTR_GET(ATTR_PG_EQ2, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EQ2 value = %x", attr_value);
            break;

        case 0x13:
            FAPI_ATTR_GET(ATTR_PG_EQ3, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EQ3 value = %x", attr_value);
            break;

        case 0x14:
            FAPI_ATTR_GET(ATTR_PG_EQ4, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EQ4 value = %x", attr_value);
            break;

        case 0x15:
            FAPI_ATTR_GET(ATTR_PG_EQ5, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EQ5 value = %x", attr_value);
            break;

        case 0x20:
            FAPI_ATTR_GET(ATTR_PG_EC00, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC00 value = %x", attr_value);
            break;

        case 0x21:
            FAPI_ATTR_GET(ATTR_PG_EC01, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC01 value = %x", attr_value);
            break;

        case 0x22:
            FAPI_ATTR_GET(ATTR_PG_EC02, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC02 value = %x", attr_value);
            break;

        case 0x23:
            FAPI_ATTR_GET(ATTR_PG_EC03, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC03 value = %x", attr_value);
            break;

        case 0x24:
            FAPI_ATTR_GET(ATTR_PG_EC04, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC04 value = %x", attr_value);
            break;

        case 0x25:
            FAPI_ATTR_GET(ATTR_PG_EC05, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC05 value = %x", attr_value);
            break;

        case 0x26:
            FAPI_ATTR_GET(ATTR_PG_EC06, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC06 value = %x", attr_value);
            break;

        case 0x27:
            FAPI_ATTR_GET(ATTR_PG_EC07, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC07 value = %x", attr_value);
            break;

        case 0x28:
            FAPI_ATTR_GET(ATTR_PG_EC08, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC08 value = %x", attr_value);
            break;

        case 0x29:
            FAPI_ATTR_GET(ATTR_PG_EC09, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC09 value = %x", attr_value);
            break;

        case 0x2A:
            FAPI_ATTR_GET(ATTR_PG_EC10, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC10 value = %x", attr_value);
            break;

        case 0x2B:
            FAPI_ATTR_GET(ATTR_PG_EC11, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC11 value = %x", attr_value);
            break;

        case 0x2C:
            FAPI_ATTR_GET(ATTR_PG_EC12, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC12 value = %x", attr_value);
            break;

        case 0x2D:
            FAPI_ATTR_GET(ATTR_PG_EC13, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC13 value = %x", attr_value);
            break;

        case 0x2E:
            FAPI_ATTR_GET(ATTR_PG_EC14, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC14 value = %x", attr_value);
            break;

        case 0x2F:
            FAPI_ATTR_GET(ATTR_PG_EC15, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC15 value = %x", attr_value);
            break;

        case 0x30:
            FAPI_ATTR_GET(ATTR_PG_EC16, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC16 value = %x", attr_value);
            break;

        case 0x31:
            FAPI_ATTR_GET(ATTR_PG_EC17, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC17 value = %x", attr_value);
            break;

        case 0x32:
            FAPI_ATTR_GET(ATTR_PG_EC18, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC18 value = %x", attr_value);
            break;

        case 0x33:
            FAPI_ATTR_GET(ATTR_PG_EC19, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC19 value = %x", attr_value);
            break;

        case 0x34:
            FAPI_ATTR_GET(ATTR_PG_EC20, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC20 value = %x", attr_value);
            break;

        case 0x35:
            FAPI_ATTR_GET(ATTR_PG_EC21, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC21 value = %x", attr_value);
            break;

        case 0x36:
            FAPI_ATTR_GET(ATTR_PG_EC22, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC22 value = %x", attr_value);
            break;

        case 0x37:
            FAPI_ATTR_GET(ATTR_PG_EC23, i_target, attr_value);
            FAPI_DBG("ATTR_PG_EC23 value = %x", attr_value);
            break;

        default:
            FAPI_ERR("PervPGTargets:  invalid chiplet number %u", i_chiplet_num);
    }

    if (attr_value & 0xC000)
    {
        o_present = true;
    }

#endif
    return FAPI2_RC_SUCCESS;

}

/// @brief Function to determine if pervsaive target within a chip is
///     present and, thus, considered functional per PG attributes
template<TargetType K>
ReturnCode
plat_TargetPresent( Target<TARGET_TYPE_PROC_CHIP>& i_chip_target,
                    Target<K>& i_chiplet_target,
                    bool& b_present)
{
    // TODO: Needs to be revisited for the SGPE, no ATTR_PG
    // Find the PERV target number in the partial good initialization
    // array
    ChipletNumber_t chiplet_number = i_chiplet_target.getChipletNumber();

    FAPI_TRY(plat_PervPGTargets(i_chip_target, chiplet_number, b_present));

    if (b_present)
    {
        i_chiplet_target.setPresent();
        i_chiplet_target.setFunctional(true);
    }
    else
    {
        FAPI_DBG("Perv target NOT present (nor functional): chiplet_number = %d",
                 chiplet_number);
    }

    FAPI_DBG("Target present = %u, Target functional = %u",
             i_chiplet_target.getPresent(),
             i_chiplet_target.getFunctional());

fapi_try_exit:
    return current_err;
}


/// @brief Function to initialize the G_targets vector based on partial good
///      attributes ///  this will move to plat_target.H formally
ReturnCode plat_TargetsInit()
{
    bool b_present = false;

#if 0
    // Copy fixed section from SEEPROM to PIBMEM
    G_sbe_attrs.G_system_attrs = G_system_attributes;
    G_sbe_attrs.G_proc_chip_attrs = G_proc_chip_attributes;
    G_sbe_attrs.G_perv_attrs = G_perv_attributes;
    G_sbe_attrs.G_core_attrs = G_core_attributes;
    G_sbe_attrs.G_eq_attrs = G_eq_attributes;
    G_sbe_attrs.G_ex_attrs = G_ex_attributes;

    // Initialise global attribute pointers
    G_system_attributes_ptr = &(G_sbe_attrs.G_system_attrs);
    G_proc_chip_attributes_ptr = &(G_sbe_attrs.G_proc_chip_attrs);
    G_perv_attributes_ptr = &(G_sbe_attrs.G_perv_attrs);
    G_core_attributes_ptr = &(G_sbe_attrs.G_core_attrs);
    G_eq_attributes_ptr = &(G_sbe_attrs.G_eq_attrs);
    G_ex_attributes_ptr = &(G_sbe_attrs.G_ex_attrs);

#endif
    std::vector<plat_target_handle_t>::iterator tgt_iter;
    uint32_t l_beginning_offset;
    uint64_t l_qcsr = 0;
    uint64_t l_ccsr = 0;

    FAPI_DBG("Platform target initialization.  Target Count = %u", TARGET_COUNT);

    // Read the QCSR and QSSR registers to set the functional state of EC, EQ
    // and EX targets

    // Not checking of return code as SGPE has a stop-on-error policy
    getscom_abs(PU_OCB_OCI_QCSR_SCOM, &l_qcsr);
    getscom_abs(PU_OCB_OCI_CCSR_SCOM, &l_ccsr);

    /*
     * Initialize all entries to NULL
     */
    for (uint32_t i = 0; i < TARGET_COUNT; ++i)
    {
        G_vec_targets.push_back((plat_target_handle_t)0x0);
    }

    /*
     * Chip Target is the first one
     */
    l_beginning_offset = CHIP_TARGET_OFFSET;

    Target<TARGET_TYPE_PROC_CHIP> chip_target((plat_target_handle_t)0);
    G_vec_targets.at(l_beginning_offset) = revle32((plat_target_handle_t)(chip_target.get()));

    /*
     * Nest Targets - group 1
     */
    l_beginning_offset = NEST_GROUP1_TARGET_OFFSET;

    for (uint32_t i = 0; i < NEST_GROUP1_TARGET_COUNT; ++i)
    {
        Target<TARGET_TYPE_PERV> target_name((plat_target_handle_t)i);

        // Determine if the chiplet is present and, thus, functional
        // via partial good attributes
        FAPI_TRY(plat_TargetPresent(chip_target, target_name, b_present));

        G_vec_targets.at(l_beginning_offset + i) = revle32((plat_target_handle_t)(target_name.get()));
    }

    /*
     * Memory Controller Synchronous (MCS) Targets
     */

    l_beginning_offset = MCS_TARGET_OFFSET;

    for (uint32_t i = 0; i < MCS_TARGET_COUNT; ++i)
    {
        Target<TARGET_TYPE_MCS> target_name((plat_target_handle_t)i);

        // Determine if the chiplet is present and, thus, functional
        // via partial good attributes
        FAPI_TRY(plat_TargetPresent(chip_target, target_name, b_present));

        G_vec_targets.at(l_beginning_offset + i) = revle32((plat_target_handle_t)(target_name.get()));

    }

    /*
     * Nest Targets - group 2
     */
    l_beginning_offset = NEST_GROUP2_TARGET_OFFSET;

    for (uint32_t i = NEST_GROUP2_TARGET_OFFSET;
         i < (NEST_GROUP2_TARGET_OFFSET + NEST_GROUP2_TARGET_COUNT); ++i)
    {
        Target<TARGET_TYPE_PERV> target_name((plat_target_handle_t)(i - 1));

        // Determine if the chiplet is present and, thus, functional
        // via partial good attributes
        FAPI_TRY(plat_TargetPresent(chip_target, target_name, b_present));

        G_vec_targets.at(i) = revle32((plat_target_handle_t)(target_name.get()));
    }

    /*
     * Cache (EQ) Targets
     */
    l_beginning_offset = EQ_TARGET_OFFSET;

    for (uint32_t i = 0; i < EQ_TARGET_COUNT; ++i)
    {
        Target<TARGET_TYPE_EQ> target_name((plat_target_handle_t)i);
        plat_target_handle_t l_hndl = target_name.get();

        // EQ is good if at lest one of it's EX is good
        if(l_qcsr & BITS(i, 2))
        {
            target_name.setPresent();
            target_name.setFunctional(true);
        }

        G_vec_targets.at(l_beginning_offset + i) = revle32(l_hndl);
    }

    /*
     * Core (EC) Targets
     */

    l_beginning_offset = CORE_TARGET_OFFSET;

    for (uint32_t i = 0; i < CORE_TARGET_COUNT; ++i)
    {
        Target<TARGET_TYPE_CORE> target_name((plat_target_handle_t)i);

        // Check goodness from CCSR
        if(l_ccsr & BITS(i, 1))
        {
            target_name.setPresent();
            target_name.setFunctional(true);
        }

        G_vec_targets.at(l_beginning_offset + i) = revle32((plat_target_handle_t)(target_name.get()));
    }

    /*
     * EX Targets
     */

    l_beginning_offset = EX_TARGET_OFFSET;

    for (uint32_t i = 0; i < EX_TARGET_COUNT; ++i)
    {
        Target<TARGET_TYPE_EX> target_name((plat_target_handle_t)i);

        // Check goodness from QCSR
        if(l_qcsr & BITS(i, 1))
        {
            target_name.setPresent();
            target_name.setFunctional(true);
        }

        G_vec_targets.at(l_beginning_offset + i) = revle32((plat_target_handle_t)(target_name.get()));
    }

fapi_try_exit:
    return current_err;
}

/// @brief Function to initialize the G_targets vector based on partial good
///        attributes
Target<TARGET_TYPE_PROC_CHIP> plat_getChipTarget()
{

    // Get the chip specific target
    return ((Target<TARGET_TYPE_PROC_CHIP>)G_vec_targets.at(0));
}

/// @brief Function to return a platform target handle, given the chiplet
//         number
//  @param i_chipletNumber The chiplet number of the target
//  @return Platform target handle
//  @note The caller can use the platform target handle to construct a
//        Target of it's choice. Ex:
//  Target<TARGET_TYPE_CORE>
//  l_core(plat_getTargetHandleByChipletNumber(0x20);
plat_target_handle_t plat_getTargetHandleByChipletNumber(
    const uint8_t i_chipletNumber)
{
    assert(((i_chipletNumber > 0) &&
            (i_chipletNumber < (EQ_CHIPLET_OFFSET + EQ_TARGET_COUNT))) ||
           ((i_chipletNumber >= CORE_CHIPLET_OFFSET) &&
            (i_chipletNumber < (CORE_CHIPLET_OFFSET + CORE_TARGET_COUNT))));

    uint32_t l_idx = 0;

    if(i_chipletNumber < (EQ_CHIPLET_OFFSET + EQ_TARGET_COUNT))
    {
        l_idx = (i_chipletNumber - NEST_GROUP1_CHIPLET_OFFSET) +
                NEST_GROUP1_TARGET_OFFSET;
    }
    else
    {
        l_idx = (i_chipletNumber - CORE_CHIPLET_OFFSET) +
                CORE_TARGET_OFFSET;
    }

    return G_vec_targets[l_idx];
}

} // fapi2
