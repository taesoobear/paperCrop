#!/usr/bin/env lua
local isWin=string.find(string.lower(os.getenv('OS') or 'nil'),'windows')~=nil
if isWin then
	package.path =os.getenv('HOMEDRIVE')..os.getenv('HOMEPATH').."/bin/?.lua" .. ";"..package.path 
	if os.getenv('HOME') then
		package.path =os.getenv('HOME').."/bin/?.lua" .. ";"..package.path 
	end
	HOME = os.getenv('HOMEDRIVE')..os.getenv('HOMEPATH')
else
	package.path =os.getenv('HOME').."/bin/?.lua" .. ";"..package.path 
	HOME = os.getenv('HOME')
end
if _VERSION=='Lua 5.2' then
	require("mylib52")
else
	require("mylib")
end

PAPERCROP_PATH= HOME.. "\\bin\\papercrop_bin"
CWD=os.currentDirectory()

if not os.isFileExist(PAPERCROP_PATH..'\\paperCrop.exe') then
    print('PaperCrop Error! Cannot find paperCrop', HOME, PAPERCROP_PATH)
end


if isWin then
	-- on some systems, os.execute doesn't work perfectly. so  this is a workaround.
	--os.execute_original=os.execute
	--function os.execute(cmd)
	--	local fn= os.createUnnamedBatchFile({cmd})
	--	os.execute_original('"'..fn..'"')
	--end
	g_tmp_folder=os.home_path()..'\\tmp\\'
end
function execute2(...)
	local tbl={...}
	for i,v in pairs(tbl) do
		print(v)
	end
	os.execute2(unpack(tbl))
end
function do_papercrop (fn)
    print(fn)
	local cmd={}
    if fn:sub(1,2)=='~/' then
        table.insert(cmd, 'cd "'..PAPERCROP_PATH..'"')
		table.insert(cmd, '.\\paperCrop "filename=\''..HOME..'/'+fn+'\'"')
	elseif fn:sub(1,2)=='C:' then
		fn=os.shellEscape(fn)
        table.insert(cmd, 'cd "'..PAPERCROP_PATH..'"')
		table.insert(cmd, '.\\paperCrop.exe "filename=\''..fn..'\'"')
    else
        cmd='cd "'..PAPERCROP_PATH..'";.\\paperCrop "filename=\''..CWD..'/'..fn..'\'"'
	end


    execute2(unpack(cmd))

    return true
end

-- main
if #arg==1 then
    do_papercrop(arg[1])
else
    print('usage: paperCrop filename')
end
