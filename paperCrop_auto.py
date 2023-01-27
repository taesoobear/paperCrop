#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
#  Add quote in lyx using lyxclient

import os,sys

HOME = os.environ.get("HOME")
PAPERCROP_PATH= os.path.join(HOME, "bin/papercrop/paperCrop_source")
CWD=os.getcwd()

if not os.path.exists(PAPERCROP_PATH):
	print('PaperCrop Error! Cannot find paperCrop', HOME, PAPERCROP_PATH)

def do_papercrop (fn):
	if fn[0:2]=='~/':
		cmd='cd "'+PAPERCROP_PATH+'";./paperCrop_console "filename=\''+HOME+'/'+fn+'\';auto=true"&'
	elif fn[0:1]=='/':
		cmd='cd "'+PAPERCROP_PATH+'";./paperCrop_console "filename=\''+fn+'\';auto=true"&'
	else:
		cmd='cd "'+PAPERCROP_PATH+'";./paperCrop_console "filename=\''+CWD+'/'+fn+'\';auto=true"&'

	print('$ '+cmd)
	print(' ')
	print('Launching paperCrop ...')
	print('You can ignore Xdbe error')
	os.system(cmd)

	return True

# main
if len(sys.argv)>1:
	do_papercrop(sys.argv[1])
else:
	print('usage: paperCrop filename')
