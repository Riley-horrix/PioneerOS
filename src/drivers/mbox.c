/**
 * @file mbox.c
 * @author Riley Horrix (riley@horrix.com)
 * @brief Mailbox function implementations.
 * @version 0.1
 * @date 2025-03-26
 *
 * Copyright (c) Riley Horrix 2025
 */

#include "drivers/mbox.h"
#include "common/common.h"
#include "common/mmio.h"
#include "common/types.h"
#include "drivers/uart.h"

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

enum MailboxCodes {
    MBOX_REQUEST_CODE     = 0x00000000,
    MBOX_REQUEST_SUCCEED  = 0x8000000,
    MBOX_REQUEST_FAILED   = 0x8000001,
    MBOX_TAG_REQUEST_CODE = 0x00000000,
    MBOX_END_TAG          = 0x00000000
};

enum MailboxStatus { MBOX_EMPTY = (0x1 << 30), MBOX_FULL = (0x1 << 31) };

#define MAX_MBOX_BUFFER 16

static u32_t mailbox_read(enum MailboxChannels channel) {
    // Make sure that the message is from the right channel
    u32_t result;
    u32_t status;
    do {
        // Make sure there is mail to recieve
        do {
            status = read_mmion(MBOX_0_STATUS);
        } while (status & MBOX_EMPTY);

        // Get the message
        result = read_mmion(MBOX_0_READ);
    } while ((result & 0xf) != channel);

    return result;
}

static void mailbox_send(u32_t msg) {
    u32_t status;

    // Make sure you can send mail
    do {
        status = read_mmion(MBOX_0_STATUS);
    } while (status & MBOX_FULL);

    // Send the message
    write_mmion(MBOX_1_WRITE, msg);
}

/**
 * @brief Call mailbox with specified buffer.
 *
 * This function must be given a buffer aligned to a 16 byte boundary.
 *
 * @param buffer The mailbox buffer.
 */
static void mailbox_call(u32_t* buffer, enum MailboxChannels channel) {
    // Get upper 28 bits of message address and combine with lower 4 bits
    // containing 4 bits of channel number.
    u32_t message = ((u32_t)buffer & ~0xf) | ((u32_t)channel & 0xf);

    mailbox_send(message);
    mailbox_read(channel);
}

/**
 * @brief Find out how large of a buffer must be allocated to store a request to the mailbox with
 * this code.
 *
 * Note that `MBOX_GET_CLOCKS` can return a multiple of the returned required buffer size. The
 * multiple
 *
 * @param code The request code.
 * @return int The required size of the buffer in bytes. 0 returned if `code` is not valid.
 */
int mailbox_resolve_request_buffer_size(enum MailboxRequestCodes code) {
    switch (code) {
    case MBOX_GET_FIRMWARE_MODEL:
        return 0x4;
        break;
    case MBOX_GET_BOARD_MODEL:
        return 0x4;
        break;
    case MBOX_GET_BOARD_REVISION:
        return 0x4;
        break;
    case MBOX_GET_BOARD_MAC_ADDR:
        return 0x6;
        break;
    case MBOX_GET_BOARD_SERIAL:
        return 0x8;
        break;
    case MBOX_GET_ARM_MEMORY:
        return 0x8;
        break;
    case MBOX_GET_VC_MEMORY:
        return 0x8;
        break;
    case MBOX_GET_CLOCKS:
        return 0x8;
        break;

    default:
        return 0;
        break;
    }
}

/**
 * @brief Request a property through the mailbox interface. Returns an integer status code,
 * MailboxReturnStatus.
 *
 * @param code The mailbox property to request.
 * @param buffer Return buffer, this must be large enough to support the full buffer. The size of
 * the required buffer can be found out by calling mailbox_resolve_buffer_size()
 */
enum MailboxReturnStatus mailbox_request_property(enum MailboxRequestCodes code, u8_t* buffer) {
    u32_t mboxBuffer[MAX_MBOX_BUFFER] __attribute__((aligned(16)));

    u8_t valueBufferSize = mailbox_resolve_request_buffer_size(code);
    u32_t* mbuf_32       = mboxBuffer;
    u8_t* vbuf_8         = (u8_t*)(mbuf_32 + 5);

    // Setup header
    mbuf_32[0] = (0x6 * 4) + ((valueBufferSize + 3) &
                              ~0x3); // This code rounds valueBufferSize up to the nearest
                                     // multiple of 4 to account for padding to 32 bits.

    // Request from the mailbox.
    mbuf_32[1] = MBOX_REQUEST_CODE;

    // Tag id.
    mbuf_32[2] = code;

    // Value buffer size in bytes.
    mbuf_32[3] = valueBufferSize;

    // Request code.
    mbuf_32[4] = MBOX_TAG_REQUEST_CODE;

    // Init value buffer to 0x0
    for (int i = 0; i < valueBufferSize; i++) {
        vbuf_8[i] = 0x0;
    }

    // Padding goes here.
    // Setup footer.
    mbuf_32[5 + valueBufferSize / 4] = MBOX_END_TAG;

    mailbox_call(mboxBuffer, MBOX_PROPERTY_CHANNEL);

    if ((mbuf_32[4] & (1 << 31)) == 0) {
        return MBOX_ERROR_NO_RESPONSE;
    }

    if (mbuf_32[1] == MBOX_REQUEST_FAILED) {
        return MBOX_ERROR_PARSING_REQUEST;
    }

    // Write output into buffer
    for (int i = 0; i < (int)(mbuf_32[4] & ~(1 << 31)); i++) {
        buffer[i] = vbuf_8[i];
    }

    return MBOX_RETURN_OK;
}
