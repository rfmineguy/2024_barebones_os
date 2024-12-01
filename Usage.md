# Usage

![Image](images/rfos.jpg)

> [!WARNING]
> If for some reason when the OS boots the keyboard isn't immediately working,
> try restarting the OS. I don't know why the keyboard doesn't work sometimes,
> but that's a simple "fix".

## The Splash Pane This pane is dedicated to displaying the Operating System's
name and logo.

## The Shell Pane In this OS we have one main interactive pane called the
`shell`. It is wired up to the keyboard interrupt so you can type into it. In
this shell there is a few supported commands. They go as follows.
   - `read <filename>` - read the file from the fat system with name of
     'filename'
   - `newf <filename>` - create new file ready to be appended to
   - `appf <filename> <quoted>` - append the **"quoted text"** to the end of
     the file specified by 'filename'
   - `list` - lists all the files on the fat drive
   - `reboot` - restarts the operating system (no need to relaunch qemu)
   - `clear` - clears the shell area
 
These commands support arguments, and these arguments are denoted via the angle
brackets.
   - When you see `<filename>` it expects a string such as `a.txt` or
     `test.txt`
   - When you see `<quoted>` it expects a quoted string such as `"this is a
     quoted string"`

## The Files Pane The files pane is only updated on boot up (for now). It reads
the filesystem on the generated FAT12 image and displays formatted filenames to
let you know what files are present on the drive. Note: The fat driver is
pretty basic and doesn't support the whole FAT12 spec *yet*.

> [!NOTE] I'm still learning, but the big things missing are directory support,
> and support for files larger than one sector.

> [!NOTE] This pane does not currently live update when changes are made to the
> file system.  If you make a new file, it will not show up in the files pane
> until you `reboot` However when you `list` files, the new file will show up
> You can also interact with the new file with `read` and `appf` just fine

## The Info Pane This area is sort of legacy, and was made during the
development of the UI system to test some of the features. It is a fully static
pane, and is mostly just for showing what happens on bootup.

## The Tips Pane This area of the interface is simply a display of the commands
that are supported by the shell. The list is duplicated in various places for
redundancy.
