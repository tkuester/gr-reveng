# gr-reveng

Often times, when reverse engineering an RF protocol, you will be looking for
a particular pattern to signify the start of a packet. This can be done
manually by searching through a file with a one-off python script, but that's
tedious. This package contains a few blocks that should prove to be useful in
deframing packets and displaying them on the screen.

**This software is still under development!**

## Packet Deframer

This block takes in a stream of byte samples, where the LSB for each sample
represents a bit. The packet searches for the specified sync word, obtains
the specified number of bytes, and sends it off as a PDU.

The Packet Deframer can be configured to run on fixed length packets, or
variable length packets, similar to ones generated from the CC1101.

### PDU Format

```python
(meta, bits) = pdu
'''
meta => {'ts': long,   # microseconds since midnight today
        'name': str}   # configured below
bits => list([0,1]*99) # a list of bit samples, 1 and 0
'''
```

### Parameters

The following parameters apply to all use cases.

* **Name**: An optional name to send along with the PDU metadata. Useful when processing multiple streams of data.
* **Sync Word**: An arbitrarily long list of bits to sync on.
* **Mode**: Fixed Length, or Variable Length.

#### Fixed Length Parameters

* **Packet Len**: The number of bits to grab for the packet

#### Variable Length Parameters

* **Packet Len Offset**: The number of bytes that come before the packet length
* **Additional Bytes**: The number of bytes to capture in addition to the given length, in the event the packet length does not include the checksum

## Future Work
* Specify checksum algorithm *à la* `python-crcmod`
* Currently, packet length must be 8 bytes. Accommodate for 16 bit lengths
* Endian-ness and MSB/LSB first, esp. for variable length packets
* Option to pack bits into bytes
* Write some test cases >_>
