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
#include <libusb-1.0/libusb.h>
#include <signal.h>
#include <stdio.h>

#define DEVICE_DESC(vendor, product) ((((vendor))<<16) | (product))

uint32_t devices[] = {
	DEVICE_DESC(0x195d, 0x1016), // ASUS CLAW
	DEVICE_DESC(0x195d, 0x1017), // SAGITTA
	DEVICE_DESC(0x12cf, 0x700b), // COUGAR 700M
	0
};

static int do_exit = 0;

void callback(struct libusb_transfer *transfer);
void sighandler(int sig);
uint16_t vendor_of(uint32_t usb_device_desc);
uint16_t product_of(uint32_t usb_device_desc);

int main(void) {
    struct sigaction sigact;
    struct libusb_context *ctx;
    struct libusb_device **list;
    struct libusb_device_handle *handle;
    struct libusb_transfer *transfer;
    uint8_t data[8];
    int completed;

    libusb_init(&ctx);

    if (libusb_get_device_list(ctx, &list) < 0)
        goto fail_0;

    int i = 0;
    while (devices[i] != 0) {
	uint16_t vendor = (uint16_t)(devices[i] >> 16);
	uint16_t product = (uint16_t)(devices[i]);
	handle = libusb_open_device_with_vid_pid(ctx, vendor, product);

	if (devices[i] == 0) {
		goto fail_1;
	}
	if (handle == NULL) {
		i++;
		continue;
	}
	break;
    }

    // Interface 0x02, Endpoint 0x83 (EP3 IN)
    (void)libusb_detach_kernel_driver(handle, 2);
    if (libusb_claim_interface(handle, 2))
        goto fail_2;

    transfer = libusb_alloc_transfer(0);
    libusb_fill_interrupt_transfer(transfer, handle, 0x83, data,
				   sizeof(data), callback, &completed, 0);

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
    fprintf(stderr,"No device or no handle.\n");
    libusb_close(handle);
fail_1:
    fprintf(stderr,"Couldn't get device list.\n");
    libusb_free_device_list(list, 1);
fail_0:
    fprintf(stdout,"Quit.\n");
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

uint16_t vendor_of(uint32_t usb_device_desc) {
	return (uint16_t)(usb_device_desc >> 16);
}

uint16_t product_of(uint32_t usb_device_desc) {
	return (uint16_t)usb_device_desc;
}
