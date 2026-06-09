/* HERMES Modem
 *
 * Copyright (C) 2025 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>
#include <stddef.h>

#ifndef FRAMER_H
#define FRAMER_H

/*
 * Mercury routes decoded modem frames by the framer byte in data[0].
 *
 * After KISS (or any other outer transport) has delivered a modem payload,
 * parse_frame_header() reads byte 0 and tells the modem whether the frame
 * belongs to ARQ or broadcast. This is independent from the KISS command
 * byte used on the TCP link.
 */
#define PACKET_TYPE_ARQ_CONTROL       0x00  /* ACK, DISCONNECT, TURN_REQ, etc.    */
#define PACKET_TYPE_ARQ_DATA          0x01  /* data payload frames                */
#define PACKET_TYPE_ARQ_CALL          0x02  /* CALL/ACCEPT setup (compact layout) */
#define PACKET_TYPE_BROADCAST_CONTROL 0x03  /* broadcast/control frame type       */
#define PACKET_TYPE_BROADCAST_DATA    0x04  /* broadcast/data frame type          */
#define PACKET_TYPE_ARQ_CQ            0x05  /* compact DATAC13 CQ metadata frame  */

#define PACKET_TYPE_BITS   3    /* bits [7:5] of framer byte */
#define PACKET_TYPE_SHIFT  5
#define PACKET_TYPE_MASK   0x07
#define FRAME_EXT_BITS     5    /* bits [4:0] of framer byte */
#define FRAME_EXT_MASK     0x1f

#define HEADER_SIZE 1 // Size of the Hermes header
#define BROADCAST_CONFIG_PACKET_SIZE 9 // hermes-broadcast RaptorQ config packet

/* Extension bits for PACKET_TYPE_BROADCAST_DATA frames.
 *
 * BCAST_EXT_LEN_PREFIX (bit 0): the frame carries a 2-byte big-endian payload
 * length immediately after the Mercury header byte.  Senders set this bit and
 * write the real AX.25 payload length before zero-padding to the full modem
 * frame size.  Receivers that recognise this bit deliver exactly that many
 * bytes as CMD_AX25CALLSIGN, independent of the latched bcast_reply_cmd,
 * which eliminates both trailing-null padding and the transmit-first latch.
 * Receivers that do not recognise the bit fall back to legacy behaviour. */
#define BCAST_EXT_LEN_PREFIX 0x01

/* Number of bytes occupied by the big-endian length field when
 * BCAST_EXT_LEN_PREFIX is set in the Mercury header. */
#define BCAST_LEN_FIELD_SIZE 2

static inline uint8_t frame_header_packet_type(uint8_t header)
{
    return (uint8_t)((header >> PACKET_TYPE_SHIFT) & PACKET_TYPE_MASK);
}

static inline uint8_t frame_header_extension(uint8_t header)
{
    return (uint8_t)(header & FRAME_EXT_MASK);
}

// Parse the frame header and optionally return the extension field.
// Returns packet type or negative on invalid input.
int8_t parse_frame_header(const uint8_t *data_frame, uint32_t frame_size, uint8_t *extension_out);
void write_frame_header(uint8_t *data, int packet_type, uint8_t extension);




#endif // FRAMER_H
