#include "wavelet.h"


using namespace std;

WaveletToolBox::WaveletToolBox(int Dim, int Wt, string typeFilter)
{
	flagDim = Dim;
	flagWt = Wt;
	SetupFilters(typeFilter);
}

WaveletToolBox::WaveletToolBox()
{

}

vector<float> WaveletToolBox::ConvForward(vector<float> filter, vector<float> data, bool reversed, bool debug)
{
	if (debug)
	{
		cout << "input: " << endl;
		for (size_t j = 0; j < data.size(); j++)
		{
			cout << data[j] << " ";
		}
		cout << "" << endl;
	}
	if (!reversed) 
	{
		reverse(filter.begin(),filter.end()); 
	}
	size_t flen = filter.size();
	size_t datalen = data.size();
	flen -= 1;
	for (size_t i = 0; i < flen; i++)
	{
		data.insert(data.begin(), data[data.size() - 1 - i]);
	}
	if (debug)
	{
		cout << "input_padded: " << endl;
		for (size_t j = 0; j < data.size(); j++)
		{
			cout << data[j] << " ";
		}
		cout << "" << endl;
	}
	vector<float> output;
	flen++;
	for (size_t i = 0; i < datalen; i++)
	{
		float sum = 0.0f;
		for (size_t j = 0; j < flen; j++)
		{
			sum += data[i + j] * filter[j];
		}
		output.push_back(sum);
	}
	if (debug) 
	{
		cout << "output: " << endl;
		for (size_t j = 0; j < output.size(); j++)
		{
			cout << output[j] << " ";
		}
		cout << "" << endl;
	}
	return output;
}

vector<float>  WaveletToolBox::ConvBackward(vector<float> filter, vector<float> data, bool reversed, bool debug)
{
	if (debug)
	{
		cout << "input: " << endl;
		for (size_t j = 0; j < data.size(); j++)
		{
			cout << data[j] << " ";
		}
		cout << "" << endl;
	}
	if (!reversed)
	{
		reverse(filter.begin(), filter.end());
	}
	size_t flen = filter.size();
	size_t datalen = data.size();
	flen -= 1;
	for (size_t i = 0; i < flen; i++)
	{
		data.push_back(data[i]);
	}
	if (debug)
	{
		cout << "input_padded: " << endl;
		for (size_t j = 0; j < data.size(); j++)
		{
			cout << data[j] << " ";
		}
		cout << "" << endl;
	}
	vector<float> output;
	flen++;
	for (size_t i = 0; i < datalen; i++)
	{
		float sum = 0.0f;
		for (size_t j = 0; j < flen; j++)
		{
			sum += data[i + j] * filter[j];
		}
		output.push_back(sum);
	}
	if (debug)
	{
		cout << "output: " << endl;
		for (size_t j = 0; j < output.size(); j++)
		{
			cout << output[j] << " ";
		}
		cout << "" << endl;
	}
	return output;
}

size_t  WaveletToolBox::ZeroPadding2N_1D(vector<float>& data)
{
	size_t N = data.size();
	size_t base = 2;
	while (base < N) 
	{
		base *= 2;
	}
	base = base - N;
	for (size_t i = 0; i < base; i++) 
	{
		data.push_back(0.0f);
	}
	return N;
}

SIZE  WaveletToolBox::ZeroPadding2N_2D(vector<vector<float>>& data)
{
	size_t rows = data.size();
	size_t cols = data[0].size();

	size_t N = max(rows,cols);
	size_t base = 2;
	while (base < N)
	{
		base *= 2;
	}

	vector<vector<float>> ans;
	vector<float> tmp;

	for (size_t i = 0; i < base; i++) 
	{
		tmp.push_back(0.0f);
	}
	for (size_t i = 0; i < base; i++) 
	{
		ans.push_back(tmp);
	}
	for (size_t row = 0; row < rows; row++) 
	{
		for (size_t col = 0; col < cols; col++) 
		{
			ans[row][col] = data[row][col];
		}
	}

	SIZE S = SIZE(rows,cols);
	data = ans;
	return S;
}

void WaveletToolBox:: Crop_1D(vector<float>& data, size_t len)
{
	size_t N = data.size() - len;
	for (size_t i = 0; i < N; i++) 
	{
		data.pop_back();
	}
}

void WaveletToolBox::Crop_2D(vector<vector<float>>& data, SIZE S)
{
	size_t rows = S.rows;
	size_t cols = S.cols;

	vector<vector<float>> ans;
	vector<float> tmp;
	for (size_t i = 0; i < cols; i++) 
	{
		tmp.push_back(0.0f);
	}
	for (size_t i = 0; i < rows; i++) 
	{
		ans.push_back(tmp);
	}
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t col = 0; col < cols; col++)
		{
			ans[row][col] = data[row][col];
		}
	}
	data = ans;
}

vector<float> WaveletToolBox::DownSample(vector<float> data)
{
	vector<float> ans;
	for (size_t i = 0; i < data.size(); i++) 
	{
		if (i % 2 == 1) 
		{
			ans.push_back(data[i]);
		}
	}
	return ans;
}

vector<float> WaveletToolBox::UpSample(vector<float> data)
{
	size_t N = 2 * data.size();
	vector<float> ans;
	for (size_t i = 0; i < N; i++) 
	{
		ans.push_back(0.0f);
	}
	for (size_t i = 0; i < N; i++) 
	{
		if (i % 2 == 1) 
		{
			ans[i] = data[(i - 1) >> 1];
		}
	}
	return ans;
}

void WaveletToolBox::SetupFilters(string typeFilter) 
{
	vector<float> lp, hp, rlp, rhp;
	if (typeFilter == "Haar") 
	{
		lp.push_back(0.707106);
		lp.push_back(0.707106);
		hp.push_back(-0.707106);
		hp.push_back(0.707106);
		rlp.push_back(0.707106);
		rlp.push_back(0.707106);
		rhp.push_back(0.707106);
		rhp.push_back(-0.707106);
		filters.lowpass = lp;
		filters.highpass = hp;
		filters.recLowpass = rlp;
		filters.recHighpass = rhp;
		cout << "Filter: Haar" << endl;
		return;
	}
	if (typeFilter == "Dp2") 
	{
		lp = {-0.129409523f,0.224143868f,0.836516304f,0.482962913f};
		hp = { -0.482962913f,0.836516304f,-0.224143868f,-0.129409523f};
		rlp = {0.482962913f,0.836516304f,0.224143868f,-0.129409523f};
		rhp = {-0.129409523f,-0.224143868f,0.836516304f,-0.482962913f};
		filters.lowpass = lp;
		filters.highpass = hp;
		filters.recLowpass = rlp;
		filters.recHighpass = rhp;
		cout << "Filter: Dp2" << endl;
		return;
	}
	if (typeFilter == "Dp3") 
	{
		lp = { 0.035226392f,-0.085441274f,-0.13501102f,0.459877502f,0.806891509f,0.332670553f };
		hp = { -0.332670553f,0.806891509f,-0.459877502f,-0.13501102f,0.085441274f,0.035226292f};
		rlp = { 0.332670553f,0.806891509f,0.459877502f,-0.13501102f,-0.085441274f,0.035226292f};
		rhp = { 0.035226292f,0.085441274f,-0.13501102f,-0.459877502f,0.806891509f,-0.332670553f};
		filters.lowpass = lp;
		filters.highpass = hp;
		filters.recLowpass = rlp;
		filters.recHighpass = rhp;
		cout << "Filter: Dp3" << endl;
		return;
	}




}

void WaveletToolBox::AdjustData1D(vector<float> data) 
{
	vector<float> tmp = data;
	data1d.olen = ZeroPadding2N_1D(tmp);
	data1d.data = tmp;
}

void WaveletToolBox::AdjustData2D(vector<vector<float>> data) 
{
	vector<vector<float>> tmp = data;
	data2d.S = ZeroPadding2N_2D(tmp);
	data2d.data = tmp;
}

response1D WaveletToolBox::StepForward(vector<float> &data, Filters filters, int level, bool reversed, bool debug)
{
	int base = 1;
	for (int i = 0; i < level; i++) 
	{
		base *= 2;
	}
	size_t realLen = data.size() / base;
	vector<float> tmp;
	for (size_t i = 0; i < realLen; i++) 
	{
		tmp.push_back(data[i]);
	}
	vector<float> lowpassed = DownSample(ConvForward(filters.lowpass, tmp, reversed, debug));
	vector<float> highpassed = DownSample(ConvForward(filters.highpass,tmp, reversed, debug));
	for (size_t i = 0; i < realLen / 2; i++) 
	{
		data[i] = lowpassed[i];
	}
	for (size_t i = realLen / 2; i < realLen; i++) 
	{
		data[i] = highpassed[i- realLen / 2];
	}
	response1D feature;
	feature.level = level;
	feature.highpassed = highpassed;
	feature.lowpassed = lowpassed;
	return feature;
}

void WaveletToolBox::StepBackward(vector<float> &data, Filters filters, int level,bool reversed, bool debug)
{
	int base = 1;
	for (int i = 0; i < level; i++)
	{
		base *= 2;
	}
	size_t realLen = data.size() / base;
	vector<float> lowpassed, highpassed;
	for (size_t i = 0; i < realLen; i++)
	{
		lowpassed.push_back(data[i]);
	}
	for (size_t i = realLen; i < 2 * realLen; i++) 
	{
		highpassed.push_back(data[i]);
	}
	lowpassed = ConvBackward(filters.recLowpass, UpSample(lowpassed), reversed, debug);
	highpassed = ConvBackward(filters.recHighpass, UpSample(highpassed), reversed, debug);
	for (size_t i = 0; i < realLen * 2; i++) 
	{
		data[i] = lowpassed[i] + highpassed[i];
	}                                                                                                                                                                                                                        
}

void WaveletToolBox::Dwt1DForward(int level, bool reversed, bool debug)
{
	feature1D.clear();
	int base = 1;
	for (int i = 0; i < level - 1; i++)
	{
		base *= 2;
	}
	size_t endLen = data1d.data.size() / base;
	if (endLen < filters.highpass.size()) 
	{
		cout << "level is too high for input" << endl;
		return;
	}
	for (int i = 0; i < level; i++) 
	{
		response1D respond = StepForward(data1d.data, filters, i, reversed, debug);
		feature1D.push_back(respond);
	}
	return;
}

void WaveletToolBox::Dwt1DBackward(int level, bool reversed, bool debug)
{
	for (int i = level; i > 0; i--) 
	{
		StepBackward(data1d.data, filters, i, reversed, debug);
	}
	Crop_1D(data1d.data, data1d.olen);
}

void WaveletToolBox::Dwt2DForward(int level, bool reversed, bool debug)
{
	feature2D.clear();
	int base = 1;
	for (int i = 0; i < level - 1; i++)
	{
		base *= 2;
	}
	size_t endLen = data2d.data.size() / base;
	if (endLen < filters.highpass.size())
	{
		cout << "level is too high for input" << endl;
		return;
	}
	base = 1;
	vector<float> tmp;
	for (int k = 0; k < level; k++) 
	{
		cout << "computing wavelet coef...   level = " << k << endl;
		size_t N = data2d.data.size() / base;
		vector<vector<float>> LL(N / 2, vector<float>(N / 2, 0));
		vector<vector<float>> LH(N / 2, vector<float>(N / 2, 0));
		vector<vector<float>> HL(N / 2, vector<float>(N / 2, 0));
		vector<vector<float>> HH(N / 2, vector<float>(N / 2, 0));

		//process rows
		for (size_t i = 0; i < N; i++) 
		{
			tmp = data2d.data[i];
			StepForward(tmp, filters, k, reversed, debug);
			for (size_t j = 0; j < N; j++) 
			{
				data2d.data[i][j] = tmp[j];
			}
		}
		//process cols
		for (size_t i = 0; i < N; i++) 
		{
			tmp.clear();
			for (size_t j = 0; j < data2d.data[0].size(); j++) 
			{
				tmp.push_back(data2d.data[j][i]);
			}
			StepForward(tmp, filters, k, reversed, debug);
			for (size_t j = 0; j < N; j++)
			{
				data2d.data[j][i] = tmp[j];
			}
		}
		//gather features LL
		tmp.clear();
		for (size_t i = 0; i < N / 2; i++) 
		{
			for (size_t j = 0; j < N / 2; j++) 
			{
				LL[i][j] = data2d.data[i][j];
			}
		}
		for (size_t i = 0; i < N / 2; i++)
		{
			for (size_t j = N / 2; j < N; j++)
			{
				LH[i][j - N / 2] = data2d.data[i][j];
			}
		}
		for (size_t i = N / 2; i < N; i++)
		{
			for (size_t j = 0; j < N / 2; j++)
			{
				HL[i - N / 2][j] = data2d.data[i][j];
			}
		}
		for (size_t i = N / 2; i < N; i++)
		{
			for (size_t j = N / 2; j < N; j++)
			{
				HH[i - N / 2][j - N / 2] = data2d.data[i][j];
			}
		}
		response2D response;
		response.level = k;
		response.mapLL = LL;
		response.mapLH = LH;
		response.mapHL = HL;
		response.mapHH = HH;
		feature2D.push_back(response);
		// crop factor update
		base *= 2;
	}
}

void WaveletToolBox::Dwt2DBackward(int level, bool reversed, bool debug) 
{
	//init crop factor
	int base = 1;
	for (int i = 0; i < level - 1; i++) 
	{
		base *= 2;
	}
	vector<float> tmp;
	for (int k = level; k > 0; k--) 
	{
		int N = data2d.data.size() / base;
		//recover cols
		for (size_t i = 0; i < N; i++)
		{
			tmp.clear();
			for (size_t j = 0; j < data2d.data.size(); j++)
			{
				tmp.push_back(data2d.data[j][i]);
			}
			StepBackward(tmp, filters, k, reversed, debug);
			for (size_t j = 0; j < N; j++)
			{
				data2d.data[j][i] = tmp[j];
			}
		}
		//revover rows
		for (size_t i = 0; i < N; i++)
		{
			tmp = data2d.data[i];
			StepBackward(tmp, filters, k, reversed, debug);
			for (size_t j = 0; j < N; j++)
			{
				data2d.data[i][j] = tmp[j];
			}
		}
		// crop factor update
		base /= 2;
	}
	Crop_2D(data2d.data,data2d.S);
}

vector<response1D> WaveletToolBox::Dwt1D(vector<float> data, int level, string FilterType)
{
	feature1D.clear();
	AdjustData1D(data);
	SetupFilters(FilterType);
	Dwt1DForward(level, false, false);
	return feature1D;
}

vector<response2D> WaveletToolBox::Dwt2D(vector<vector<float>> data, int level, string FilterType) 
{
	feature2D.clear();
	AdjustData2D(data);
	cout << data2d.data.size() << " " << data2d.data[0].size() << endl;
	SetupFilters(FilterType);
	Dwt2DForward(level, false, false);
	return feature2D;
}











void WaveletToolBox::Test1D(void)
{
	vector<float> odata = {6,4,8,7,5,9,3,2,1};

	cout << "original data: " << endl;
	for (int i = 0; i < odata.size(); i++) 
	{
		cout << odata[i] << " ";
	}
	cout << "" << endl;

	AdjustData1D(odata);

	cout << "padded data: " << endl;
	for (int i = 0; i < data1d.data.size(); i++)
	{
		cout << data1d.data[i] << " ";
	}
	cout << "" << endl;
	SetupFilters("Haar");
	Dwt1DForward(2, false, true);
	cout << "after forward: " << endl;
	for (int i = 0; i < data1d.data.size(); i++)
	{
		cout << data1d.data[i] << " ";
	}
	cout << "" << endl;
	Dwt1DBackward(2, false, true);
	cout << "after backward: " << endl;
	for (int i = 0; i < data1d.data.size(); i++)
	{
		cout << data1d.data[i] << " ";
	}
	cout << "" << endl;

}


void WaveletToolBox::Test2D(void)
{
	vector<vector<float>> data(9, vector<float>(10, 0));
	for (int i = 0; i < 9; i++) 
	{
		for (int j = 0; j < 10; j++) 
		{
			data[i][j] = 10 * i + j + 1;
		}
	}
	cout << "original_data: " << endl;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			cout << data[i][j] << " ";
		}
		cout << "" << endl;
	}
	cout << "" << endl;
	AdjustData2D(data);
	cout << "padded_data: " << endl;
	for (int i = 0; i < data2d.data.size(); i++)
	{
		for (int j = 0; j < data2d.data[0].size(); j++)
		{
			cout << data2d.data[i][j] << " ";
		}
		cout << "" << endl;
	}
	cout << "" << endl;
	
	SetupFilters("Haar");

	Dwt2DForward(2, false, false);

	cout << "forward: " << endl;
	for (int i = 0; i < data2d.data.size(); i++)
	{
		for (int j = 0; j < data2d.data[0].size(); j++)
		{
			cout << data2d.data[i][j] << " ";
		}
		cout << "" << endl;
	}
	cout << "" << endl;

	cout << "recovered:" << endl;

	Dwt2DBackward(2, false, false);

	for (int i = 0; i < data2d.data.size(); i++)
	{
		for (int j = 0; j < data2d.data[0].size(); j++)
		{
			cout << data2d.data[i][j] << " ";
		}
		cout << "" << endl;
	}
	cout << "" << endl;

}


