# How to Make Releases

These are the steps that you should follow to create a release for the aggregator/data gator and make sure that you get the correct settings and configurations.

### Steps

1. _**Configuration Profile:**_ configuration profiles contain settings network connection, sensor polling rate, and debug modes. Documentation for how to do this and more about what they are can be found in [Configuration Files and Creating Profiles](./Configuration_Files_and_Creating_Profiles.md). If you want something other than a pre-existing profile or the default, you should consider editing this file.

2. _**Major.Minor.Patch Version:**_ before creating a release binary, you should choose the type of release, major, minor, or patch. These release types represent different levels of changes. Major releases represent the introduction of features that break backward compatibility. Minor releases include additional functionality with backward compatibility. Patch versions fix bugs but don't introduce changes to API/functionality.

        pio run -e rmajor|rminor|rpatch -t upload

3. _**Create Release:**_ create a github release with version number on the binary file in `.pio/build/<release type>/firmware_<version number>.bin` and instructions for flashing.

### On Flashing to Physical Devices
The steps for flashing directly to a physical device are always included with the Github release. The best method for flashing to one or two devices, perhaps for testing, you may still want to take advantage of the Over-the-Air update capability. If you do so, be aware that currently OTA updates do not allow flashing to a single device... this is possible, but not implemented at the moment I believe.
