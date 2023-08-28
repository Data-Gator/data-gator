# TimeStamp
NTP time stamp formatting with support for WDT relative offsets. Standard timestamp formatting for NTP servers is "mm-dd-yyThh:mm:ss" but since this library is written for usage in systems which may need to log timestamps without absolute NTP knowledge(between reboots in locations without WiFi), this library adds a "+\<offset\>" section to the end of the conventional timestamp.
This represents a timestamp from "\<offset\>" _minutes_ in the future.
