#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
 
using namespace std;

//Функция вычисления сигмоида
double sigmoid(double value)
{
	return 1/(1 + exp(-value));
}

//Класс, в который будем сохранять слои нейронов и их индексы.
template <typename T>
class Vector
{
public:
	Vector ()
	{
	}
	Vector (int size)
	{
		this->size = size;
		this->values = new T [size];
	}
	void SetValue (int id, T value)
	{
		this->values[id] = value;
	}
	T GetValue (int id)
	{
		return this->values[id];
	}
	int GetSize ()
	{
		return this->size;
	}
private:
	int size;
	T *values;
 
};

//Класс, в который будем сохранять все связи между соседними слоями нейронов
class Field
{
public:
	Field()
	{
	}
	Field (int n, int c)
	{
		this->n = n;
		this->c = c;
		this->rates = new double * [n];
		for (int i = 0; i < n; i++)
		{
			this->rates[i] = new double[c];
		}
	}
	Field (const Field &other)
	{
		this->n = other.n;
		this->c = other.c;
		this->rates = new double * [other.n];
		for (int i = 0; i < other.n; i++)
		{
			this->rates[i] = new double[other.c];
			for (int j = 0; j < other.c; j++)
			{
				this->rates[i][j] = other.rates[i][j];
			}
 
		}
	}
	Field &operator = (const Field &other)
	{
		this->n = other.n;
		this->c = other.c;
		if (this->rates != 0)
		{
			for (int i = 0; i < this->n; i++)
			{
				delete [] this->rates[i];
			}
			delete [] this->rates;
		}
		this->rates = new double * [other.n];
		for (int i = 0; i < other.n; i++)
		{
			this->rates[i] = new double[other.c];
			for (int j = 0; j < other.c; j++)
			{
				this->rates[i][j] = other.rates[i][j];
			}
 
		}
		return *this;
	};
	void Randomize ()
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < c; j++)
			{
				this->rates[i][j] = (double) (rand() % 100 - 50)/10;
			}
		}
	}
	void SetValue (int n, int c, double value)
	{
		this->rates[n][c] = value;
	}
	double GetValue(int n, int c)
	{
		return this->rates[n][c];
	}
	void ViewTenValues ()
	{
		int size = 10;
		if (size > this->c)
		{
			size = this->c;
		}
		cout << "Rates: ";
		for (int j = 0; j < size; j++) {
			cout << rates[0][j] << " ";
		}
		cout << endl;
	}
private:
	int n;
	int c;
	double **rates;
};

//Функция пересчета нейронов класса, путем перемножения нейнорон на веса их связей и вычисления сигмоида
void Calculate (Vector <double> &input, Vector <double> &output, Field &f)
{
	double sum = 0;
	for (int c = 0; c < output.GetSize(); c++)
	{
		sum = 0;
		for (int n = 0; n < input.GetSize(); n++)
		{
			sum += input.GetValue(n) * f.GetValue(n,c);
		}
		output.SetValue(c, sigmoid(sum));
	}
}

//Функция для добавления строки в двумерный динамический массив
void addRow (double ** &arr, int &rows, int &cols, double * &varr)
{
	double **newArray = new double *[rows + 1];
	for (int i = 0; i < rows; i++)
	{
		newArray[i] = new double [cols];
		for (int j = 0; j < cols; j++)
		{
			newArray[i][j] = arr[i][j];
		}
	}
	newArray[rows] = new double [cols];
	for (int i = 0; i < cols; i++)
	{
		newArray[rows][i] = varr[i];
	}
	for (int i = 0; i < rows; i++)
	{
		delete [] arr[i];
	}
	rows++;
	arr = newArray;
}

//Функция добавления нового значения в конец динамического массива
void addValue (double * &arr, int &size, double value)
{
	double *newArray = new double [size+1];
	for (int i=0; i<size; i++)
	{
		newArray[i] = arr[i];
	}
	newArray[size] = value;
	size++;
	delete [] arr;
	arr = newArray;
}

//Функция перемешивания значений вектора, хранящего индексы нейронов
void Shuffle(Vector <int> &v)
{
	srand(time(0));
	int a, b;
	int valueA, valueB;
	int size = v.GetSize();
 
	for (int i = 0; i < size; i++)
	{
		a = rand()%size;
		b = rand()%size;
		valueA = v.GetValue(a);
		valueB = v.GetValue(b);
		v.SetValue(a, valueB);
		v.SetValue(b, valueA);
	}
}
 
int main() {
	
	double temp;
	ifstream fin;
	stringstream sstm;
	string fileName;
	string tmpString;
 
	int numLayers; //Количество слоев
	cout << "Enter amount of layers" << endl << "Minimum 3 layers (input, hidden, output)" << endl << "More layers create more hidden layers." << endl;
	cin >> numLayers;
	cout << endl;
 
	int *NeuronsInLayer = new int [numLayers]; //Количество нейронов в слое
	for (int i = 0; i < numLayers; i++) {
		cout << "Enter amount of neurons for layer " << (i+1) << endl;
		cin >> NeuronsInLayer[i];
	}
 
	cout << "NeuroNet: ";
	for (int i = 0; i < numLayers; i++)
	{
		cout << NeuronsInLayer[i] << " > ";
	}
	cout << endl;
  
	//Количество полей содержащих все связи между соседними слоями нейронов
	int numFields = numLayers-1;
	
	//Количество связей в каждом поле
	int *ConnectionsBtwnLayers = new int [numFields];
	for (int i = 0; i < numFields; i++)
	{
		ConnectionsBtwnLayers[i] = NeuronsInLayer[i] * NeuronsInLayer[i+1];
	}
 
	cout << "Connections: ";
	for (int i = 0; i < numFields; i++)
	{
		cout << ConnectionsBtwnLayers[i] << " > ";
	}
	cout << endl << endl;
 
	//Создаем векторы содержащие индексы нейронов каждого слоя (понадобитлся для перемешивания)
	Vector <int> fids[numFields];
	for (int i = 0; i < numFields; i++)
	{
		fids[i] = Vector <int> (ConnectionsBtwnLayers[i]);
		for (int j = 0; j < (ConnectionsBtwnLayers[i]); j++)
		{
			fids[i].SetValue(j, j);
		}
	}
 
	//Создаем векторы нейронов для каждого слоя
	Vector <double> v[numLayers];
	for (int i = 0; i < numLayers; i++)
	{
		v[i] = Vector <double> (NeuronsInLayer[i]);
	}
 
	//CСоздаем поля коэффициентов (понадобятся для вычислений)
	Field f[numFields];
	for (int i = 0; i < numFields; i++)
	{
		//Пробуем прочитать коэффициенты из файла
		sstm.str().resize(0);
		f[i] = Field(NeuronsInLayer[i],NeuronsInLayer[i+1]);
		sstm << "field" << i << ".txt";
		fileName = sstm.str();
		fin.open(fileName.c_str());
		if (!fin.is_open())
		{
			//Если файл не найден, создаем коэффициенты случайным образом
			f[i].Randomize();
		}
		else
		{
			while (!fin.eof())
			{
				cout << "Load Rates for field " << i << endl;
				for (int n = 0; n < NeuronsInLayer[i]; n++)
				{
					for (int c = 0; c < NeuronsInLayer[i+1]; c++)
					{
						fin >> temp;
						f[i].SetValue(n, c, temp);
					}
				}
			}
		}
 		//Показываем первые 10 элементов (для информации)
		f[i].ViewTenValues();
		fin.close();
	}
	cout << endl;
  
	//Читаем из файла данные, содержащие значения индикаторов за периоды
	double *tmpArr = new double [(NeuronsInLayer[0])];
	int inputSize = 0;
	double **inputArr = new double * [1];
 
	cout << "Enter filename of Input Vector with rows of " << NeuronsInLayer[0] << " values:" << endl;
	cin >> fileName;
 
	fin.open((fileName + ".txt").c_str());
 
	if (!fin.is_open())
	{
		cout << "Error Open file " + fileName + ".txt" << endl;
	}
	else
	{
		while (!fin.eof())
		{
			sstm.str().resize(0);
			getline(fin,tmpString);
			sstm << tmpString;
			int i = 0;
			while (sstm.good() && i < NeuronsInLayer[0]){
				sstm >> tmpArr[i++];
			}
			addRow (inputArr, inputSize, NeuronsInLayer[0], tmpArr);
		}
	}
	fin.close();
 
	cout << endl;
	cout << "INPUTS:" << endl;
	for (int i = 0; i < inputSize; i++)
	{
		for (int j = 0; j < NeuronsInLayer[0]; j++)
		{
			cout << " " << inputArr[i][j];
		}
		cout << endl;
	}
	cout << endl;
 
	//Читаем из файла цены за периоды
	double *pricesArr = new double [1];
	int pricesSize = 0;
 
	cout << "Enter filename of prices:" << endl;
	cin >> fileName;
 
	fin.open((fileName + ".txt").c_str());
 
	if (!fin.is_open())
	{
		cout << "Error Open file " + fileName + ".txt" << endl;
	}
	else
	{
		while (!fin.eof())
		{
			fin >> temp;
			addValue (pricesArr, pricesSize, temp);
		}
	}
	fin.close();
 
	cout << endl;
	cout << "PRICES:" << endl;
	for (int i = 0; i < pricesSize; i++)
	{
		cout << pricesArr[i] << endl;
	}
	cout << endl;
 
	int maxEpoch;
	cout << "Enter maximum amount of epochs:" << endl;
	cin >> maxEpoch;
	cout << endl;
  
	cout << "TRAINING" << endl;
	//ОБУЧЕНИЕ
 
	double startBalanceQuote, currentBalanceQuote, currentBalanceBase, prevBalanceSum, currentBalanceSum, amountBet;
	int epoch = 0, steps, prevSteps, times = 1, numBuys, numSells;
 
	Field modf[numFields];
	//Копируем поля
	for (int i = 0; i < numFields; i++)
	{
		modf[i] = f[i];
	}
	//Перемешиваем
	for (int i = 0; i < numFields; i++){
		Shuffle(fids[i]);
	}
	int n, c;
 
	while (epoch < maxEpoch)
	{
		startBalanceQuote = 10000.0;
		currentBalanceQuote = 5500.0;
		currentBalanceBase = 1.0;
		prevBalanceSum = 0.0;
		currentBalanceSum = 0.0;
		amountBet = 10.0;
 
		numBuys = 0;
		numSells = 0;
 
		prevSteps = steps;
		steps = 0;
		if (times % 3 == 1)
		{
			for (int i = 0; i < numFields; i++)
			{
				for (int j = 0; j < NeuronsInLayer[i+1]; j++)
				{
					n = fids[i].GetValue(j) / NeuronsInLayer[i+1];
					c = fids[i].GetValue(j) % NeuronsInLayer[i+1];
					temp = modf[i].GetValue(n,c);
					temp += 0.1;
					modf[i].SetValue(n,c,temp);
				}
			}
		}
		if (times % 3 == 2)
		{
			for (int i = 0; i < numFields; i++)
			{
				for (int j = 0; j < NeuronsInLayer[i+1]; j++)
				{
					n = fids[i].GetValue(j) / NeuronsInLayer[i+1];
					c = fids[i].GetValue(j) % NeuronsInLayer[i+1];
					temp = modf[i].GetValue(n,c);
					temp -= 0.1;
					modf[i].SetValue(n,c,temp);
				}
			}
		}
		if (times % 3 == 0)
		{
			for (int i = 0; i < numFields; i++){
				Shuffle(fids[i]);
			}
		}
 
		for (int i = 0; i < inputSize; i++)
		{
			for (int j = 0; j < NeuronsInLayer[0]; j++)
			{
				//Вставляем входные данные в первый слой
				v[0].SetValue(j, inputArr[i][j]);
			}
			for (int j = 0, k = 1; j < numFields; j++, k++)
			{
				//Пересчитываем значения нейронов для остальных слоев
				Calculate (v[j], v[k], f[j]);
			}
			//Проверяем значение последнего нейрона последнего слоя
			if (v[numLayers-1].GetValue(0) > 0.8)
			{
				if (currentBalanceQuote >= amountBet) {
					// ПОКУПКА
					currentBalanceQuote -= amountBet;
					currentBalanceBase += amountBet * 0.999 / pricesArr[i];
					numBuys++;
				}
			}
			if (v[numLayers-1].GetValue(0) < 0.2)
			{
				if (currentBalanceBase >= amountBet / pricesArr[i])
				{
					// ПРОДАЖА
					currentBalanceBase -= amountBet / pricesArr[i];
					currentBalanceQuote += amountBet * 0.999;
					numSells++;
				}
			}	
			currentBalanceSum = currentBalanceQuote + currentBalanceBase * pricesArr[i];
			steps++;
		}
 
		if (prevSteps <= steps || prevBalanceSum <= currentBalanceSum)
		{
			for (int i = 0; i < numFields; i++)
			{
				f[i] = modf[i];
			}
			epoch++;
			times = 0;
		} else {
			for (int i = 0; i < numFields; i++)
			{
				modf[i] = f[i];
			}
			times++;
		}
 
		if (!(epoch%100)) {
			//Каждые 100 эпох делаем дамп коэффициентов в файлы, и выводим инфу
			cout << "Epochs: " << epoch << "\tSteps: " << steps << "/" << inputSize << endl;
			cout << "Start Balance: " << startBalanceQuote << "Current Balance: " << currentBalanceSum << endl << endl;
			cout << "Buys: " << numBuys << "\t\tSells: " << numSells << endl;
 
			string fileName;
			for (int i = 0; i < numFields; i++)
			{
				sstm.str().resize(0);
				sstm << "field" << i << ".txt";
				fileName = sstm.str();
 
				ofstream fout;
				fout.open(fileName.c_str());
 
				if (!fout.is_open())
				{
					cout << "Error Open file " + fileName + ".txt" << endl;
				}
				else
				{
					for (int j = 0; j < NeuronsInLayer[i]; j++)
					{
						for (int k = 0; k < NeuronsInLayer[i+1];k++)
						{
							fout << f[i].GetValue(j,k);
							if (k < (NeuronsInLayer[i+1] - 1)) {
								fout  << ' ';
							}
						}
						if (j < (NeuronsInLayer[i]-1)) {
							fout << '\n';
						}
					}
				}
 
				fout.close();
			}
		}
 
	}
 
	delete [] NeuronsInLayer;
	delete [] ConnectionsBtwnLayers;
	return 0;
}
 
