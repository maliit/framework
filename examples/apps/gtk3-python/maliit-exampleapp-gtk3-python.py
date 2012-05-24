#!/usr/bin/env python

from gi.repository import Gtk
from gi.repository import Maliit

def main():
    win = Gtk.Window()
    win.connect('destroy', lambda w: Gtk.main_quit())
    win.set_default_size(450, 550)
    win.set_title("Maliit Python Gtk+ example")

    input_method = Maliit.InputMethod()

    box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
 
    button = Gtk.Button("Show keyboard")
    button.connect("clicked", lambda x: input_method.show())
    box.pack_start(button, False, False, 0)

    button = Gtk.Button("Hide keyboard")
    button.connect("clicked", lambda x: input_method.hide())
    box.pack_start(button, False, False, 0)

    entry = Gtk.Entry()
    box.pack_start(entry, False, False, 0)

    win.add(box)

    win.show_all()
    Gtk.main()

if __name__ == '__main__':
    main()

