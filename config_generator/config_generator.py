#!/usr/bin/python3
import configparser
import argparse


def print_title():
    print((
    '   _____             __ _          _____           \n' +  
    '  / ____|           / _(_)        / ____|          \n' + 
    ' | |     ___  _ __ | |_ _  __ _  | |  __  ___ _ __ \n' + 
    " | |    / _ \| '_ \|  _| |/ _` | | | |_ |/ _ \ '_ \ \n" + 
    " | |___| (_) | | | | | | | (_| | | |__| |  __/ | | | \n" + 
    "  \_____\___/|_| |_|_| |_|\__, |  \_____|\___|_| |_| \n" + 
    "                           __/ |                     \n" + 
    "                          |___/                      \n"))

def init_config(config):
    """
    Initializes the configuration.
    
    :param      config:  The configuration
    :type       config:  { type_description }
    """
    with open('default_config.ini') as configfile:
        config.write(configfile)


def read_config_ini(input_file_name:str):
    """
    Reads a configuration initialize.
    
    :param      input_file_name:  The input file name
    :type       input_file_name:  str
    :param      profile:          The profile to be loaded
    :type       profile:          str
    """
    config = configparser.ConfigParser()
    config.read(input_file_name)

    return config


def write_config_header(config:str, profile:str, output_file:str):
    """
    Writes a configuration header.
    
    :param      config:       The configuration
    :type       config:       str
    :param      profile:      The profile
    :type       profile:      str
    :param      output_file:  The output file
    :type       output_file:  str
    """
    header = (
        "/*\n" +
        ' * Project Configuration Header\n' + 
        ' *     generated from config.ini using config_generator.py\n' + 
        " */\n")

    # write output to config file
    with open(output_file, 'w+') as outputf:
        outputf.write(header)
        # detect type of option and make appropriate definition
        for option in config.options(profile):

            opt_type = option[0:1] # s=str, i=int
            opt_name = option[2:].upper()

            #print(opt_type, opt_name)

            if opt_type == 'i':
                # integer
                outputf.write(f'#define {opt_name} {config.get(profile, option)}\n')

            elif opt_type == 's':
                # string
                outputf.write(f'#define {opt_name} \"{config.get(profile, option)}\"\n')

            else:
                print(f'[ERROR] unknown type with {opt_name}')



def manual_config():
    """
    Select the field to edit for this config.
    """
    menu = '{:=^30}'.format('MENU')
    # print menu header
    print(menu)

    # print fields
    fields = [
        'Network Name', 
        'Network Password', 
        'MQTT Broker IP Address',
        'MQTT Broker Port',
        'WiFi Timeout(ms)',
        'Serial Debug Statements',
        'Task Scheduling',
        'Write to \'config.h\'',
        'Cancel Changes']

    for i in range(len(fields)):
        width = 28-len(fields[i])
        print('[{}]{:.>{width}}'.format(fields[i], i, width=width))

    print('{:=<30}'.format(''))

    print('Select Option: ')
    option = int(input())



def main(args):

    if args.profile == 'manual':
        manual_config()

    else: 
        config = read_config_ini(args.input_config_file)

        # print config options for confirmation
        print('{:=<50}'.format("PROFILE"))
        for option in config.options(args.profile):
            width = 48 - len(option) - len(config.get(args.profile, option))
            print(
                option, 
                '{:^{width}}{}|'.format('', config.get(args.profile, option), width=width))

        print('{:=<50}'.format(""))


        if input("Confirm Selection?(y/n): ") == 'y': 
            write_config_header(config, args.profile, args.output_file)

        else:
            exit()


print_title()

if __name__ == '__main__':
    # parse command line arguments
    parser = argparse.ArgumentParser(description='Generate a config header file for the aggregator project.')
    parser.add_argument(
        '-p', 
        '--profile', 
        action='store',
        metavar='-p',
        dest='profile',
        help='select a profile which contains values necessary for the target environment, if no input file provided then the default reads from \'config.ini\'',
        default='DEFAULT')

    parser.add_argument(
        '-cf', 
        '--config-file',
        action='store',
        dest='input_config_file',
        help='the name of a configparser-style configuration file with profiles defined',
        default='config.ini')

    parser.add_argument(
        '-o', 
        '--output',
        action='store',
        dest='output_file',
        help='output header file, default is \'config.h\'',
        default='config.h')

    args = parser.parse_args()
    main(args)

else:
    # assume called as automated script
    config = read_config_ini('config.ini')
    
    print('\n{:=<50}'.format('AVAILABLE PROFILES'))

    sections = config.sections()
    for i in range(len(sections)): 
        width = 49
        print('{index}{:.>{width}}'.format(sections[i], index=i, width=width))

    print('{:=<50}'.format(''))

    profile = int(input())

    write_config_header(config, sections[profile], 'include/config.hpp')

