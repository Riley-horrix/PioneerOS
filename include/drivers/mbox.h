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
    MBOX_GET_FIRMWARE_MODEL = 0x00000001,
    MBOX_GET_BOARD_MODEL    = 0x00010001,
    MBOX_GET_BOARD_REVISION = 0x00010002,
    MBOX_GET_BOARD_MAC_ADDR = 0x00010003,
    MBOX_GET_BOARD_SERIAL   = 0x00010004,
    MBOX_GET_ARM_MEMORY     = 0x00010005,
    MBOX_GET_VC_MEMORY      = 0x00010006,
    MBOX_GET_CLOCKS         = 0x00010007
};

enum MailboxCodes {
    MBOX_REQUEST_CODE     = 0x00000000,
    MBOX_REQUEST_SUCCEED  = 0x8000000,
    MBOX_REQUEST_FAILED   = 0x8000001,
    MBOX_TAG_REQUEST_CODE = 0x00000000,
    MBOX_END_TAG          = 0x00000000
};

enum MailboxStatus { MBOX_EMPTY = (0x1 << 30), MBOX_FULL = (0x1 << 31) };

enum MailboxReturnStatus {
    MBOX_RETURN_OK = 0,
    MBOX_ERROR_PARSING_REQUEST = -1,
    MBOX_ERROR_NO_RESPONSE = -2
};

enum MailboxChannels { MBOX_PROPERTY_CHANNEL = 0x8 };

/**
 * @brief Request the board revision number from the mailbox.
 *
 * @return u32_t Board revision number.
 */
u32_t mailbox_get_board_revision(void);

/**
 * @brief Request a property through the mailbox interface. Returns an integer status code, MailboxReturnStatus.
 *
 * @param code The mailbox property to request.
 * @param buffer Return buffer, this must be large enough to support the full buffer. The size of
 * the required buffer can be found out by calling mailbox_resolve_buffer_size()
 */
enum MailboxReturnStatus mailbox_request_property(enum MailboxRequestCodes code, u8_t* buffer);

/**
 * @brief Find out how large of a buffer must be allocated to store a request to the mailbox with
 * this code.
 *
 * @param code
 * @return int
 */
int mailbox_resolve_request_buffer_size(enum MailboxRequestCodes code);

#endif