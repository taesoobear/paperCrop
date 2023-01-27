#pragma once

class intIntervals
{
public:
	intIntervals(){}
	~intIntervals(){}
	int numInterval() const			{ return m_vStart.size();}
	int size() const				{ return numInterval();}

	void setSize(int n)				{ m_vStart.setSize(n); m_vEnd.setSize(n);}
	void resize(int n)				{ m_vStart.resize(n); m_vEnd.resize(n);}
	void removeInterval(int i);
	int& start(int iInterval) const	{ return m_vStart[iInterval];}
	int& end(int iInterval) const	{ return m_vEnd[iInterval];}
	void load(const char* filename);
	void pushBack(int start, int end) 	{m_vStart.pushBack(start);m_vEnd.pushBack(end);}

	// [start, end) 과 겹치는 interval의 index를 return. 없으면 -1
	int findOverlap(int start, int end, int startInterval=0);

	/**
	* RunLength encoding을 구한다.
	* [1 1 1 0 0 0 0 1 0] -> [0 3], [7 8]
	* 0 부터 3까지 1이 나오고, 7부터 8까지 나오고 1이 나온다는 뜻.	 
	*/
	void runLengthEncode(const bitvectorn& source, int start=0, int end=INT_MAX);
	

	/**
	* RunLength encoding을 구한다.
	* [1 1 1 3 3 3 3 4 4] -> [0 3], [3 7], [7 9]  
	* 0 부터 3까지 1이 나오고, 3부터 7까지 3이 나오고, 4가 9까지 나온다는 뜻.
	*/
	void runLengthEncode(const intvectorn& source);

	/**
	* [123789] -> [0 3], [3 6]
	*/
	void findConsecutiveIntervals(const intvectorn& source);

	/**
	* CutState의 RunLength encoding을 구한다.
	* depricated. -> moved into intIntervals class.
	* [1 0 0 1 0 0 0 1 0] -> [0 3, 3 7, 7 9]
	* 0 부터 3까지 가 첫번째 구간, 3~7이 두번째 구간, 7~9가 세번째 구간.*/
	void runLengthEncodeCut(const bitvectorn& cutState, int start=0, int end=INT_MAX);

	void encodeIntoVector(intvectorn& out);
	void decodeFromVector(const intvectorn& in);
	void offset(int offset);

	void toBitvectorn(bitvectorn& bitVector);

	intvectorn m_vStart;
	intvectorn m_vEnd;
};

