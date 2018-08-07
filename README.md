# treeplanner (CLI)
> Simple and convenient task/to-do list manger based on a dependency tree structure.

## Installation
### Clone the repository
```console
$ git clone https://github.com/freeeroam/treeplanner
```
### Build and setup 
```console
$ cd treeplanner
$ make
$ ./setup.sh
```
The setup  script "setup.sh" will create a data directory within your treeplanner directory for storing items and will also create a run script which will be placed in a directory that is in your PATH environment variable, in order to allow the script to be launched from any directory.

If the setup script fails due to your distribution's file system structure, you can manually move the run script to a directory within your PATH by replacing "/usr/local/bin" in the following command with the absolute path of the desired directory:
```console
$ sudo mv run_script /usr/local/bin/treeplanner
```

## Usage
### Run
Once it has been installed and setup, you can run treeplanner using the command:
```console
$ treeplanner
```

### Controls
- Use <kbd>↑</kbd> and <kbd>↓</kbd> to navigate through dependency items.
- Use <kbd>g</kbd> and <kbd>G</kbd> to navigate to the top and bottom items, respectively.
- Use <kbd>→</kbd> or <kbd>ENTER</kbd> to view the selected item.
- Use <kbd>←</kbd> to go back to the previous item.
- Use <kbd>a</kbd> to add a new item.
- Use <kbd>d</kbd> to delete the selected item.
- Use <kbd>?</kbd> to view the help screen.
- Use <kbd>s</kbd> to save all items (all items are saved automatically when quitting).
- Use <kbd>r</kbd> to refresh the current view.
- Use <kbd>q</kbd> to quit.

## Requirements
- g++
- libncurses5-dev
- make
