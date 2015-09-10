# _Introduction_ #

This document is about the auxiliary data files used by HBL: their purpose and their structure.

# _Details_ #

Although the code is full of details about the external data files, we will detail them also here, so people who don't want to dig out in the source can find about the files.

Since [r289](https://code.google.com/p/valentine-hbl/source/detail?r=289), there is a group of files: _libs_.

## libs ##

_libs_ are included in the directories libs\_5xx, libs\_6xx and lib\_66x. They refer to the firmware version they're built upon. HBL will automatically detect the firmware version and use the relevant directory. Since both directories have the same function, we will refer to them as _libs_ from now on.

Inside _libs_ directory there are various _.nids_ files. Each file has the name of an exported FW library, and it contains all NIDs exported from that library (binary format). Each NID is 4 bytes long, and they're just concatenated one after the other. So the file is no more than a big NID array. This files are used by the syscall estimation process.