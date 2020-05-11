title: The REVENG OOT Module
brief: Looks for sync words in a stream, converts to packets
tags:
  - sdr
  - packet
author:
  - Tim Kuester <tkuester@2n3904.net>
copyright_owner:
  - Tim Kuester
license:
gr_supported_version: 3.8.1.0
repo: https://github.com/tkuester/gr-reveng
---
GNU Radio has excellent tools for processing streams of samples, and some tools
for processing packets. However, there aren't many tools for crossing the
boundary between samples and packets.

**gr-reveng** was originally written to reverse engineer small ISM band
devices. While other solutions like
[inspectrum](https://github.com/miek/inspectrum) are MUCH better suited towards
reverse engineering, **gr-reveng** fills a necessary gap for quickly
prototyping receivers that work in realtime.
