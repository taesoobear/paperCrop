width=device_width
height=device_height
right_max_margin=0.1
figure_min_height=100
max_vspace_reflow=6 -- pixels

-- outdir: output directory
-- pageNo: current page
-- numRects: # of crop rectangles.
function processPage(outdir, pageNo, numRects)

	-- create merged image imageM
	local imageM=CImage()
	processPageSubRoutine(imageM, pageNo, width*((1.0-right_max_margin)*2),numRects)
    postprocessImage(imageM)
	reflow(imageM, width, 2, max_vspace_reflow, 255, right_max_margin, figure_min_height)

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
		local imageS=CImage()
		processPageSubRoutine(imageS, pageNo, width*((1.0-right_max_margin)*2),win:getNumRects())
		reflow(imageS, width, 2, 2, 255, right_max_margin, figure_min_height)		imageM:concatVertical(imageM, imageS)
		splitImagePart(imageM, height, outdir, pageNo, false)
		pageNo=pageNo+1
	end	
	postprocessImage(imageM)
	splitImage(imageM, height, outdir, pageNo, false)
	finalizeOutput(outdir)
end
