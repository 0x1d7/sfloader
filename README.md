# sfloader
Application for loading soundfonts with AWEMAN API

# Info
A program that can upload a new soundfont to soundcards like SB32 and AWE64.\
Soundfont paths can be specified in settings.cfg and choosen with a key.\
The program updates the General Midi soundfont in the registry and\
then uses AWEMAN to reload General Midi, this will upload the specified soundfont\
from the registry.
<br>
The program uses AWEMAN.DLL that need to be installed on the computer.\
The code also uses the sample header files from SB AWE32 Developer Packages.\
These files can be found in the repo: https://github.com/qb40/sound-blaster-awe32
Files used are:\
- AWE_API.H
- AWE_DLL.H
- ERRORMSG.H
- INDICES.H

These files are located in ./include/sfloader/

For easier development on newer machines that does not have the required soundcards installed I have created a mock of the AWEMAN32.DLL.\
This mocked DLL does not have any functionality other than let the code continue running as if the real DLL and soundcard where installed.\

I provided a build script and project settings for VS Code.\
Compiled and built with Open WATCOM 2.0 64bit from repo: https://github.com/open-watcom/open-watcom-v2

Tested with SB32 28MB RAM and AWE64 Soundcards.

# Compiling

Install (Open WATCOM 2.0 64bit)[https://github.com/open-watcom/open-watcom-v2]. Make sure to allow the setup to add environment variables or manually add the environment variables to your user environment variables or system environment variables.

Clone the repository.

Open a PowerShell prompt and navigate to the root of the local repository. Run

```pwsh
.\scripts\build.ps1 -task sfloader -cfgPath .\scripts\build_config.ps1
```

`sfloader.exe` will output to `.\build\bin`.

# Sample configuration file

See `settings.cfg` for a sample configuration file.

# Background
I got a SB32 installed in my retro rig, and I found that I want to use different soundfonts for different games.\
The way I did this was with AWE Control, choosing a new General Midi soundfont and then launch the game.\
This got quite tiresome so I wanted a way to automatically pick a soundfont when launching the game and unload it when done.\
That is why this program now exists, not only does it make it easier to use different soundfont settings depending on game but also quicker and easier to try out different soundfonts.

A quick note, I'm not a C developer and this is the first real program I have written in C.\
So all suggestions and improvements are much appriciated!\
Sorry for all broken C code conventions!\
This is a learning experience.

# TODO
- Messages, both standard and verbose
- Launch argument handling is lacking - Error handling and parsing
- No check for settings.cfg - Create if not found
- Find out if there is a standard registry path for settings
- Crashes i key not found in settings.cfg - Make settings.cfg key case insensitive
- Refactoring

# Help
```sh
Usage:
	sflaoder [options] <arg>

Example:
	sfloader -s <key>
	sfloader -s <key> -v
	sfloader -s <key> -r <registry_key_path>
	sfloader -s <file_path> -r <registry_key_path> -t <synth_type> -v
	sfloader -p <path>
	sfloader -i -v
	sfloader -a

Options:
	-s <key>				Load soundfont <key> from settings
	-p <file_path>			Load soundfont <file_path>
	-r <registry_key_path>	Override regpath in settings.cfg
	-t <type>				Set synth type ('GM', 'GS', 'MT', 'USER')
	-i						Test initialization of AWEMAN
	-a						Check available DRAM
	-l						List soundfont <key> in settings
	-v						Verbose
	-h						Help
```