#!/usr/bin/env python3

from __future__ import print_function

from gi.repository import GObject, GLib
from gi.repository import Maliit

import sys, inspect

def print_settings_entry(entry, indent=' '):

    print (indent + 'Key:', entry.get_key())
    print (indent + 'Value:', entry.get_value().unpack())
    print (indent + 'Description:', entry.get_description())
    print (indent + 'Type:', entry.get_entry_type())

    attributes = entry.get_attributes()

    # XXX: perhaps Maliit library should define (translatable)
    # names for each of these well-known attributes
    known_attributes = (Maliit.SETTING_DEFAULT_VALUE,
                        Maliit.SETTING_VALUE_DOMAIN,
                        Maliit.SETTING_VALUE_DOMAIN_DESCRIPTIONS,
                        Maliit.SETTING_VALUE_RANGE_MAX,
                        Maliit.SETTING_VALUE_RANGE_MIN,
                        )

    try:
        print (indent + 'Default Value:', attributes[Maliit.SETTING_DEFAULT_VALUE].unpack())
    except KeyError:
        pass

    try:
        print (indent + 'Min Value:', attributes[Maliit.SETTING_VALUE_RANGE_MIN].unpack())
    except KeyError:
        pass

    try:
        print (indent + 'Max Value:', attributes[Maliit.SETTING_VALUE_RANGE_MAX].unpack())
    except KeyError:
        pass

    try:
        domains = attributes[Maliit.SETTING_VALUE_DOMAIN].unpack()
        domain_descriptions = attributes[Maliit.SETTING_VALUE_DOMAIN_DESCRIPTIONS].unpack()

        if len(domains) != len(domain_descriptions):
            print ("Warning: Number of domain and domain description values not equal")

        print (indent + 'Allowed Values: (value, "description")')
        for domain, description in zip(domains, domain_descriptions):
            print (indent*2 + '- (%s, "%s")' % (domain, description))

    except KeyError:
        pass

    additional_attributes = dict((key, value.unpack()) for key, value in attributes.items() if not key in known_attributes)
    if additional_attributes:
        print (indent + 'Additional Attributes:', additional_attributes)


def print_plugin_info(plugin_info, indent='  '):
    print (indent + 'Name:', plugin_info.get_plugin_name())
    print (indent + 'Description:', plugin_info.get_plugin_description())

def print_plugin_settings(plugin_info, indent='  '):
    print (indent + 'Settings:')
    for entry in plugin_info.get_configuration_entries():
        print (indent + '-',  entry.get_key().split('/')[-1])
        print_settings_entry(entry, indent*2)

def find_entry(entries, predicate, user_data):
    """Return the first entry in entries for which predicate(entry, user_data) returns True."""

    for entry in entries:
        if predicate(entry, user_data):
            return entry
    return None


def variant_from_string(string, maliit_setting_type):
    """Return a GLib.Variant from a user-provided string."""

    def int_from_string(int_string):
        variant_format = 'i'
        value = int(string)
        return variant_format, value

    def string_from_string(s):
        s = s.strip()
        s = s.strip("'")
        s = s.strip('"')
        variant_format = 's'
        value = s
        return variant_format, value

    string = string.strip()

    if maliit_setting_type == Maliit.SettingsEntryType.STRING_TYPE:
        try:
            variant_format, value = string_from_string(string)
        except ValueError:
            return None

    elif maliit_setting_type == Maliit.SettingsEntryType.INT_TYPE:
        try:
            variant_format, value = int_from_string(string)
        except ValueError:
            return None

    elif maliit_setting_type == Maliit.SettingsEntryType.BOOL_TYPE:
        variant_format = 'b'
        if string.lower() == 'true':
            value = True
        elif string.lower() == 'false':
            value = False
        else:
            return None

    elif maliit_setting_type == Maliit.SettingsEntryType.STRING_LIST_TYPE:
        variant_format = 'as'
        value = []
        string = string.rstrip(']')
        string = string.lstrip('[')
        items = string.split(',')
        for item in items:
            item = item.strip()
            s = string_from_string(item)[1]
            value.append(s)

    elif maliit_setting_type == Maliit.SettingsEntryType.INT_LIST_TYPE:
        variant_format = 'ai'
        value = []
        string = string.rstrip(']')
        string = string.lstrip('[')
        items = string.split(',')
        for item in items:
            item = item.strip()
            value.append(int_from_string(item)[1])

    else:
        raise NotImplementedError, "Unable to create variant for %s" % repr(maliit_settings_type)

    variant = GLib.Variant(variant_format, value)
    return variant

def about_plugin_settings_received(settings_manager, settings, plugin_name, loop):
    found = False

    for plugin_info in settings:
        if (plugin_info.get_plugin_name() == plugin_name):
            found = True
            print_plugin_info(plugin_info)
            print_plugin_settings(plugin_info)

    if not found:
        print ('No such active plugin:', plugin_name)

    loop.quit()

def get_settings_entry(settings, plugin_name, setting_name):

    def match_plugin(plugin_info, plugin_id):
        if (plugin_info.get_plugin_name() == plugin_id):
            return True

    def match_setting(entry, setting_id):
        entry_id = entry.get_key().split('/')[-1]
        if entry_id == setting_id:
            return True

    plugin = find_entry(settings, match_plugin, plugin_name)

    if not plugin:
        print ('No such active plugin:', plugin_name)
        return None

    setting_entry = find_entry(plugin.get_configuration_entries(), match_setting, setting_name)
    if not setting_entry:
        print ('%s has no setting "%s"' % (plugin_name, setting_name))
        return None

    return setting_entry

def set_plugin_settings_received(settings_manager, settings,
                                 plugin_name, setting_name, setting_value, loop):

    setting_entry = get_settings_entry(settings, plugin_name, setting_name)
    if not setting_entry:
        loop.quit()

    setting_name = setting_entry.get_key().split('/')[-1]
    setting_value_variant = variant_from_string(setting_value, setting_entry.get_entry_type())
    if not setting_entry.is_value_valid(setting_value_variant):
        # TODO: print valid range/domain, to help user find the correct value
        print ('Value %s is not valid for setting %s' % (setting_value_variant, setting_name))
        loop.quit()

    setting_entry.set_value(setting_value_variant)
    loop.quit()

def get_plugin_settings_received(settings_manager, settings,
                                 plugin_name, setting_name, loop):

    setting_entry = get_settings_entry(settings, plugin_name, setting_name)

    if not setting_entry:
        loop.quit()

    print (setting_entry.get_value().unpack())
    loop.quit()

def list_plugin_settings_received(settings_manager, settings, loop):

    for plugin_info in settings:
        print (plugin_info.get_plugin_name())

    loop.quit()


### Subcommand handlers ###
def about_handle(args, settings_manager, loop):
    """Usage: %prog about [plugin]"""

    if len(args) == 1:
        plugin_name = args[0]
    elif len(args) == 0:
        plugin_name = "server"
    else:
        raise ValueError, 'Incorrect number of arguments'

    settings_manager.connect('plugin-settings-received',
                             about_plugin_settings_received, plugin_name, loop)


def set_handle(args, settings_manager, loop):
    """Usage: %prog set [plugin] <setting> <value>"""

    plugin_name = None
    setting_name = None
    setting_value = None

    if len(args) == 3:
        plugin_name, setting_name, setting_value = args
    elif len(args) == 2:
        setting_name, setting_value = args
        plugin_name = "server"
    else:
        raise ValueError, 'Incorrect number of arguments'

    settings_manager.connect('plugin-settings-received', set_plugin_settings_received,
                             plugin_name, setting_name, setting_value, loop)


def get_handle(args, settings_manager, loop):
    """Usage: %prog get [plugin] <setting>"""

    plugin_name = None
    setting_name = None

    if len(args) == 2:
        plugin_name, setting_name = args
    elif len(args) == 1:
        setting_name = args
        plugin_name = "server"
    else:
        raise ValueError, 'Incorrect number of arguments'

    settings_manager.connect('plugin-settings-received', get_plugin_settings_received,
                             plugin_name, setting_name, loop)


def list_plugins_handle(args, settings_manager, loop):
    """Usage: %prog list-plugins"""

    settings_manager.connect('plugin-settings-received', list_plugin_settings_received, loop)


def print_help(handlers, subcommand=''):
    """ """

    def print_doc(doc_string):
        print (doc_string.replace('%prog', sys.argv[0]))

    if subcommand:
        handler = handlers[subcommand][0]
        print_doc (inspect.getdoc(handler))
    else:
        print_doc ('Usage: %prog [--help] [<command>] ...')
        print ('\nValid commands:')
        for command, value in handlers.items():
            description = value[1]
            print ('  ', command, ':', description)

        print_doc ('\nExamples:')
        print_doc ('  %prog about')
        print_doc ('  %prog about libmaliit-keyboard-plugin.so')
        print_doc ('  %prog set libmaliit-keyboard-plugin.so current_style nokia-n9')
        print_doc ('  %prog get libmaliit-keyboard-plugin.so current_style')


# TODO: also allow to use the settings key instead of "plugin_name setting_name" for get/set?
def main():
    loop = GObject.MainLoop()

    subcommand_handlers = {
        'about': (about_handle, 'Information about the available settings'),
        'set': (set_handle, 'Set setting entry value'),
        'get': (get_handle, 'Get setting entry value'),
        'list-plugins': (list_plugins_handle, 'List the enabled plugins'),
    }

    if len (sys.argv) < 2:
        print_help(subcommand_handlers)
        sys.exit (1)

    if '--help' in sys.argv:
        subcommand = None
        sys.argv.remove('--help')
        if len(sys.argv) >= 2:
            subcommand = sys.argv[1]
        print_help(subcommand_handlers, subcommand)
        sys.exit (1)

    subcommand = sys.argv[1]
    arguments = sys.argv[2:]

    try:
        handler = subcommand_handlers[subcommand][0]
    except KeyError:
        print ('No subcommand for %s')
        print_help(subcommand_handlers)
        sys.exit(1)

    settings_manager = Maliit.SettingsManager()

    try:
        handler(arguments, settings_manager, loop)
    except ValueError, e:
        print (e, '\n')
        print_help(subcommand_handlers, subcommand)
        sys.exit(1)

    settings_manager.load_plugin_settings()
    loop.run()

if __name__ == '__main__':
    main()

