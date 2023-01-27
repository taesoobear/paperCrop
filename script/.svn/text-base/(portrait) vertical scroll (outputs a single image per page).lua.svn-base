width=device_width
height=device_height

function processPage(outdir, pageNo, numRects)
	local imageM=CImage()
	processPageSubRoutine(imageM, pageNo, width,numRects)
    postprocessImage(imageM)
	outputImage(imageM,outdir,pageNo,0)

	if numRects==0 then
		win:setStatus("Error! no rects were specified.")
		return 0
	end			
	return 1
end

function processAllPages(outdir)
	initializeOutput(outdir)
	for pageNo=0, win:getNumPages()-1 do
		local imageM=CImage()
		win:setCurPage(pageNo)
		if processPageSubRoutine(imageM, pageNo, width, win:getNumRects())==0 then
			return 0
		end        		
	    	postprocessImage(imageM)
		outputImage(imageM, outdir, pageNo, 0)
	end
	finalizeOutput(outdir)
end
