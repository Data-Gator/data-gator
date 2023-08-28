#!/usr/bin/python3
"""
    Create and manage project version for PlatformIO project.

    This script is responsible for creating and maintaining 
        a version file version.hpp, which tracks the major, minor,
        and patch number of the project release. 

    When this script is called the version is incremented(either major,
        minor, or patch), and the project is built with the output
        file name set to follow the format specified in platformio.ini 
        (generally <custom_fw_prefix>_v<major>.<minor>.<patch>.bin).

"""
import os
from datetime import datetime, timezone
from os import path

def print_title():
    print((
     " __  __ __  __ _____   __      __           _             \n" +
     "|  \/  |  \/  |  __ \  \ \    / /          (_)            \n" + 
     "| \  / | \  / | |__) |  \ \  / /__ _ __ ___ _  ___  _ __  \n" + 
     "| |\/| | |\/| |  ___/    \ \/ / _ \ '__/ __| |/ _ \| '_ \ \n" + 
     "| |  | | |  | | |         \  /  __/ |  \__ \ | (_) | | | |\n" + 
     "|_|  |_|_|  |_|_|          \/ \___|_|  |___/_|\___/|_| |_|\n"))
                                                           

Import("env")
output_prefix = env.GetProjectOption("custom_fw_prefix")

if env.IsIntegrationDump():
    # stop the current script execution
    Return()

print_title()
print("\nMajor - Minor - Patch(MMP) Version System")
print("\t[AUTHOR] grwells\n\t[VERSION] v0.1.0")

vmajor = 0
vminor = 0 
vpatch = 0 




def prompt_for_version():
    """
    Get a version number from the user manually.
    """
    print('Initialize Project Version(MMP) with format vMAJOR.MINOR.PATCH')
    good_v = False 

    while not good_v:
        print("enter major version(int):")
        major = int(input())
        print('enter minor(int):')
        minor = int(input())
        print('enter patch(int):')
        patch = int(input())

        print(f'set {major}.{minor}.{patch}? (y/n): ')
        good_v = ('y' == input())

    return major, minor, patch


def write_version(major:int, minor:int, patch:int):
    """
    Write/create version.h with the specified version number.

    Wipe all other contents from the file.
    """
    file = open('./include/version.hpp', "w")
    file.write(('/* AUTO GENERATED FILE - DO NOT MODIFY'+
                f'\n* PROJECT: {output_prefix}'+
                '\n* DESCRIPTION: version tracking file with current release version as defined below' +
                f'\n* LAST UPDATED: {(datetime.utcnow()).strftime("%X, %x")} (UTC)' + 
                '\n*/')) 
    file.write(f'\n#define VERSION_MAJOR {major}\n#define VERSION_MINOR {minor}\n#define VERSION_PATCH {patch}\n')
    file.close()


def read_version():
    """
    Open version.h and read current project version from the file.

    Assume that file already exists, allow error to be thrown otherwise.
    """
    vmajor = 0
    vminor = 0 
    vpatch = 0 

    file = open('./include/version.hpp', "r")
    contents = file.readlines()
    
    for line in contents:
        # check each line and extract the defined current version
        if 'VERSION_MAJOR' in line:
            vmajor = int(line[22:len(line)-1])

        if 'VERSION_MINOR' in line:
            vminor = int(line[22:len(line)-1])

        if 'VERSION_PATCH' in line:
            vpatch = int(line[22:len(line)-1])


    file.close()
    print(f'\t\textracted current version v{vmajor}.{vminor}.{vpatch} from version.h')
    return vmajor, vminor, vpatch



# check if version.h exists
if path.exists('./include/version.hpp'):
    # version.h exists, so read current version, increment by type, then write to version.h
    # read the current version
    print('\t\tfound version.hpp file in project\'s include directory')
    vmajor, vminor, vpatch = read_version()

    # increment version number based on release type
    project_options = env.GetProjectOptions()
    contains_rtype = False
    rtype = 'default'

    # check if release type is specified
    for opt in project_options:
        if 'custom_release_type' in opt: 
            contains_rtype = True

    if contains_rtype:
        # increment appropriate version segment, reset all lower segments
        rtype = env.GetProjectOption("custom_release_type")
        print(f'\t\trelease type: {rtype}')
        if rtype == 'major':
            vmajor += 1
            vminor = vpatch = 0

        if rtype == 'minor':
            vminor += 1
            vpatch = 0

        if rtype == 'patch':
            vpatch += 1

        if rtype == 'manual':
            vmajor, vminor, vpatch = prompt_for_version()
            print(f'!!!: {vmajor}, {vminor}, {vpatch}')

        # write to version.h
        write_version(vmajor, vminor, vpatch)

    else:
        print('\t\t[ERROR] no option found')
        print(env, dir(env))
        print(env.GetProjectOptions())

else:
    # prompt user for starting version and create version.h 
    vmajor, vminor, vpatch = prompt_for_version()

    # write the version to version.h
    print('creating version.hpp...')
    file = open('./include/version.hpp', 'w+')
    file.write(f'#define VERSION_MAJOR {vmajor}\n#define VERSION_MINOR {vminor}\n#define VERSION_PATCH {vpatch}\n')
    file.close()
    print('initialized version.h')

# set the firmware file name based on project and version 
output_file = output_prefix + f'_v{vmajor}.{vminor}.{vpatch}'
env.Replace(PROGNAME=output_file)
print(f'\t[OUTPUT] {output_file}')

print('\n\t[END] MMP Version System Exiting\n\n')


