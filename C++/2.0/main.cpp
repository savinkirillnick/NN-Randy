#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <sstream>

// v 2.0
// RUS

using namespace std;

//Функция вычисления сигмоида
double sigmoid(const double &value)
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
		this->size = 0;
	}
	Vector (const int size)
	{
		this->size = size;
		this->values = new T [size];
		for (int i = 0; i < size; i++)
		{
			this->values[i] = 0;
		}
	};
	Vector (const Vector &other)
	{
		this->size = other.size;
		this->values = new T [other.size];
		for (int i = 0; i < other.size; i++)
		{
			this->values[i] = other.values[i];
		}
	}
	Vector &operator = (const Vector &other)
	{
		if (this->size > 0)
		{
			delete [] this->values;
		}
		this->size = other.size;
		this->values = new T [other.size];
		for (int i = 0; i < other.size; i++)
		{
			this->values[i] = other.values[i];
		}
		return *this;
	};
	~Vector ()
	{
		if (this->size > 0)
		{
			delete [] this->values;
		}
	}
	void SetValue (const int &id, const T &value)
	{
		this->values[id] = value;
	}
	T GetValue (const int id)
	{
		return this->values[id];
	}
	int GetSize ()
	{
		return this->size;
	}
	void Shuffle ()
	{
		int a, b;
		T tmp;
		for (int i = 0; i < this->size; i++)
		{
			a = rand() % this->size;
			b = rand() % this->size;
			tmp = values[a];
			values[a] = values[b];
			values[b] = tmp;
		}
	}
private:
	int size;
	T *values;
 
};

//Функция пересчета нейронов класса, путем перемножения нейнорон на веса их связей и вычисления сигмоида
void Calculate (Vector <double> &input, Vector <double> &output, Vector <double> &f)
{
	double sum;
	for (int c = 0, i = 0; c < output.GetSize(); c++)
	{
		sum = 0;
		for (int n = 0; n < input.GetSize(); n++, i++)
		{
			sum += input.GetValue(n) * f.GetValue(i);
		}
		output.SetValue(c, sigmoid(sum));
	}
}

//Функция для добавления строки в двумерный динамический массив
void AddRow (double ** &arr, int &rows, int &cols, double * varr)
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
	delete [] arr;
	rows++;
	arr = newArray;
}

//Функция добавления нового значения в конец динамического массива
void AddValue (double * &arr, int &size, double value)
{
	double *newArray = new double [size+1];
	for (int i=0; i < size; i++)
	{
		newArray[i] = arr[i];
	}
	newArray[size] = value;
	size++;
	delete [] arr;
	arr = newArray;
}

int main ()
{
	setlocale(LC_ALL, "rus");
	srand(time(0));
	
	double temp;
	ifstream fin;
	string fileName;
	string tmpString;
	
	int numLayers; //Количество слоев
	cout << "Введите количество слоев: " << endl << "Минимум 3 слоя (входной, скрытый, выходной)" << endl << "Большое количество слоев означает больше скрытых слоев." << endl;
	cin >> numLayers;
	cout << endl;
	
	int *NeuronsInLayer = new int [numLayers]; //Количество нейронов в слое
	for (int i = 0; i < numLayers; i++) {
		cout << "Введите количество нейронов на " << (i+1) << " слое: ";
		cin >> NeuronsInLayer[i];
	}
	
	cout << "НейроСеть: ";
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
	
	cout << "Связи между слоями: ";
	for (int i = 0; i < numFields; i++)
	{
		cout << ConnectionsBtwnLayers[i] << " > ";
	}
	cout << endl << endl;

	//Создаем векторы нейронов для каждого слоя
	Vector <double> v[numLayers];
	for (int i = 0; i < numLayers; i++)
	{
		v[i] = Vector <double> (NeuronsInLayer[i]);
	}

	//Создаем векторы коэффициентов и индексов нейронов каждого слоя (понадобитлся для перемешивания)
	Vector <double> f[numFields];
	Vector <int> fids[numFields];
	
	for (int i = 0; i < numFields; i++)
	{
		f[i] = Vector <double> (ConnectionsBtwnLayers[i]);
		fids[i] = Vector <int> (ConnectionsBtwnLayers[i]);
		
		//Заполняем веторы коэффициентов порядковыми номерами
		for (int j = 0; j < ConnectionsBtwnLayers[i]; j++)
		{
			fids[i].SetValue(j, j);
		}
		
		//Пробуем прочитать коэффициенты из файла
		stringstream sstm;
		sstm << "field" << i << ".txt";
		fileName = sstm.str();
		fin.open(fileName.c_str());
		if (!fin.is_open())
		{
			//Если файл не найден, создаем коэффициенты случайным образом
			for (int j = 0; j < ConnectionsBtwnLayers[i]; j++)
			{
				f[i].SetValue(j, (double) (rand() % 100 - 50)/100);
			}
		}
		else
		{
			while (!fin.eof())
			{
				for (int j = 0; j < ConnectionsBtwnLayers[i]; j++)
				{
					fin >> temp;
					f[i].SetValue(j, temp);
				}
			}	
		}
		
 		//Показываем первые 10 элементов (для информации)
		int k = 10;
		if (ConnectionsBtwnLayers[i] < k)
		{
			k = ConnectionsBtwnLayers[i];
		}
		
		cout << "коэффициенты слоя " << i << ": " <<endl;
		for (int j = 0; j < k; j ++)
		{
			cout << f[i].GetValue(j) << " ";
		}
		cout << endl;
		
		fin.close();
		
	}
	
	//Читаем из файла данные, содержащие значения индикаторов за периоды
	int inputSize = 0;
	double **inputArr = new double * [1];

	cout << "Введите название файла с вводными содержащими по " << NeuronsInLayer[0] << " значений:" << endl;
	cin >> fileName;
 
	fin.open((fileName + ".txt").c_str());
	
	if (!fin.is_open())
	{
		cout << "Ошибка открытия файла " + fileName + ".txt" << endl;
	}
	else
	{
		while (!fin.eof())
		{
			double tmpArr[NeuronsInLayer[0]];
			for (int i = 0; i < NeuronsInLayer[0]; i++)
			{
				fin >> tmpArr[i];
			}
			AddRow (inputArr, inputSize, NeuronsInLayer[0], tmpArr);
		}
	}
	fin.close();
	cout << endl;
	
	//Показываем первые 10 строк значений из загруженых данных
	cout << "ВВОДНЫЕ: " << endl;
	for (int i = 0; i < inputSize && i < 10; i++)
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
 
	cout << "Введите название файла с ценами: " << endl;
	cin >> fileName;
 
	fin.open((fileName + ".txt").c_str());
	
	if (!fin.is_open())
	{
		cout << "Ошибка открытия файла " + fileName + ".txt" << endl;
	}
	else
	{
		while (!fin.eof())
		{
			fin >> temp;
			AddValue (pricesArr, pricesSize, temp);
		}
	}
	fin.close();
	cout << endl;
	
	//Показываем первые 10 строк значений из загруженых данных
	cout << "ЦЕНЫ:" << endl;
	for (int i = 0; i < pricesSize && i < 10; i++)
	{
		cout << pricesArr[i] << endl;
	}
	cout << endl;
	
	cout << "ОБУЧЕНИЕ" << endl;
	//ОБУЧЕНИЕ
 
 
	int maxEpoch;
	cout << "Введите максимальное количество эпох: ";
	cin >> maxEpoch;
 
	StartTraining:
	
	double startBalanceQuote, currentBalanceQuote, currentBalanceBase;
	double prevBalanceSum = 0, currentBalanceSum = 0, amountBet = 10.0;
	int epoch = 0, numBuys, numSells;

	double learningStep;
	cout << "Введите шаг обучения: ";
	cin >> learningStep;

	Vector <double> modf[numFields];
	//Копируем поля
	for (int i = 0; i < numFields; i++)
	{
		modf[i] = f[i];
	}
	
	//Перемешиваем
	for (int i = 0; i < numFields; i++){
		fids[i].Shuffle();
	}
	
	int n = 0;
	
	while (epoch < maxEpoch)
	{
		startBalanceQuote = 10000.0;
		currentBalanceQuote = 5700.0;
		currentBalanceBase = 1;
 
		numBuys = 0;
		numSells = 0;

		if ((epoch % 2) == 0)
		{
			//Уменьшаем коэффициенты на шаг обучения
			for (int i = 0; i < numFields; i++)
			{
				for (int j = 0; j < NeuronsInLayer[i+1]; j++)
				{
					n = fids[i].GetValue(j);
					temp = modf[i].GetValue(n) - learningStep;
					modf[i].SetValue(n,temp);
				}
			}
		}
		if ((epoch % 2) == 1)
		{
			//Увеличиваем коэффициенты на шаг обучения
			for (int i = 0; i < numFields; i++)
			{
				for (int j = 0; j < NeuronsInLayer[i+1]; j++)
				{
					n = fids[i].GetValue(j);
					temp = modf[i].GetValue(n) + learningStep;
					modf[i].SetValue(n,temp);
				}
			}
		}
		
		//Прогоняем сеть с текущими коэфициентами по всем загруженным данным
		for (int i = 0; i < (inputSize-1); i++)
		{
			for (int j = 0; j < NeuronsInLayer[0]; j++)
			{
				//Вставляем входные данные в первый слой
				v[0].SetValue(j, inputArr[i][j]);
			}
			for (int j = 0; j < numFields; j++)
			{
				//Пересчитываем значения нейронов для остальных слоев
				Calculate (v[j], v[j+1], modf[j]);
			}
			//Проверяем значение последнего нейрона последнего слоя
			if (v[numLayers-1].GetValue(0) < 0.2)
			{
				if (currentBalanceQuote >= amountBet) {
					// ПОКУПКА
					currentBalanceQuote -= amountBet;
					currentBalanceBase += amountBet * 0.999 / pricesArr[i];
					numBuys++;
				}
			}
			if (v[numLayers-1].GetValue(1) > 0.8)
			{
				if (currentBalanceBase >= (amountBet / pricesArr[i]))
				{
					// ПРОДАЖА
					currentBalanceBase -= (double) amountBet / pricesArr[i];
					currentBalanceQuote += amountBet * 0.999;
					numSells++;
				}
			}	
			currentBalanceSum = currentBalanceQuote + currentBalanceBase * pricesArr[i];
		}
		
		//Сравниваем текущий баланс и предыдущий
		if (currentBalanceSum > prevBalanceSum)
		{
			//Если текущий баланс превышает предыдущий, применяем коэффициенты
			prevBalanceSum = currentBalanceSum;
			for (int i = 0; i < numFields; i++)
			{
				f[i] = modf[i];
			}
		}
		else
		{
			//Если текущий баланс не превышает предыдущий, восстанавливаем коэффициенты
			for (int i = 0; i < numFields; i++)
			{
				modf[i] = f[i];
			}
		}

		for (int i = 0; i < numFields; i++){
			fids[i].Shuffle();
		}

		epoch++;
		
		//Каждую эпоху будем рандомизировать по 1 коэффициенту в слое
		for (int i = 0; i < numFields; i++)
		{
			n = fids[i].GetValue(0);
			double tmp = (double) (rand() % 100 - 50)/100;
			modf[i].SetValue(n,tmp);
		}

		if (!(epoch%100)) {
			//Каждые 100 эпох делаем дамп коэффициентов в файлы, и выводим инфу
			cout << "Эпохи: " << epoch << "\tПокупки: " << numBuys << "\tПродажи: " << numSells << "\tШаг:" << learningStep << endl;
			cout << "Начальный баланс: " << startBalanceQuote << "\tТекущий баланс: " << currentBalanceSum << endl << endl;
 
			for (int i = 0; i < numFields; i++)
			{
				stringstream sstm;
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
					for (int j = 0; j < ConnectionsBtwnLayers[i]; j++)
					{
						fout << f[i].GetValue(j);
						
						if (j < (ConnectionsBtwnLayers[i]-1)) {
							fout << ' ';
						}
					}
				}
				fout.close();
			}
		}
	}
 
	cout << "Введите количество эпох чтобы продолжить обучение: ";
	cin >> maxEpoch;
	if (maxEpoch > 0)
	{
		goto StartTraining;
	}
 
	delete [] NeuronsInLayer;
	delete [] ConnectionsBtwnLayers;

	return 0;
}
