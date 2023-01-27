width=device_width
height=device_height

-- outdir: output directory
-- pageNo: current page
-- numRects: # of crop rectangles.
function processPage(outdir, pageNo, numRects)

	-- create merged image imageM
	local imageM=CImage()
	processPageSubRoutine(imageM, pageNo, width,numRects)
    postprocessImage(imageM)

    splitImage(imageM, height, outdir, pageNo, false)

	if numRects==0 then
		win:setStatus("Error! no rects were specified.")
		return 0
	end			
	return 1
end

function processAllPages(outdir)
	initializeOutput(outdir)
	local imageM=CImage()
	local pageNo=0
	while pageNo<win:getNumPages() do
		win:setCurPage(pageNo)
		if processPageSubRoutine(imageM, pageNo, width, win:getNumRects())==0 then
			return 0
		end
		splitImagePart(imageM, height, outdir, pageNo, false)
		pageNo=pageNo+1
	end	
	postprocessImage(imageM)
    	splitImage(imageM, height, outdir, pageNo, false)
	finalizeOutput(outdir)
end
