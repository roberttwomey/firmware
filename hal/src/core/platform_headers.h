/*
 * File:   platform_headers.h
 * Author: mat
 *
 * Created on 10 October 2014, 11:02
 */

#ifndef PLATFORM_HEADERS_H
#define	PLATFORM_HEADERS_H

#ifdef	__cplusplus
extern "C" {
#endif


// These headers contain platform-specific defines.
#include "pinmap_hal.h"
#include "hw_config.h"
#include "pinmap_impl.h"
#include "cc3000_common.h"
#include "hci.h"
#include "wlan.h"
#include "socket.h"
#include "nvmem.h"
#include "netapp.h"


#ifdef	__cplusplus
}
#endif

#endif	/* PLATFORM_HEADERS_H */

