#include "drivers/mbox.h"
#include "common/common.h"
#include "common/mmio.h"
#include "common/types.h"
#include "drivers/uart.h"

static u32_t mailbox_read(u32_t channel) {
    // Make sure that the message is from the right channel
    u32_t result;
    u32_t status;
    do {
        // Make sure there is mail to recieve
        do {
            status = read_mmion(MBOX_0_STATUS);
            uart_putch('a');
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
    u32_t message = ((u32_t)buffer) & ((u32_t)MBOX_PROPERTY_CHANNEL);

    if (((u32_t)buffer & 0xf) != 0) {
        uart_puts("[mbox] Buffer is not 16 byte aligned!\n");
        return;
    }

    mailbox_send(message);
    buffer[5] = mailbox_read(MBOX_PROPERTY_CHANNEL);
}

#define BOARD_REVISION_BUFFER_SIZE 7

u32_t mailbox_get_board_revision(void) {
    u32_t buffer[BOARD_REVISION_BUFFER_SIZE] __attribute__((aligned(16)));

    buffer[0] = BOARD_REVISION_BUFFER_SIZE * sizeof(u32_t); // buffer size in bytes
    buffer[1] = MBOX_REQUEST_CODE;
    // tags begin
    buffer[2] = MBOX_GET_BOARD_REVISION; // tag identifier
    buffer[3] = 4;                       // maximum of request and response value buffer's length.
    buffer[4] = MBOX_TAG_REQUEST_CODE;
    buffer[5] = 0; // value buffer
    // tags end
    buffer[6] = MBOX_END_TAG;

    mailbox_call(buffer);

    return buffer[5];
}