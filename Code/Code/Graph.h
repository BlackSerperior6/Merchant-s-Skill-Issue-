#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>

#define PI 3.14159265

#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

const int VertexRadius = 20;

const int startMinValue = 10000;

struct ZeroElement
{
	int Row;
	int Column;
	int Value;

	ZeroElement() {};

	ZeroElement(int row, int column, int value)
	{
		Row = row;
		Column = column;
		Value = value;
	}

	void operator=(ZeroElement &other) 
	{
		Row = other.Row;
		Column = other.Column;
		Value = other.Value;
	}
};

template<typename T>
struct TurnBackPoint
{
	vector<vector<int>> Matrix;

	ZeroElement zero_element;

	map<T, T> RecordedPath;

	vector<pair<T, T>> Buffer;

	int Counter;

	TurnBackPoint() {}

	TurnBackPoint(vector<vector<int>> &matrix, ZeroElement &zeroElm, map<T, T> &recordedPath, int counter, vector<pair<T, T>>& buffer) 
	{
		Matrix = matrix;
		zero_element = zeroElm;
		RecordedPath = recordedPath;
		Counter = counter;
		Buffer = buffer;
	}
};

template<typename T>
struct GraphElement
{
	CircleShape visualVertex;
	
	T Data;
};

template <typename T>
class Graph
{
public:

	void DrawGraph(RenderWindow &window, vector<string> &choosenElements)
	{

		for (int i = 0; i < VertexList.size(); i++)
			DrawVertex(window, VertexList[i], choosenElements);

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
				DrawEdge(window, VertexList[i], VertexList[j], AdjMatrix[i][j]);	
		}
	}

	void TryRemoveVertexByCoordinates(Vector2i coords)
	{
		T item;
		bool flag = false;

		for (int i = 0; i < VertexList.size() && !flag; i++)
		{
			flag = VertexList[i].visualVertex.getGlobalBounds().contains(coords.x, coords.y);

			if (flag)
				item = VertexList[i].Data;
		}

		if (flag)
			RemoveVertex(item);
	}

	void AddVertex(T vertex, Vector2i position)
	{
		int oldSize = VertexList.size();

		GraphElement<T> elm;
		
		elm.Data = vertex;
		elm.visualVertex.setRadius(VertexRadius);
		elm.visualVertex.setPosition(position.x - VertexRadius, position.y - VertexRadius);
		elm.visualVertex.setOutlineColor(Color::Black);
		elm.visualVertex.setFillColor(Color::White);
		elm.visualVertex.setOutlineThickness(2);

		VertexList.push_back(elm);	
		
		int currentSize = VertexList.size();

		vector<vector<int>> Buffer = AdjMatrix;

		AdjMatrix.clear();

		AdjMatrix = vector<vector<int>>(currentSize, vector<int>(currentSize));

		for (int i = 0; i < oldSize; i++)
		{
			for (int j = 0; j < oldSize; j++)
				AdjMatrix[i][j] = Buffer[i][j];
		}
			
		for (int i = oldSize; i < currentSize; i++)
		{
			for (int j = oldSize; j < currentSize; j++)
				AdjMatrix[i][j] = 0;
		}
	}

	void TrySetNewChoosen(Vector2i coords, vector<T> &listChoosen) 
	{
		bool flag = false;

		T choosenData;

		for (int i = 0; i < VertexList.size() && !flag; i++) 
		{
			flag = VertexList[i].visualVertex.getGlobalBounds().contains(coords.x, coords.y);

			if (flag)
				choosenData = VertexList[i].Data;
		}
			

		if (!flag)
			return;

		flag = false;

		for (int i = 0; i < listChoosen.size() && !flag; i++)
			flag = listChoosen[i] == choosenData;

		if (flag)
			return;

		listChoosen.push_back(choosenData);
	}

	void MoveVertex(T data, Vector2i position) 
	{
		GraphElement<T>* element = nullptr;

		for (int i = 0; i < VertexList.size() && element == nullptr; i++)
		{
			if (VertexList[i].Data == data)
				element = &VertexList[i];
		}

		element->visualVertex.setPosition(position.x - VertexRadius, position.y - VertexRadius);
	}

	bool CanSolveMerchantProblem()
	{
		bool result = true;

		for (int i = 0; i < AdjMatrix.size() && result; i++)
		{
			for (int j = 0; j < AdjMatrix.size() && result; j++)
				result = i == j || (AdjMatrix[i][j] != 0 && AdjMatrix[i][j] != -1);
		}

		return result;
	}

	int GetVertexCount()
	{
		return VertexList.size();
	}

	map<T, T> SolveMerchantProblem()
	{
		vector<pair<T, T>> buffer;
		map<T, T> result;

		vector<vector<int>> BufferMatrix = vector<vector<int>>
			(AdjMatrix.size(), vector<int>(AdjMatrix.size()));

		//В задачу коммивояжера пути могут не существовать только на главной диагонали таблицы смежности
		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			for (int j = 0; j < AdjMatrix.size(); j++)
				BufferMatrix[i][j] = i == j ? -1 : AdjMatrix[i][j];
		}

		vector<TurnBackPoint<T>> TurnBackPoints;

		cout << "Starting table: " << endl << endl;

		PrintTable(BufferMatrix);

		for (int v = 0; v < VertexList.size(); v++)
		{
			cout << "----------------------" << endl << endl;

			PrintTable(BufferMatrix);

			vector<int> MinElementsOfRows;

			cout << endl <<"Row minimum elements:" << endl;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				int minValue = startMinValue;

				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j] >= 0 && minValue > BufferMatrix[i][j])
						minValue = BufferMatrix[i][j];
				}

				cout << i << " : " << minValue << endl;
				MinElementsOfRows.push_back(minValue);
			}

			cout << "Matrix after row reduction:" << endl << endl;;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j] > 0)
						BufferMatrix[i][j] -= MinElementsOfRows[i];
				}		
			}

			PrintTable(BufferMatrix);

			vector<int> MinElemntsOfColumns;

			cout << endl << endl << "Minimum column elements:" << endl;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				int minValue = startMinValue;

				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[j][i] >= 0 && minValue > BufferMatrix[j][i])
						minValue = BufferMatrix[j][i];
				}

				cout << i << " : " << minValue << endl;

				MinElemntsOfColumns.push_back(minValue);
			}

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)

					if (BufferMatrix[j][i] > 0)
						BufferMatrix[j][i] -= MinElemntsOfColumns[i];
			}

			cout << endl << "Matrix after second reduction: " << endl << endl;

			PrintTable(BufferMatrix);

			vector<ZeroElement> ZeroElements;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j] == 0)
					{
						int minRow = startMinValue;
						int minColumn = startMinValue;

						for (int uj = 0; uj < BufferMatrix.size(); uj++)
						{
							if (BufferMatrix[i][uj] >= 0 && j != uj)
							{
								if (minRow > BufferMatrix[i][uj])
									minRow = BufferMatrix[i][uj];
							}	
						}

						for (int ui = 0; ui < BufferMatrix.size(); ui++)
						{
							if (BufferMatrix[ui][j] >= 0 && ui != i)
							{
								if (minColumn > BufferMatrix[ui][j])
									minColumn = BufferMatrix[ui][j];
							}
						}

						ZeroElement element(i, j, minColumn + minRow);
						ZeroElements.push_back(element);
					}
				}
			}

			if (ZeroElements.size() == 0)
			{
				cout << "No more zero elements, finding obvious routes" << endl;

				for (int i = 0; i < VertexList.size(); i++)
				{
					bool flag = false;

					if (result.find(VertexList[i].Data) == result.end())
					{
						for (int j = 0; j < VertexList.size() && !flag; j++)
						{
							if (i == j)
								continue;

							bool does_exist = false;

							for (auto& elm : result)
							{
								if (elm.second == VertexList[j].Data)
								{
									does_exist = true;
									break;
								}
							}

							flag = !does_exist;

							if (flag)
								result[VertexList[i].Data] = VertexList[j].Data;
						}
					}
				}
			}
			else
			{
				int maxValue = -1;
				vector<ZeroElement> maxElements;

				cout << "Zero elements:" << endl;

				for (int i = 0; i < ZeroElements.size(); i++)
					cout << ZeroElements[i].Row << " " << ZeroElements[i].Column << " " << ZeroElements[i].Value << endl;

				for (int i = 0; i < ZeroElements.size(); i++)
				{
					if (maxValue < ZeroElements[i].Value)
					{
						maxElements.clear();

						maxElements.push_back(ZeroElements[i]);

						maxValue = ZeroElements[i].Value;
					}
					else if (maxValue == ZeroElements[i].Value)
						maxElements.push_back(ZeroElements[i]);
				}

				if (maxElements.size() > 1)
				{
					cout << "More than one max value zero elements found, creating a turn back point for each but not the first one!" << endl;

					for (int i = 1; i < maxElements.size(); i++)
						TurnBackPoints.push_back(TurnBackPoint<T>(BufferMatrix, maxElements[i], result, v, buffer));
				}

				ZeroElement maxElement = maxElements[0];

				pair<T, T> to_add;

				to_add.first = VertexList[maxElement.Row].Data;
				to_add.second = VertexList[maxElement.Column].Data;

				for (int i = 0; i < buffer.size(); i++)
				{
					if (to_add.first == buffer[i].second)
						BufferMatrix[GetVertexIndex(to_add.second)][GetVertexIndex(buffer[i].first)] = -1;
				}

				buffer.push_back(to_add);

				cout << "Result pair: " << to_add.first << " " << to_add.second << endl;

				result[VertexList[maxElement.Row].Data] = VertexList[maxElement.Column].Data;

				for (int i = 0; i < BufferMatrix.size(); i++)
					BufferMatrix[maxElement.Row][i] = -1;


				for (int i = 0; i < BufferMatrix.size(); i++)
					BufferMatrix[i][maxElement.Column] = -1;

				BufferMatrix[maxElement.Row][maxElement.Column] = -1;
				BufferMatrix[maxElement.Column][maxElement.Row] = -1;

				BufferMatrix[maxElement.Row][maxElement.Column] = -1;
				BufferMatrix[maxElement.Column][maxElement.Row] = -1;

				cout << "Matrix after full reduction: " << endl << endl;

				PrintTable(BufferMatrix);
			}

			if (v == VertexList.size() - 1)
			{
				if (PathIsBad(result))
				{
					cout << "Path is found but deemed bad, returning to one of the turn back points!" << endl << endl;

					TurnBackPoint<T>& point = TurnBackPoints[TurnBackPoints.size() - 1];

					BufferMatrix = point.Matrix;
					result = point.RecordedPath;
					buffer = point.Buffer;
					ZeroElement chosenElement = point.zero_element;
					v = point.Counter;

					pair<T, T> to_add2;

					to_add2.first = VertexList[chosenElement.Row].Data;
					to_add2.second = VertexList[chosenElement.Column].Data;

					for (int i = 0; i < buffer.size(); i++)
					{
						if (to_add2.first == buffer[i].second)
							BufferMatrix[GetVertexIndex(to_add2.second)][GetVertexIndex(buffer[i].first)] = -1;
					}

					buffer.push_back(to_add2);

					cout << "Result pair: " << to_add2.first << " " << to_add2.second << endl;

					result[VertexList[chosenElement.Row].Data] = VertexList[chosenElement.Column].Data;

					for (int i = 0; i < BufferMatrix.size(); i++)
						BufferMatrix[chosenElement.Row][i] = -1;


					for (int i = 0; i < BufferMatrix.size(); i++)
						BufferMatrix[i][chosenElement.Column] = -1;

					BufferMatrix[chosenElement.Row][chosenElement.Column] = -1;
					BufferMatrix[chosenElement.Column][chosenElement.Row] = -1;

					BufferMatrix[chosenElement.Row][chosenElement.Column] = -1;
					BufferMatrix[chosenElement.Column][chosenElement.Row] = -1;

					TurnBackPoints.pop_back();

					cout << "Matrix after full reduction: " << endl << endl;

					PrintTable(BufferMatrix);
				}
				else
					cout << "Path is found and deemed good!" << endl;
			}	
		}

		return result;
	}

	bool PathIsBad(map<T, T> &path) 
	{
		vector<T> metPoints;

		T current = VertexList[0].Data;

		bool result = false;

		for (int i = 0; i < VertexList.size() - 1 && !result; i++)
		{
			for (int j = 0; j < metPoints.size() && !result; j++)
				result = metPoints[j] == current;

			metPoints.push_back(current);
			current = path[current];
		}

		return result;
	}

	void AddEdge(T vertex1, T vertex2, int weight)
	{
		if (weight <= 0)
			return;

		int vertex1Index = GetVertexIndex(vertex1);
		int vertex2Index = GetVertexIndex(vertex2);

		AdjMatrix[vertex1Index][vertex2Index] = weight;

		if (AdjMatrix[vertex2Index][vertex1Index] == 0)
			AdjMatrix[vertex2Index][vertex1Index] = -1;
	}

	void RemoveEdge(T vertex1, T vertex2)
	{
		int vertex1Index = GetVertexIndex(vertex1);
		int vertex2Index = GetVertexIndex(vertex2);

		if (AdjMatrix[vertex2Index][vertex1Index] != -1)
			AdjMatrix[vertex1Index][vertex2Index] = -1;
		else
		{
			AdjMatrix[vertex1Index][vertex2Index] = 0;
			AdjMatrix[vertex2Index][vertex1Index] = 0;
		}		
	}

	int GetWeight(T vertex1, T vertex2)
	{
		int v1index = GetVertexIndex(vertex1);
		int v2index = GetVertexIndex(vertex2);

		return AdjMatrix[v1index][v2index];;
	}

	void Clear()
	{
		VertexList.clear();
		AdjMatrix.clear();
	}

private:

	vector<GraphElement<T>> VertexList;

	vector<vector<int>> AdjMatrix;

	void PrintTable(vector<vector<int>> to_print)
	{
		if (VertexList.size() == 0)
		{
			cout << "���� ����" << endl;
			return;
		}

		int greatestLenght = GetWidestLenght();

		for (int i = 0; i < greatestLenght; i++)
			cout << "_";

		cout << "|";

		for (int i = 0; i < VertexList.size(); i++)
		{
			PrintWithSpaces(VertexList[i].Data, greatestLenght);

			cout << "|";
		}

		PrintDivideLine(VertexList.size() * greatestLenght * 2);

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			PrintWithSpaces(VertexList[i].Data, greatestLenght);

			cout << "|";

			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				if (to_print[i][j] < 0)
					PrintWithSpaces("m", greatestLenght);
				else
					PrintWithSpaces(to_string(to_print[i][j]), greatestLenght);

				cout << "|";
			}

			PrintDivideLine(VertexList.size() * greatestLenght * 2);
		}
	}

	void RemoveVertex(T vertex)
	{
		int vIndex = GetVertexIndex(vertex);

		vector<GraphElement<T>> VertexBuffer;

		for (int i = 0; i < VertexList.size(); i++)
		{
			if (VertexList[i].Data != vertex)
				VertexBuffer.push_back(VertexList[i]);
		}

		int oldSize = VertexList.size();

		VertexList.clear();

		VertexList = VertexBuffer;

		int currentSize = VertexList.size();

		vector<vector<int>> EdgesBuffer = AdjMatrix;

		AdjMatrix.clear();

		AdjMatrix = vector<vector<int>>(currentSize, vector<int>(currentSize));

		int ColumnOffset = 0;

		for (int i = 0; i < currentSize; i++)
		{
			if (i == vIndex)
				ColumnOffset++;

			int RowOffset = 0;

			for (int j = 0; j < currentSize; j++)
			{
				if (j == vIndex)
					RowOffset++;

				AdjMatrix[i][j] = EdgesBuffer[i + ColumnOffset][j + RowOffset];
			}
		}
	}

	void DrawVertex(RenderWindow &window, GraphElement<T> &Vertex, vector<T> &choosenElements) 
	{
		sf::Text text;
		Font font;

		font.loadFromFile("CyrilicOld.TTF");

		//Настройка текста
		text.setString(Vertex.Data);
		text.setFont(font);
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);
		text.setCharacterSize(VertexRadius);

		//Отцентровка текста
		FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(Vertex.visualVertex.getPosition() + Vector2f(VertexRadius, VertexRadius));

		bool flag = false;

		for (int i = 0; i < choosenElements.size() && !flag; i++)
			flag = choosenElements[i] == Vertex.Data;

		Vertex.visualVertex.setOutlineColor(flag ? Color::Magenta : Color::Black);

		window.draw(Vertex.visualVertex);
		window.draw(text);
	}

	void DrawEdge(RenderWindow& window, GraphElement<T> Vertex1, GraphElement<T> Vertex2, int weight)
	{
		if (weight == 0 || weight == -1)
			return;

		Vector2f boundaryForVertex1 =
			CalculateBoundaryPoint(Vertex2.visualVertex.getPosition(), Vertex1.visualVertex.getPosition(), 22)
			+ Vector2f(VertexRadius, VertexRadius);

		Vector2f boundaryForVertex2 =
			CalculateBoundaryPoint(Vertex1.visualVertex.getPosition(), Vertex2.visualVertex.getPosition(), 22)
			+ Vector2f(VertexRadius, VertexRadius);

		VertexArray Edge(Lines, 2);

		Edge[0] = boundaryForVertex1;
		Edge[1] = boundaryForVertex2;

		Edge[0].color = Color::Black;
		Edge[1].color = Color::Black;

		Vector2f weightTextPosition = CalculateBoundaryPoint(boundaryForVertex1, boundaryForVertex2, 
			sqrt(pow(boundaryForVertex2.x - boundaryForVertex1.x, 2) + pow(boundaryForVertex2.y - boundaryForVertex1.y, 2)) 
			/ 4);

		Font font;

		font.loadFromFile("CyrilicOld.TTF");

		sf::Text text;
		text.setFont(font);
		text.setString(to_string(weight));
		text.setCharacterSize(15);
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);

		FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(weightTextPosition);

		VertexArray triangleArrow(Triangles, 3);

		double angle = GetAngle(boundaryForVertex2, boundaryForVertex1, Vector2f(boundaryForVertex2.x, boundaryForVertex2.y + 20));

		if (boundaryForVertex1.x > boundaryForVertex2.x)
			angle *= -1;

		Transform rotation;
		rotation.rotate(angle, boundaryForVertex2);

		Vector2f pointB = Vector2f(boundaryForVertex2.x - 10, boundaryForVertex2.y + 12);
		Vector2f pointC = Vector2f(boundaryForVertex2.x + 10, boundaryForVertex2.y + 12);

		triangleArrow[0] = boundaryForVertex2;
		triangleArrow[1] = rotation.transformPoint(pointB);
		triangleArrow[2] = rotation.transformPoint(pointC);

		triangleArrow[0].color = Color::Black;
		triangleArrow[1].color = Color::Black;
		triangleArrow[2].color = Color::Black;

		window.draw(text);
		window.draw(triangleArrow);
		window.draw(Edge);
	}

	double GetAngle(Vector2f &A, Vector2f &B, Vector2f &C)
	{
		//A - boundary2
		//B - boundary1
		//C - boundaty2, но сдвиг по y на 20 вниз

		double AB = GetSideLenght(A, B);

		double AC = GetSideLenght(A, C);

		double BC = GetSideLenght(B, C);

		return acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC)) * 180/PI;
	}

	double GetSideLenght(Vector2f &point1, Vector2f &point2)
	{
		return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
	}

	Vector2f CalculateBoundaryPoint(Vector2f point1, Vector2f point2, double minus)
	{
		double Lenght = GetEdgeLenght(point1, point2) - minus;

		double a = atan2(point2.y - point1.y, point2.x - point1.x);

		return Vector2f(point1.x + Lenght * cos(a), point1.y + Lenght * sin(a));
	}

	double GetEdgeLenght(Vector2f &point1, Vector2f &point2)
	{
		double DistanceX = point2.x - point1.x;
		double DistanceY = point2.y - point1.y;

		return sqrt(DistanceX * DistanceX + DistanceY * DistanceY);
	}

	int GetVertexIndex(T vertex)
	{
		int result = -1;

		for (int i = 0; i < VertexList.size() && result == -1; i++)
		{
			if (VertexList[i].Data == vertex)
				result = i;
		}

		return result;
	}

	int GetWidestLenght()
	{
		int result = 0;

		for (int i = 0; i < VertexList.size(); i++)
		{
			string element = VertexList[i].Data;

			if (result < element.length())
				result = element.length();
		}

		for (int i = 0; i < VertexList.size(); i++)
		{
			for (int j = 0; j < VertexList.size(); j++)
			{
				string element = to_string(AdjMatrix[i][j]);

				if (result < element.length())
					result = element.length();
			}
		}

		return result;
	}

	void PrintWithSpaces(string element, int maxLenght)
	{
		cout << element;

		for (int u = 0; u < maxLenght - element.length(); u++)
			cout << " ";
	}

	void PrintDivideLine(int Lenght)
	{
		cout << endl;

		for (int i = 0; i < Lenght * 2; i++)
			cout << "-";

		cout << endl;
	}
};

