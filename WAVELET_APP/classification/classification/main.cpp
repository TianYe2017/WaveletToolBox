#include "wavelet.h"
#include "opencv.hpp"
#include "highgui.hpp"


using namespace std;


struct cell 
{
	int label;
	vector<float> feature;
};

vector<cell> trainData;
vector<cell> testData;
cv::Mat Mean;


void ComputeMean(bool blur, int N, int low, int high) 
{	
	cout << "computing mean... " << endl;
	vector<cv::Mat> cluster;
	for (int s = 1; s <= N; s++)
	{
		for (int i = low; i <= high; i++)
		{
			string path = "./att_faces/s";
			path += to_string(s);
			path += "/";
			path += to_string(i);
			path += ".pgm";
			cv::Mat tmp = cv::imread(path);
			cv::cvtColor(tmp, tmp, CV_RGB2GRAY);
			if (blur)
			{
				cv::blur(tmp, tmp, cv::Size(3, 3));
			}
			tmp.convertTo(tmp, CV_32F);
			for (int row = 0; row < tmp.rows; row++)
			{
				for (int col = 0; col < tmp.cols; col++)
				{
					tmp.at<float>(row, col) /= 255.0f;
				}
			}
			cluster.push_back(tmp);
		}
	}
	Mean = cv::Mat(cv::Size(cluster[0].cols,cluster[0].rows),CV_32F);
	for (int i = 0; i < cluster.size(); i++) 
	{
		for (int row = 0; row < cluster[0].rows; row++) 
		{
			for (int col = 0; col < cluster[0].cols; col++) 
			{
				Mean.at<float>(row, col) += cluster[i].at<float>(row, col);
			}
		}
	}
	for (int row = 0; row < cluster[0].rows; row++)
	{
		for (int col = 0; col < cluster[0].cols; col++)
		{
			Mean.at<float>(row, col) /= (float)(cluster[0].rows * cluster[0].cols);
		}
	}
}


vector<vector<float>> ProcessSingleImage(string path, bool blur)
{
	cv::Mat img = cv::imread(path);
	cv::cvtColor(img, img, CV_RGB2GRAY);
	if (blur)
	{
		cv::blur(img, img, cv::Size(3, 3));
	}
	img.convertTo(img, CV_32F);
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			img.at<float>(row, col) /= 255.0f;
		}
	}
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			img.at<float>(row, col) -= Mean.at<float>(row,col);
		}
	}
	vector<vector<float>> cluster;
	for (int row = 0; row < img.rows; row++)
	{
		vector<float> tmp;
		for (int col = 0; col < img.cols; col++)
		{
			tmp.push_back(img.at<float>(row, col));
		}
		cluster.push_back(tmp);
	}
	cout << "path: " << path << endl;
	cout << "rows: " << img.rows << " " << "cols: " << img.cols << " " << "SizeOfCluster: " << cluster.size() << " " << cluster[0].size() << endl;
	cout << "" << endl;
	return cluster;
}



vector<float> ProcessSingleImageFlatten(string path, bool blur)
{
	cv::Mat img = cv::imread(path);
	cv::cvtColor(img, img, CV_RGB2GRAY);
	if (blur)
	{
		cv::blur(img, img, cv::Size(3, 3));
	}
	img.convertTo(img, CV_32F);
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			img.at<float>(row, col) /= 255.0f;
		}
	}
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			img.at<float>(row, col) -= Mean.at<float>(row, col);
		}
	}
	vector<float> cluster;
	for (int row = 0; row < img.rows; row++)
	{	
		for (int col = 0; col < img.cols; col++)
		{
			cluster.push_back(img.at<float>(row,col));
		}	
	}
	cout << "path: " << path << endl;
	cout << "rows: " << img.rows << " " << "cols: " << img.cols << " " << "SizeOfCluster: " << cluster.size() << endl;
	cout << "" << endl;
	return cluster;
}


float ComputeEnergy(vector<vector<float>> response) 
{
	float sum = 0.0f;
	for (int i = 0; i < response.size(); i++) 
	{
		for (int j = 0; j < response[0].size(); j++) 
		{
			sum += response[i][j] * response[i][j];
		}
	}
	//sum = sqrt(sum / (float)(response.size() * response[0].size()));
	sum = sqrt(sum);
	return sum;
}

float ComputeEnergyFlatten(vector<float> response)
{
	float sum = 0.0f;
	for (int i = 0; i < response.size(); i++)
	{	
		sum += response[i] * response[i];	
	}
	//sum = sqrt(sum / (float)(response.size()));
	sum = sqrt(sum);
	return sum;
}


vector<float> ComputeFeatureVector(vector<response2D> raw) 
{
	vector<float> ans;
	for (int i = 0; i < raw.size(); i++) 
	{
		ans.push_back(ComputeEnergy(raw[i].mapHH));
		ans.push_back(ComputeEnergy(raw[i].mapHL));
		ans.push_back(ComputeEnergy(raw[i].mapLH));
		//ans.push_back(ComputeEnergy(raw[i].mapLL)); //?
	}
	return ans;
}

vector<float> ComputeFeatureVectorFlatten(vector<response1D> raw)
{
	vector<float> ans;
	for (int i = 0; i < raw.size(); i++)
	{
		ans.push_back(ComputeEnergyFlatten(raw[i].highpassed));
		//ans.push_back(ComputeEnergyFlatten(raw[i].lowpassed));
	}
	return ans;
}

void CreateTrainingCells(WaveletToolBox* wavelet, bool blur, int N, int low, int high, int level, string FilterType) 
{
	for (int s = 1; s <= N; s++) 
	{
		for (int i = low; i <= high; i++)
		{
			string path = "./att_faces/s";
			path += to_string(s);
			path += "/";
			path += to_string(i);
			path += ".pgm";
			vector<vector<float>> data = ProcessSingleImage(path,blur);
			vector<response2D> response = wavelet->Dwt2D(data,level,FilterType);
			cell c;
			c.label = s;
			c.feature = ComputeFeatureVector(response);
			trainData.push_back(c);
		}
	}
}

void CreateTestingCells(WaveletToolBox* wavelet, bool blur, int N, int low, int high, int level, string FilterType)
{
	for (int s = 1; s <= N; s++)
	{
		for (int i = low; i <= high; i++)
		{
			string path = "./att_faces/s";
			path += to_string(s);
			path += "/";
			path += to_string(i);
			path += ".pgm";
			vector<vector<float>> data = ProcessSingleImage(path, blur);
			vector<response2D> response = wavelet->Dwt2D(data, level, FilterType);
			cell c;
			c.label = s;
			c.feature = ComputeFeatureVector(response);
			testData.push_back(c);
		}
	}
}

void CreateTrainingCellsFlatten(WaveletToolBox* wavelet, bool blur, int N, int low, int high, int level, string FilterType)
{
	for (int s = 1; s <= N; s++)
	{
		for (int i = low; i <= high; i++)
		{
			string path = "./att_faces/s";
			path += to_string(s);
			path += "/";
			path += to_string(i);
			path += ".pgm";
			vector<float> data = ProcessSingleImageFlatten(path, blur);
			vector<response1D> response = wavelet->Dwt1D(data, level, FilterType);
			cell c;
			c.label = s;
			c.feature = ComputeFeatureVectorFlatten(response);
			trainData.push_back(c);
		}
	}
}

void CreateTestingCellsFlatten(WaveletToolBox* wavelet, bool blur, int N, int low, int high, int level, string FilterType)
{
	for (int s = 1; s <= N; s++)
	{
		for (int i = low; i <= high; i++)
		{
			string path = "./att_faces/s";
			path += to_string(s);
			path += "/";
			path += to_string(i);
			path += ".pgm";
			vector<float> data = ProcessSingleImageFlatten(path, blur);
			vector<response1D> response = wavelet->Dwt1D(data, level, FilterType);
			cell c;
			c.label = s;
			c.feature = ComputeFeatureVectorFlatten(response);
			testData.push_back(c);
		}
	}
}

float L1Distance(vector<float> a, vector<float> b)
{
	float sum = 0.0f;
	for (int i = 0; i < a.size(); i++)
	{
		sum += abs(a[i] - b[i]);
	}
	return sum;
}

float L2Distance(vector<float> a, vector<float> b)
{
	float sum = 0.0f;
	for (int i = 0; i < a.size(); i++)
	{
		sum += (a[i] - b[i]) * (a[i] - b[i]);
	}
	sum = sqrt(sum);
	return sum;
}

void GatherTestAccuracy(vector<cell> ref, vector<cell> unknown, int flag)
{
	float accuracy;
	float numOfPos = 0.0f;
	for (int i = 0; i < unknown.size(); i++)
	{
		int real_label = unknown[i].label;
		int label = ref[0].label;
		float minDis;
		float dis;
		if (flag == 0)
		{
			minDis = L2Distance(ref[0].feature, unknown[i].feature);
		}
		else
		{
			minDis = L1Distance(ref[0].feature, unknown[i].feature);
		}
		for (int j = 1; j < ref.size(); j++)
		{
			if (flag == 0)
			{
				dis = L2Distance(ref[j].feature, unknown[i].feature);
			}
			else
			{
				dis = L1Distance(ref[j].feature, unknown[i].feature);
			}
			if (dis < minDis)
			{
				minDis = dis;
				label = ref[j].label;
			}
		}
		cout << "real label: " << real_label << "  " << "predicted label: " << label << "  " << "distance: " << minDis << endl;
		if (real_label == label)
		{
			numOfPos += 1.0f;
		}
	}
	accuracy = numOfPos / ((float)unknown.size());
	cout << "Total number of test cases:(test) " << unknown.size() << endl;
	cout << "# correct: " << (int)numOfPos << "  " << "test accuracy: " << accuracy << endl;
}

void GatherTrainAccuracy(vector<cell> ref, vector<cell> unknown, int flag)
{
	float accuracy;
	float numOfPos = 0.0f;
	for (int i = 0; i < unknown.size(); i++)
	{
		int real_label = unknown[i].label;
		int label = ref[0].label;
		float minDis;
		float dis;
		if (flag == 0)
		{
			minDis = L2Distance(ref[0].feature, unknown[i].feature);
		}
		else
		{
			minDis = L1Distance(ref[0].feature, unknown[i].feature);
		}
		for (int j = 1; j < ref.size(); j++)
		{
			if (flag == 0)
			{
				dis = L2Distance(ref[j].feature, unknown[i].feature);
			}
			else
			{
				dis = L1Distance(ref[j].feature, unknown[i].feature);
			}
			if (dis < minDis)
			{
				minDis = dis;
				label = ref[j].label;
			}
		}
		cout << "real label: " << real_label << "  " << "predicted label: " << label << "  " << "distance: " << minDis << endl;
		if (real_label == label)
		{
			numOfPos += 1.0f;
		}
	}
	accuracy = numOfPos / ((float)unknown.size());
	cout << "Total number of test cases:(train) " << unknown.size() << endl;
	cout << "# correct: " << (int)numOfPos << "  " << "train accuracy: " << accuracy << endl;
}



void main(void) 
{
	
	WaveletToolBox *wavelet = new WaveletToolBox();
	ComputeMean(false, 40, 1, 7);
	CreateTrainingCells(wavelet, false, 40, 1, 7, 6, "Haar");
	CreateTestingCells(wavelet, false, 40, 8, 10, 6, "Haar");
	GatherTrainAccuracy(trainData, trainData, 0);
	GatherTestAccuracy(trainData, testData, 0);

	/*WaveletToolBox *wavelet = new WaveletToolBox();
	ComputeMean(false, 40, 1, 7);
	CreateTrainingCellsFlatten(wavelet, false, 40, 1, 7, 13, "Haar");
	CreateTestingCellsFlatten(wavelet, false, 40, 8, 10, 13, "Haar");
	GatherTrainAccuracy(trainData, trainData, 0);
	GatherTestAccuracy(trainData, testData, 0);*/



	while (1) 
	{
		cv::waitKey(20);
	}

}