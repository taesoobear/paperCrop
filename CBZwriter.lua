
require('mylib')
do 
	ImageWriter=LUAclass()


	function ImageWriter:__init(fn)
		self.filename=fn
		self.numPages=0

		if os.isFileExist(fn) then
			os.execute('rm -rf "'..fn..'/*"')
		end
		os.createDir(fn)
	end

	function ImageWriter:addPage(image, color_depth)
		local fn=string.format("Page%04d"..device.output_format,self.numPages+1)
		image:save(self.filename..'/'..fn, color_depth)
		self.numPages=self.numPages+1
	end
	function ImageWriter:save()
	end
	function ImageWriter:init()
		-- compatibility function. does nothing
	end
	function ImageWriter:addPageColor(image)
		-- compatibility function
		self:addPage(image,color_depth)
	end
end
CBZwriter=LUAclass()


function CBZwriter:__init(fn)
	self.filename=fn
	self.numPages=0

	if os.isFileExist(fn) then
		os.deleteFiles(fn)
	end
	if os.isFileExist("temp.zip") then
		os.deleteFiles("temp.zip")
	end
end
function CBZwriter:addPage(image, color_depth)
	local fn=string.format("page%06d.png",self.numPages+1)
	image:save(fn, color_depth)
	os.execute("zip -m -1 temp.zip "..fn)
	self.numPages=self.numPages+1
end
function CBZwriter:save()
	-- compatibility function. does nothing
	os.copyFile('temp.zip "'..self.filename..'"')
	os.deleteFiles("temp.zip")
end
function CBZwriter:init()
	-- compatibility function. does nothing
end
function CBZwriter:addPageColor(image)
	-- compatibility function
	self:addPage(image,24)
end

do
	XMLwriter=LUAclass()

	function XMLwriter:__init(fn, book_pages, outdir)
		self.outdir=outdir
		self.filename=fn
		self.book_pages=book_pages

		--if os.isFileExist(fn) then
		--	os.deleteFiles(fn)
		--end
		self.pages={}
	end
	function XMLwriter.processRects(tbl)
		local targetRects={}
		local function width(trect)
			return trect.right-trect.left
		end
		local function height(trect)
			return trect.bottom-trect.top
		end
		local function scale(trect,s)
			return {left=trect.left*s, top=trect.top*s, right=trect.right*s, bottom=trect.bottom*s, scalef=trect.scalef*s}
		end
		local function translateY(trect,y)
			return {left=trect.left, top=trect.top+y, right=trect.right, bottom=trect.bottom+y, scalef=trect.scalef}
		end
		local totalHeight=0
		for i=1,#tbl do
			local trect={left=0, top=0, right=tbl[i]:right()-tbl[i]:left(), bottom=tbl[i]:bottom()-tbl[i]:top(), scalef=1}
			local scalef=math.min(1/width(trect), 3)
			scalef=math.max(scalef,1)
			trect=scale(trect, scalef)
			trect=translateY(trect, totalHeight)
			totalHeight=totalHeight+height(trect)
			targetRects[#targetRects+1]=trect
		end
		return targetRects
	end
	function XMLwriter:addPage(image, color_depth)
		for i=1, self.book_pages.cache.pages:size() do
			local pdfPage=self.book_pages.cache.pages[i]
			local cpage=self.book_pages.cache[pdfPage]
			for j=1,#cpage do
				local tbl2={}
				local tbl=cpage[j].src
				local target_tbl=cpage[j].tgt
				for i=1,#tbl do
					tbl2[i]={tbl[i]:left(), tbl[i]:top(), tbl[i]:right(), tbl[i]:bottom(), 
					target_tbl[i].left, target_tbl[i].top, target_tbl[i].right, target_tbl[i].bottom, 
					target_tbl[i].scalef}
				end
				self.pages[#self.pages+1]=tbl2
				self.pages[#self.pages].pdfPage=pdfPage+1
			end
		end
		self.book_pages:clearCache()
	end
	function XMLwriter:save()
		local str=table.tostring(self.pages)
		str=string.gsub(str, "%['","")
		str=string.gsub(str, "%']","")
		util.writeFile(self.filename, str)
		self.pages={}
		local src=self.outdir..".pdf"
		local tgt=string.sub(self.filename,1,-4).."pdf"
		local xml=self.filename
		print(src, tgt, xml)
		local cmd

		if os.isUnix() then
			cmd="java -cp bin:lib/iText.jar:lib/iText-xtra.jar PdfCrop '"..src.."' '"..tgt.."' '"..xml.."'"
		else
			cmd='java -cp bin;lib/iText.jar;lib/iText-xtra.jar PdfCrop "'..src..'" "'..tgt..'" "'..xml..'"'
 		end
		os.execute2("cd pdfCrop", cmd);
	end
	function XMLwriter:init()
		-- compatibility function. does nothing
	end
	function XMLwriter:addPageColor(image)
		assert(false)
	end
end
