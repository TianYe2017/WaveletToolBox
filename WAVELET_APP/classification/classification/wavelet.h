#pragma once
#ifndef WAVELET_H
#define WAVELET_H

#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#define WT_MODE_DEMO -1
#define WT_MODE_1D 1
#define WT_MODE_2D 2

struct SIZE
{
	size_t rows;
	size_t cols;
	SIZE() 
	{
	
	}
	SIZE(size_t r, size_t c)
	{
		rows = r;
		cols = c;
	}
};

struct TransformedData
{
	vector<float> lowPassed;
	vector<float> highPassed;
	TransformedData(vector<float> lp, vector<float> hp)
	{
		lowPassed = lp;
		highPassed = hp;
	}
};

struct Filters
{
	vector<float> lowpass;
	vector<float> highpass;
	vector<float> recLowpass;
	vector<float> recHighpass;
	Filters() 
	{
	}
	Filters(vector<float> lp, vector<float> hp, vector<float> recLp, vector<float> recHp)
	{
		lowpass = lp;
		highpass = hp;
		recLowpass = recLp;
		recHighpass = recHp;
	}
};

struct response1D 
{
	vector<float> lowpassed;
	vector<float> highpassed;
	int level;
};

struct response2D 
{
	vector<vector<float>> mapLL;
	vector<vector<float>> mapLH;
	vector<vector<float>> mapHL;
	vector<vector<float>> mapHH;
	int level;
};

struct data1D 
{
	size_t olen;
	vector<float> data;
};

struct data2D 
{
	SIZE S;
	vector<vector<float>> data;
};

class WaveletToolBox 
{

public:
	WaveletToolBox(int Dim, int Wt, string typeFilter);
	WaveletToolBox();
	void Test1D(void);
	void Test2D(void);
	vector<response1D> Dwt1D(vector<float> data, int level, string FilterType);
	vector<response2D> Dwt2D(vector<vector<float>> data, int level, string FilterType);
	



private:
	int flagDim = -1; //mode selection -1: invalid; 1: 1D; 2: 2D 
	int flagWt = 1; //1:DWT 2:CWT

	data1D data1d;
	data2D data2d;
	Filters filters;
	vector<response1D> feature1D;
	vector<response2D> feature2D;

	void SetupFilters(string typeFilter);
	vector<float> ConvForward(vector<float> filter, vector<float> data, bool reversed, bool debug);
	vector<float> ConvBackward(vector<float> filter, vector<float> data, bool reversed, bool debug);
	size_t ZeroPadding2N_1D(vector<float>& data);
	SIZE ZeroPadding2N_2D(vector<vector<float>>& data);
	void Crop_1D(vector<float>& data, size_t len);
	void Crop_2D(vector<vector<float>>& data, SIZE S);
	vector<float> DownSample(vector<float> data);
	vector<float> UpSample(vector<float> data);
	void AdjustData1D(vector<float> data);
	void AdjustData2D(vector<vector<float>> data);
	response1D StepForward(vector<float> &data, Filters filters, int level, bool reversed, bool debug);
	void StepBackward(vector<float> &data, Filters filters, int level, bool reversed, bool debug);
	void Dwt1DForward(int level, bool reversed, bool debug);
	void Dwt1DBackward(int level, bool reversed, bool debug);
	void Dwt2DForward(int level, bool reversed, bool debug);
	void Dwt2DBackward(int level, bool reversed, bool debug);
	

	





};




















#endif // !"WAVELET_H"
