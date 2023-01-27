require('mylib')
function detectVC()

   local VC8_path="c:\\Program Files\\Microsoft Visual Studio 8"
   local VC9_path="c:\\Program Files\\Microsoft Visual Studio 9.0"

   if os.isFileExist(VC8_path.."\\readme.htm") then
	   print("VC8 detected")
	   devenvExe=VC8_path.."\\Common7\\IDE\\devenv.com"
	   setVar=util.readFile(VC8_path.."\\Common7\\Tools\\vsvars32.bat" )
   elseif os.isFileExist(VC9_path.."\\readme.htm") then
      print("VC9 detected")
      devenvExe=VC9_path.."\\Common7\\IDE\\devenv.com"      
      setVar=util.readFile(VC9_path.."\\Common7\\Tools\\vsvars32.bat" )
      usedVC9=true
  else
	  print("Critical Error! Cannot find visual studio. Check nmake.lua")
  end
end

detectVC()

function devenvbuild(build_folder, slnFile, target, build_opt)

   local targetName="Visual Studio 8 2005"
   if usedVC9 then
      targetName="Visual Studio 9 2008"
   end

   build_opt=build_opt or "/build"

   local msg="\nStart Building "..slnFile.."|"..target.." in "..build_folder.."\n"
   print(msg)
   local cmd='"'..devenvExe..'" '..build_opt..' "'..target..'"'..build_folder.."/"..slnFile
   print(cmd)
   execute(cmd)
end



function cmake(build_folder, opt, target, build_opt, build_path)
   local targetName="Visual Studio 8 2005"
   if usedVC9 then
      targetName="Visual Studio 9 2008"
	  opt=opt.." -D UseVC9:boolean=true"
   end

   build_opt=build_opt or "/build"
   build_path=build_path or ".."
   if build_opt=="/rebuild" then
      execute("cd "..build_folder, "rm CMakeCache.txt")
   end

   local cmd='cmake -G "'..targetName..'"'..opt.." "..build_path

   print("cd "..build_folder.."\n"..cmd)
   execute("@echo off", "cd "..build_folder, "cd", cmd)

end

-- cmake followed by devenvbuild
function devenvmake(build_folder, opt, target, build_opt, build_path, slnFile)
   cmake(build_folder, opt, target, build_opt, build_path)
   slnFile=slnFile or "paperCrop.sln"
   devenvbuild(build_folder,slnFile, target, build_opt)   
end

function devenvInstall(folder, definitions, build_tgt, build_opt, executable)
   local def=""
   for i,v in ipairs(definitions) do def=def.." -D "..v end
   devenvmake(folder, def, build_tgt.."|Win32", build_opt)

   os.execute("copy "..folder.."\\"..build_tgt.."\\paperCrop.exe .\\"..executable)

end

-- build
if arg[1]=="rebuild" or arg[2]=="rebuild" then
	arg[3]="/rebuild" 
end
if arg[1]=='debug' then -- for debugging.
	devenvInstall("build", {}, "Debug", arg[3], "paperCrop_d.exe")
else
	devenvInstall("build", {}, "Release", arg[3], "paperCrop.exe")
end
