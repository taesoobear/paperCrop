#include "stdafx.h"
#include "tvector.h"

vector2NView vector2N::range(int start, int end, int step)
{
	return _range<vector2NView >(start,end,step);
}

const vector2NView vector2N::range(int start, int end, int step) const	
{
	return ((const vector2N*)this)->range(start, end, step);
}
