# File Editor

This is a text editor command line application. It will allow a user to enter a filename to a txt file, then will be 
allowed to edit the file and then save it back to the location.

## Instructions for Build and Use

Steps to build and/or run the software:

1. Open the project folder in CLion.
2. Allow CLion to load the project's `CMakeLists.txt` file.
3. Select the `text_editor` run configuration.
4. Build the project using **Build > Build Project**.
5. Run the application using **Run > Run 'text_editor'**.

Instructions for using the software:

1. Choose `1` to create a new file, `2` to edit an existing file, or `q` to quit.
2. Enter the file path when prompted.
3. Use the arrow keys to move the cursor and type normally to insert text.
4. Use `Enter`, `Tab`, `Backspace`, and `Delete` to edit the document. Enter copies leading spaces from the previous line, and Tab inserts four spaces while moving the cursor past them.
5. Press `Ctrl-S` to save changes.
6. Press `Ctrl-Z` to undo and `Ctrl-Y` to redo edits.
7. Press `Ctrl-F`, type a case-insensitive search term, and press `Enter` to enter find mode. Use the left and right arrows to move to the previous or next match, then press `q` to leave find mode. Press `Escape` to cancel.
8. Press `Ctrl-A` to save as, `Ctrl-O` to open another file, `Ctrl-R` to reload the active file, or `Ctrl-N` to create a new file without leaving the editor.
9. Press `Ctrl-Q` to quit. If there are unsaved changes, press `y` to continue the requested operation or `n` to cancel it.

## Development Environment

To recreate the development environment, you need the following software and/or libraries with the specified versions:

* CLion
* Apple Clang 21.0.0
* C++17
* CMake 3.20 or newer through CLion

## Useful Websites to Learn More

I found these websites useful in developing this software:

* [YouTube](https://youtu.be/0gKT5ppfbn0?si=W1oZtkrqRrsZmkNP)
* [YouTube](https://youtu.be/RSDzvlXmQi4?si=UnkV7U4ozSqpu52x)

## Future Work

The following items I plan to fix, improve, and/or add to this project in the future:

* [ ] Add a GUI
* [ ] Add line numbers
* [ ] Add autosave functionality