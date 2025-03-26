/**
 * @file mbox.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Mailbox header file.
 * @version 0.1
 * @date 2025-03-26
 *
 * Copyright (c) Riley Horrix 2025
 */

#ifndef DRIVERS_MBOX_H
#define DRIVERS_MBOX_H

#include "common/types.h"

/**
 * @brief Physical addresses for the VideoCore mailbox.
 *
 * The mailbox is setup in such a way that the CPU communicates to the VC through Mailbox 1 and the
 * VC to the CPU in Mailbox 0.
 *
 * This means that the CPU should never write to Mailbox 0, and the CPU should never read from
 * Mailbox 1.
 */
enum MailboxPhysicalAddress {
    MBOX_BASE     = 0x2000B880,
    MBOX_0_READ   = (MBOX_BASE + 0x00),
    MBOX_0_POLL   = (MBOX_BASE + 0x10),
    MBOX_0_SEND   = (MBOX_BASE + 0x14),
    MBOX_0_STATUS = (MBOX_BASE + 0x18),
    MBOX_0_CONFIG = (MBOX_BASE + 0x1c),
    MBOX_1_WRITE  = (MBOX_BASE + 0x20),
    MBOX_1_POLL   = (MBOX_BASE + 0x30),
    MBOX_1_SEND   = (MBOX_BASE + 0x34),
    MBOX_1_STATUS = (MBOX_BASE + 0x38),
    MBOX_1_CONFIG = (MBOX_BASE + 0x3c)
};

enum MailboxRequestCodes {
    MBOX_GET_BOARD_REVISION = 0x00010002,

};

enum MailboxCodes {
    MBOX_REQUEST_CODE     = 0x00000000,
    MBOX_REQUEST_SUCCEED  = 0x8000000,
    MBOX_REQUEST_FAILED   = 0x8000001,
    MBOX_TAG_REQUEST_CODE = 0x00000000,
    MBOX_END_TAG          = 0x00000000
};

enum MailboxStatus { MBOX_EMPTY = (0x1 << 30), MBOX_FULL = (0x1 << 31) };

enum MailboxChannels { MBOX_PROPERTY_CHANNEL = 0x8 };

/**
 * @brief Request the board revision number from the mailbox.
 *
 * @return u32_t Board revision number.
 */
u32_t mailbox_get_board_revision(void);

#endif