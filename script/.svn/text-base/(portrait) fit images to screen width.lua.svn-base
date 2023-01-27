
function processPage(outdir, pageNo, numRects)
	local width=device_width
	local height=device_height
	for rectNo=0, numRects-1 do
		win:setStatus("processing"..pageNo.."_"..rectNo)
		local image=CImage()
		win:getRectImage_width(pageNo, rectNo, width, image)
		postprocessImage(image)
		outputImage(image, outdir,pageNo,rectNo)

	end

	if numRects==0 then
		win:setStatus("Error! no rects were specified.")
		return 0
	end			
	return 1
end


function processAllPages(outdir)
	initializeOutput(outdir)
	for pageNo=0, win:getNumPages()-1 do
		win:setCurPage(pageNo)
		if processPage(outdir, pageNo, win:getNumRects())==0 then
			return 0
		end
	end
	finalizeOutput(outdir)
	return 1
end
