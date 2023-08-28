# Config Generator
This script reads a profile from a `configparser` config file, `config.ini` by default, and generates `config.h` with each config option as a C macro which can then be referenced in other files.

### Use
The `config_generator.py` was written with use in PlatformIO projects in mind. Running this script before a build or upload command will allow the user to generate a generic config file with a set of defines which may need to be changed based on target environment. For example, enabling debug flags or changing WiFi authentication information when moving between working in your home and working at the project site.

To call in PlatformIO as part of an environment and select the config profile to apply from `config.ini` just add this line to the target environment in PlatformIO:

	extra_scripts = pre:config_generator.py