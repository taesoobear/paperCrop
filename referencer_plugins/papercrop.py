#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
#  Add quote in lyx using lyxclient

import os
import referencer
from referencer import _

referencer_plugin_info = {
	"longname": _("PaperCrop"),
	"author": "Taesoobear",
	"ui":
		"""
		<ui>
			<menubar name='MenuBar'>
				<menu action='DocMenu'>
				<placeholder name='PluginDocMenuActions'>
					<menuitem action='_plugin_papercrop'/>
				</placeholder>
				</menu>
			</menubar>
			<toolbar name='ToolBar'>
			<placeholder name='PluginToolBarActions'>
				<toolitem action='_plugin_papercrop'/>
			</placeholder>
			</toolbar>
			<popup name='DocPopup'>
			<placeholder name='PluginDocPopupActions'>
				<menuitem action='_plugin_papercrop'/>
			</placeholder>
			</popup>

		</ui>
		"""
}

referencer_plugin_actions = [
	{
	"name":        "_plugin_papercrop",
	"label":     _("open with PaperCrop"),
	"tooltip":   _("open with PaperCrop"),
	"icon":        "papercrop.png",
	"callback":    "do_papercrop",
	"accelerator": "<control><shift>p"
	}
]

LYXPIPE = None
HOME = os.environ.get("HOME")
PAPERCROP_PATH= os.path.join(HOME, "bin/papercrop/paperCrop_source")

if not os.path.exists(PAPERCROP_PATH):
	print('PaperCrop Error! Cannot find paperCrop', HOME, PAPERCROP_PATH)

def do_papercrop (library, documents):
	empty=True
	for document in documents:
		print("Do papercrop")
		print(library)
		print(document)
		print(document.get_key())
		print(document.get_filename())
		# print(document.get_title())
		# print(document.get_authors())
		empty=False
		print('cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+document.get_filename()[5:]+'\'"&')
		os.system('cd "'+PAPERCROP_PATH+'";./paperCrop "filename=\''+document.get_filename()[5:]+'\'"&')

	if empty:
		raise "No docuemnts"
		return False
	
	return True

