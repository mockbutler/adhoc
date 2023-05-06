---
title: BINSCOUT
section: 1
header: User Manual
footer: binscout 0.1.0
date: January 2017
---

# NAME
binscout - Search in binary files.

# SYNOPSIS
**binscout** [*OPTION*] *NEEDLE* *FILE*

# DESCRIPTION
Search for a byte sequence in a binary file. Output the offset of each
matching occurance in hex, one per line.

# OPTIONS

`-t type`
: Needle type.

## Needle Types

- *hex* Hexadecimal string.
- *str* ASCII string.
- *cstr* Null terminated ASCII string.
- *le16, le32, le64* Little endian integer.
- *be16, be32, be64* Big endian integer.

# AUTHORS

Marc Butler <mockbutler@gmail.com>

# BUGS

Udoubtedly.
