# gr-reveng

Often times, when reverse engineering an RF protocol, you will be looking for
a particular pattern to signify the start of a packet. This can be done
manually by searching through a file with a one-off python script, but that's
tedious. This package contains a few blocks that should prove to be useful in
deframing packets and displaying them on the screen.

A helpful, configurable demo is located in the examples folder.

This software has been developed for GNU Radio 3.7.9, which ships with
Ubuntu 16.04. It should work just as well with more recent versions.
Please let me know what you have success with!

## Packet Deframer

This block takes in a stream of byte samples, where the LSB for each sample
represents a bit. The packet searches for the specified sync word, obtains
the specified number of bytes, and sends it off as a PDU.

The Packet Deframer can be configured to run on fixed length packets, or
variable length packets, similar to ones generated from the CC1101.

### PDU Format

```python
(meta, data) = pdu
'''
meta => {'ts': double(),    # Unix timestamp (with subsecond resolution)
         'name': str(),     # see below
         'packed': bool()}  # True if the data is a list of bytes
data => list([...])         # a list of samples
'''
```

### Parameters

The following parameters apply to all use cases.

* **Name**: An optional name to send along with the PDU metadata. Useful when processing multiple streams of data.
* **Sync Word**: An arbitrarily long list of bits to sync on.
* **Mode**: Fixed Length, or Variable Length.
* **Pack Bytes**: Whether to pack the incoming bits into bytes, or not.

#### Fixed Length Parameters

* **Packet Len**: The number of **bits** to grab after the `sync_word`

#### Variable Length Parameters

* **Max Length**: The largest value accepted for the length field. Useful for squelching bad packets.
* **Packet Len Offset**: The number of **bytes** that come before the packet length. Some radios send a `transmitter_id`, which comes before the length byte.
* **Additional Bytes**: The number of **bytes** to capture in addition to the given length, in the event the packet length does not include the checksum

## Future Work
* Specify checksum algorithm *à la* `python-crcmod`
* Currently, packet length must be 8 bytes. Accommodate for 16 bit lengths
* Endian-ness and MSB/LSB first, esp. for variable length packets
* Encodings like manchester (low hanging fruit!)
