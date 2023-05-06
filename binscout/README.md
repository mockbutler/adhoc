# binscout
Search a binary file for a byte sequence.

This is reconstructed and cleaned up from various copies and variants in backups
reaching back to at least 2010. It was originally called hexcrawler and was both
simpler, and more complex. In otherwords: naive. Well at least more so.

This tool implements a feature found in any decent hex editor. The advantage
being it can be run on remote servers with only a ssh connection. Some files are
too large and distant to transfer for analysis.

There is probably an obscure command line tool, or clever invocation of a well
known one, that will provide equivalent functionality. Thanks, but I'm not
interested.

## How to generate the manpage

`pandoc -s -t man binscout.1.md -o binscout.1`

## Limitations

 - One file, one byte sequence.
 - Does not work with standard input; a cardinal sin in Unix.
 - Outputs to standard output only; something of a venial sin in Unix.
 - Warning messages will be intersprersed with output: no quiet mode.

## Missing Polish

 - Search for double and single precision IEEE 754 numbers.
 - Output offsets in a radix other than hexadecimal.
 - Process standard input by default if no file is specified.
 - Search across multiple files.
 - Start at a specified offset in the file.
 
## Missing Chrome

 - Simultaneously search for Multiple byte vectors.
 - The man page is at best only adequate.
 - Unit tests. 


