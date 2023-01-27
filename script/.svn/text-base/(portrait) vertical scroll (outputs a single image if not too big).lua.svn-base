width=device_width
height=device_height
max_height=height*10.1

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

	local imageM=CImage()

	local curOut=0

	for pageNo=0, win:getNumPages()-1 do
		win:setCurPage(pageNo)
		if processPageSubRoutine(imageM, pageNo, width, win:getNumRects())==0 then
			return 0
		end        	

		if imageM:GetHeight()>max_height then
				
		    	postprocessImage(imageM)
			outputImage(imageM, outdir, curOut, 0)
			imageM=CImage()
			curOut=curOut+1
		end
	end
	postprocessImage(imageM)
	outputImage(imageM, outdir, curOut, 0)
	finalizeOutput(outdir)
end
