# Netlink [![Language: C](https://img.shields.io/badge/Language-C-brightgreen.svg)](https://en.wikipedia.org/wiki/C_(programming_language)) [![Builder: CMake](https://img.shields.io/badge/Builder-CMake-brightgreen.svg)](https://cmake.org/)  [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

## Introduction

The Netlink socket family is a Linux kernel interface used for inter-process communication (IPC) between both the kernel and userspace processes, and between different userspace processes.

The netlink API project aims to provides functions to manipulates netlink connections/messages easily.

## Setting up netlink example

Clone the project to retrieve the sources:
```
$>git clone https://github.com/pestbuns/netlink.git
```

Go in the build directory of netlink:
```
$>cd netlink/build/
```

Build the project using CMake (from the build directory):
```
$>cmake ..
```

Finally, use make to compile the sources and so generate the binary (still from the build directory):
```
$>make
```

## API Description (netlink.h content):

```
/*! Opaque type for the netlink handler */
typedef struct nl nl_t;


/*!
 * \brief   Signature of user netlink callback
 *
 * This callback will be triggered when a NETLINK_KOBJECT_UEVENT msg is received
 *
 * \param   msglen  IN     size of the message received
 * \param   msg     IN     netlink message received
 */
typedef void (*nl_reader_cb_t)(unsigned int msglen, char *msg, void *ctx);


/*!
 * \brief   Create a netlink handler
 *
 * \param   cb          IN  callback triggered when a netlink mesage is received
 * \param   buffer_size IN  size of the buffer that will received the message
 *                           - if too small, the message will be truncated.
 * \param   ctx         IN  user context
 *
 * \return  netlink handler in case of success, NULL in case of error
 */
nl_t *nl_init_handler(nl_reader_cb_t cb, unsigned int buffer_size, void *ctx);


/*!
 * \brief   Launch a netlink listener over the handler
 *
 * Launch poll loop over a netlink socket that waits for events.
 * When a netlink message is received, the nl_reader_cb_t callback is triggered
 *
 * \param   hdl IN  netlink handler
 *
 * \return  -1 in case of failure, 0 in case of success
 */
int nl_launch_listener(nl_t *hdl);


/*!
 * \brief   Delete a netlink handler
 *
 * \param   hdl IN  netlink handler to free
 */
void nl_deinit_handler(nl_t *hdl);
```

## Example Usage:
Launch netlink:
```
$>./netlink
```

## Example Output (USB plugged-in):

```
Cli>./netlink
Waiting for events ...
msglen[248]:[add@/devices/pci0000:00/0000:00:14.0/usb1/1-1]
msglen[279]:[add@/devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1:1.0]
msglen[609]:[libudev]
msglen[86]:[add@/module/scsi_mod]
msglen[85]:[add@/class/scsi_host]
msglen[69]:[add@/bus/scsi]
msglen[89]:[add@/class/scsi_device]
msglen[133]:[libudev]
msglen[134]:[libudev]
msglen[132]:[libudev]
msglen[92]:[add@/module/usb_storage]
msglen[180]:[add@/devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1:1.0/host0]
msglen[199]:[add@/devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1:1.0/host0/scsi_host/host0]
msglen[111]:[add@/bus/usb/drivers/usb-storage]
msglen[136]:[libudev]
msglen[146]:[libudev]
msglen[123]:[libudev]
msglen[76]:[add@/module/uas]
msglen[95]:[add@/bus/usb/drivers/uas]
...
```

## More info:

* [Wikipedia Netlink](https://en.wikipedia.org/wiki/Netlink) - Netlink Introduction
* [man (7) netlink](http://man7.org/linux/man-pages/man7/netlink.7.html) - Netlink Linux Programmer's Manual

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
