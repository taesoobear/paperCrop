#!/usr/bin/env python3
# -*- coding: utf-8 -*-
 
#  Add quote in lyx using lyxclient

import os,sys

HOME = os.environ.get("HOME")
PAPERCROP_PATH= os.path.join(HOME, "bin/papercrop_bin")
CWD=os.getcwd()

if not os.path.exists(PAPERCROP_PATH):
	print('PaperCrop Error! Cannot find paperCrop', HOME, PAPERCROP_PATH)

def do_papercrop (fn):
	if fn[0:2]=='~/':
		cmd='cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+HOME+'/'+fn+'\'"&'
	elif fn[0:1]=='/':
		cmd='cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+fn+'\'"&'
	else:
		cmd='cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+CWD+'/'+fn+'\'"&'

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
	os.system('cd "'+PAPERCROP_PATH+'";./paperCrop&')
