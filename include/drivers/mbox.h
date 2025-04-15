/**
 * @file mbox.h
 * @author Riley Horrix (riley@horrix.com)
 * @brief Mailbox header file.
 * @version 0.1
 * @date 2025-03-26
 *
 * Information from :
 * https://grasslab.github.io/NYCU_Operating_System_Capstone/hardware/mailbox.html?highlight=mailbox
 * https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
 *
 *
 * Copyright (c) Riley Horrix 2025
 */

#ifndef DRIVERS_MBOX_H
#define DRIVERS_MBOX_H

#include "common/types.h"

/**
 * @brief Represents the possible mailbox property request codes.
 *
 */
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

/**
 * @brief Return status codes from the mailbox interface.
 *
 */
enum MailboxReturnStatus {
    MBOX_GOOD                  = 0,
    MBOX_ERROR_PARSING_REQUEST = -1,
    MBOX_ERROR_NO_RESPONSE     = -2,
    MBOX_ITER_NO_MORE_SEGMENTS = -3,
    MM_FAILED_INIT             = -4,
};

/**
 * @brief Represents the supported mailbox channels.
 *
 */
enum MailboxChannels { MBOX_PROPERTY_CHANNEL = 0x8 };

/**
 * @brief Bit translation of the value returned by getting the board revision number from the
 * mailbox.
 *
 * This struct can be passed into `mailbox_request_property` as the buffer if the code is
 * MBOX_GET_BOARD_REVISION.
 *
 * For an explanation of the meaning of each value, go to
 * https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#new-style-revision-codes
 */
struct MailboxBoardRevision {
    u32_t revision : 4;
    u32_t type : 8;
    u32_t processor : 4;
    u32_t manufacturer : 4;
    u32_t memory : 3;
    u32_t revStyle : 1;
    u32_t unused_0 : 1;
    u32_t warranty : 1;
    u32_t unused_1 : 3;
    u32_t otpRead : 1;
    u32_t otpProgram : 1;
    u32_t overVoltage : 1;
} __attribute__((packed));

/**
 * @brief Represents the memory information returned by the mailbox.
 *
 * This struct can be passed into `mailbox_request_property` as the buffer if the code is
 * MBOX_GET_ARM_MEMORY or MBOX_GET_VC_MEMORY.
 */
struct MailboxMemoryIterator {
    u32_t base;
    u32_t size;
    int index;      // Used by mbox.c, do not modify.
    bool is_arm;
};

int mailbox_request_buffer_size(enum MailboxRequestCodes code);

enum MailboxReturnStatus mailbox_request_property(enum MailboxRequestCodes code, u8_t* buffer);

inline enum MailboxReturnStatus mailbox_request_brev(struct MailboxBoardRevision* buffer) {
    return mailbox_request_property(MBOX_GET_BOARD_REVISION, (u8_t*)buffer);
}

enum MailboxReturnStatus mailbox_mem_iter_init(struct MailboxMemoryIterator* iter, bool is_arm);
enum MailboxReturnStatus mailbox_mem_iter_next(struct MailboxMemoryIterator* iter);


#endif