# paperCrop 

(revived an old repository because my kindle scribe doesn't accept many pdf files. This tool can convert any pdf to work on kindle scribe.)

Linux (Ubuntu or some debian distributions) 
=

* How to install.
{{{
make install_dependencies_ubuntu
make
make install
}}}

This script installs paperCrop in the ~/bin folder. 

Mac 
=
{{{
make install_dependencies_mac
make
make install
}}}

This script installs paperCrop in the ~/bin folder. 


* How to execute.

1. Easy way:

Assuming ~/bin is in the path, simply type in a terminal
{{{
paperCrop aaa.pdf
}}}
Or, you can use
{{{
~/bin/paperCrop aaa.pdf
}}}
Here, paperCrop is a python script located in your ~/bin.

2. Using a file manager:

Run "papercrop" in the "~/bin/papercrop/paperCrop_bin".

3. Alternative way in a terminal:
{{{
~/bin/paperCrop
}}}


How to uninstall
=
Just delete the install folder (~/bin/papercrop_bin).
