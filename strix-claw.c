// strix-claw -- prevents hang of Strix Claw
//  Copyright (C) 2016 Tristan Šneider
//  Copyright (C) 2016 Oleksandr Novychenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include <signal.h>
#include <libusb-1.0/libusb.h>

void callback(struct libusb_transfer *transfer);
void sighandler(int sig);

static int do_exit = 0;

int main(void) {
    struct sigaction sigact;
    struct libusb_context *ctx;
    struct libusb_device **list;
    struct libusb_device_handle *handle;
    struct libusb_transfer *transfer;
    unsigned char data[8];
    int completed;

    libusb_init(&ctx);

    if(libusb_get_device_list(ctx, &list) < 0)
        goto fail_0;

    if ((handle = libusb_open_device_with_vid_pid(ctx, 0x195d, 0x1016)) == NULL)
        goto fail_1;

    // Interface 0x02, Endpoint 0x83 (EP3 IN)
    (void)libusb_detach_kernel_driver(handle, 2);
    if (libusb_claim_interface(handle, 2))
        goto fail_2;

    transfer = libusb_alloc_transfer(0);
    libusb_fill_interrupt_transfer(transfer, handle, 0x83, data, sizeof(data), callback, &completed, 0);

    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);

    while (!do_exit) {
        completed = 0;
        if (libusb_submit_transfer(transfer) != 0)
            break;

        while (!completed) {
            if (do_exit)
                libusb_cancel_transfer(transfer);

            libusb_handle_events_completed(ctx, &completed);
        }
    }

    libusb_free_transfer(transfer);
fail_2:
    libusb_close(handle);
fail_1:
    libusb_free_device_list(list, 1);
fail_0:
    libusb_exit(ctx);
    return 0;
}

void callback(struct libusb_transfer *transfer) {
    *(int *)transfer->user_data = 1;
}

void sighandler(int sig) {
    (void)sig;
    do_exit = 1;
}
