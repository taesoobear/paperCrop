# paperCrop 

Multi-column PDF files on 6 to 10 inch displays.

About
=
This program converts PDF documents such as articles, and academic papers into a PDF (or image sequence) so that it can be read on a small screen of e-book devices or smartphones.


Usage
=
Open a PDF file (by clicking Load button, or drag-and-drop)

Choose a preset by clicking the default preset button

Click "Process all pages" button

Output will be in the same folder with the input file. (~_1.pdf)_

Usage 2 (advanced)
=
Open a PDF file (by clicking Load button, or drag-and-drop)

(Optional) choose how many columns are in the pdf file, and then click the update button.

if results are not satisfactory, modify the MIN gap, MIN text gap and margin parameters, and then click the update button.

(Optional) choose a rendering option by clicking the option button

Process all pages.

Usage 3 (manual segmentation)
=
Open a PDF file (by clicking Load button, or drag-and-drop)

Uncheck automatic segmentation

Select a region by clicking, delete one by hitting DEL when necessary, or create one by dragging.

Process all pages or Process current page

FAQ
=
How can I change output aspect ratio?
  Open config.lua and edit "vector_PDF={600,800,..." appropriately.

How can I create a preset?
  Copy presets/one-column papers.lua to a new file (e.g. "presets/ACM TOG.lua"), and edit it.

How can I change the default preset
  Edit presets/default preset.lua. It points to one of the preset files.

The output folder is empty. Why?
  The output file xxx_1.pdf is in the same folder as the input pdf (if you chose "cancel" on the file saving dialog). Not in the xxx folder.


The following FAQs are irrelevant to the default "vector PDF" device.

How can I change output format?
  Edit config.lua. Find ".gif" and replace it as .jpg, .bmp or .png. Don't forget to set output_to_pdf=false if desired output format is NOT pdf.

How can I improve readability?
  Edit config.lua. One can adjust image processing parameters such as sharpen, contrast, gamma, dithering, and color depth.

Output is gray. Why not color?
  Color pdf is usually larger. If you need colors, then edit config.lua and edit color_depth=8 -> color_depth=24. (From version 0.43)
This program is for converting technical documents k
(revived an old repository because my kindle scribe doesn't accept many pdf files. This tool can convert any pdf to work on kindle scribe.)


Linux (Ubuntu or some debian distributions) 
=

# How to install.

```
make install_dependencies_ubuntu
make
make install
```

This script installs paperCrop in the ~/bin folder. 

Mac 
=
```
make install_dependencies_mac
make
make install
```

This script installs paperCrop in the ~/bin folder. 


* How to execute.

1. Easy way:

Assuming ~/bin is in the path, simply type in a terminal
```
paperCrop aaa.pdf
```
Or, you can use
```
~/bin/paperCrop aaa.pdf
```
Here, paperCrop is a python script located in your ~/bin.

2. Using a file manager:

Run "papercrop" in the "~/bin/papercrop/paperCrop_bin".

3. Alternatively, in a terminal:
```
~/bin/paperCrop
```


How to uninstall
=
Just delete the install folder (~/bin/papercrop_bin).
