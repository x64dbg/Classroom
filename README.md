# Classroom

[![Build status](https://ci.appveyor.com/api/projects/status/w0e2ktpd48ji47y6/branch/master?svg=true)](https://ci.appveyor.com/project/x64dbg/classroom/branch/master)

![Classroom](https://user-images.githubusercontent.com/15761310/68860016-890d1b80-06e0-11ea-850f-6bd6622f2613.png)

Classroom is a x64dbg plugin that helps you analyze an object-oriented application. You can define member functions and member variables while you are debugging the application, and the class documentation will be saved and displayed whenever you need them.

The plugin has two main interfaces: a dialog and a plugin tab. In the plugin tab you can add a class, define various properties of the class such as descriptions, member variables and functions, and browse and manage all the information you currently know about this class in a clean view. The plugin dialog focuses on the most relevant information about your current work. It automatically displays the label, comments and value of all member variables of a class instance, and automatically focuses on the variable the current instruction accesses. It also shows the documentation of the function that you are debugging or that the current instruction branches to. Really saves your time analyzing object-oriented application!

Contributions are welcome. The plugin is compiled using the same compiling environment as x64dbg. The plugin currently needs a more beautiful documentation page, analysis feature, more class properties (known instances, known usages of functions and variables, virtual methods) and more.

## Instructions documentation
The plugin currently supports the following instructions:
- `classroom [class]`: Without argument: open a dialog to add a new class. With 1 argument: edit the class specified by the argument.
- `classmembervar class,[offset]`: With 1 argument: open a dialog to add a member variable to the class. With 2 arguments: open a dialog to edit the member variable at offset `offset`. Notes: all offsets are written in hex.
- `delclass class`: Delete the specified class.
- `delclassmembervar class,offset`: Delete the member variable at offset `offset` in the class.
