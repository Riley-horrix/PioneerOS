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

void mailbox_send(u32_t msg) {
    u32_t status;

    // Make sure you can send mail
    do {
        status = read_mmion(MBOX_0_STATUS);
    } while (status & MBOX_FULL);

    // send the message
    write_mmion(MBOX_1_WRITE, msg);
}

/**
 * @brief Call mailbox with specified buffer.
 *
 * This function must be given a buffer aligned to a 16 byte boundary.
 *
 * @param buffer The mailbox buffer.
 */
static void mailbox_call(u32_t* buffer) {
    // Get upper 28 bits of message address and combine with lower 4 bits
    // containing 4 bits of channel number.
    u32_t message = (((u32_t)buffer) & ~0xf) | ((u32_t)MBOX_PROPERTY_CHANNEL);

    mailbox_send(message);
    mailbox_read(MBOX_PROPERTY_CHANNEL);
    uart_puts("values : ");
    uart_puth(buffer[0]);
    uart_puts("\n\t");
    uart_puth(buffer[1]);
    uart_puts("\n\t");
    uart_puth(buffer[2]);
    uart_puts("\n\t");
    uart_puth(buffer[3]);
    uart_puts("\n\t");
    uart_puth(buffer[4]);
    uart_puts("\n\t");
    uart_puth(buffer[5]);
    uart_puts("\n\t");
    uart_puth(buffer[6]);
    uart_puts("\n\t");
    uart_puth(buffer[7]);
    uart_putch('\n');
}

#define MAX_MBOX_BUFFER 16

const u8_t requestBufferMap[] = {
    0x4, 0x4, 0x4, 0x6, 0x8, 0x8, 0x8,
    0x8, // Variable length depending on clocks
};

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

enum MailboxReturnStatus mailbox_request_property(enum MailboxRequestCodes code, u8_t* buffer) {
    u32_t mboxBuffer[MAX_MBOX_BUFFER] __attribute__((aligned(16)));

    u8_t valueBufferSize = mailbox_resolve_request_buffer_size(code);
    u32_t* mbuf_32       = mboxBuffer;
    u8_t* vbuf_8         = (u8_t*)(mbuf_32 + 5);
    uart_puts("[mbox] Buffer Address: ");
    uart_puth((u32_t)mboxBuffer);
    uart_puts("\n");
    // Setup header
    mbuf_32[0] =
        (0x6 * 4) + ((valueBufferSize + 3) & ~0x3); // This code rounds valueBufferSize up to the nearest
                                              // multiple of 4 to account for padding to 32 bits.
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

    for (int i = 0; i < 9; i++) {
        uart_puts("[mbox] buffer[");
        uart_puti(i);
        uart_puts("] = ");
        uart_puti(mboxBuffer[i]);
        uart_puts("\n");
    }

    mailbox_call(mboxBuffer);
    
    uart_puts("size : ");
    uart_puti(mbuf_32[4] & ~(1<<31));
    uart_puts("\n");

    if ((mbuf_32[4] & (1 << 31)) == 0) {
        return MBOX_ERROR_NO_RESPONSE;
    }

    if (mbuf_32[1] == MBOX_REQUEST_FAILED) {
        return MBOX_ERROR_PARSING_REQUEST;
    }



    for (int i = 0; i < (int)(mbuf_32[4] & ~(1 << 31)); i++) {
        buffer[i] = vbuf_8[i];
    }

    return MBOX_RETURN_OK;    
}

u32_t mailbox_get_board_revision(void) {
    u32_t buffer[7] __attribute__((aligned(16)));

    buffer[0] = 7 * sizeof(u32_t); // buffer size in bytes
    buffer[1] = MBOX_REQUEST_CODE;
    // tags begin
    buffer[2] = MBOX_GET_BOARD_MODEL; // tag identifier
    buffer[3] = 4;                    // maximum of request and response value buffer's length.
    buffer[4] = MBOX_TAG_REQUEST_CODE;
    buffer[5] = 0; // value buffer
    // tags end
    buffer[6] = MBOX_END_TAG;

    mailbox_call(buffer);

    return buffer[5];
}