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

struct  ElemntOfMatrix
{
	int value;
	bool isNonExistantFromStart = false;
	bool isNonExistantFromReduction = false;
};

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
};

template<typename T>
struct GraphElement
{
	CircleShape visualVertex;
	
	T Data;

	bool IsSelected = false;
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

	void PrintAsTablet() 
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
			PrintWithSpaces(to_string(VertexList[i].Data), greatestLenght);

			cout << "|";
		}

		PrintDivideLine(VertexList.size() * greatestLenght * 2);

		for (int i = 0; i < AdjMatrix.size(); i++)
		{
			PrintWithSpaces(to_string(VertexList[i].Data), greatestLenght);

			cout << "|";

			for (int j = 0; j < AdjMatrix.size(); j++)
			{
				PrintWithSpaces(to_string(AdjMatrix[i][j]), greatestLenght);
				cout << "|";
			}

			PrintDivideLine(VertexList.size() * greatestLenght * 2);
		}
	}

	void DepthRead(T startVertex)
	{
		if (GetVertexIndex(startVertex) == -1)
		{
			cout << "����� ������� ���" << endl;
			return;
		}

		bool* visitedVertexes = new bool[VertexList.size()];

		for (int i = 0; i < VertexList.size(); i++)
			visitedVertexes[i] = false;

		DepthHelper(startVertex, visitedVertexes);

		delete[] visitedVertexes;
	}

	void WidthRead(T startVertex)
	{
		if (GetVertexIndex(startVertex) == -1)
		{
			cout << "����� ������� ���" << endl;
			return;
		}

		bool* visitedVertexes = new bool[VertexList.size()];

		queue<T> queueOfVertexes;

		for (int i = 0; i < VertexList.size(); i++)
			visitedVertexes[i] = false;

		WidthHelper(startVertex, visitedVertexes, queueOfVertexes);

		delete[] visitedVertexes;
	}

	void AddVertex(T vertex, Vector2i position)
	{
		bool flag = false;

		for (int i = 0; i < VertexList.size() && !flag; i++)
			flag = VertexList[i].Data == vertex;
			
		if (flag)
		{
			cout << "����� ������� ��� ���� ���� ��� ���������� ������� � ������ ������������" << endl;
			return;
		}

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

	/*vector<pair<T, T>> SolverMerchantProblem(T startVertex)
	{
		vector<pair<T, T>> result;

		while (result.size() != VertexList.size())
		{
			vector<vector<ElemntOfMatrix>> BufferMatrix = vector<vector<ElemntOfMatrix>>
				(AdjMatrix.size(), vector<ElemntOfMatrix>(AdjMatrix.size()));

			for (int i = 0; i < AdjMatrix.size(); i++)
			{
				for (int j = 0; j < AdjMatrix.size(); j++)
				{
					BufferMatrix[i][j].value = AdjMatrix[i][j];

					if (BufferMatrix[i][j].value == 0 || BufferMatrix[i][j].value == -1)
						BufferMatrix[i][j].isNonExistantFromStart = true;
				}
			}

			vector<int> MinElementsOfRows;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				bool should_skip = false;

				for (int u = 0; u < result.size() && !should_skip; u++)
					should_skip = GetVertexIndex[result[u].first] == i;

				if (should_skip)
					continue;

				int minValue = BufferMatrix[i][0].value;

				bool flag = false;

				for (int j = 0; j < BufferMatrix.size() && !flag; j++)
				{
					flag = !BufferMatrix[i][j].isNonExistantFromStart;

					if (!flag)
						minValue = BufferMatrix[i][j + 1].value;
				}

				for (int j = 1; j < BufferMatrix.size(); j++)
				{
					if (!BufferMatrix[i][j].isNonExistantFromStart)
					{
						if (minValue > BufferMatrix[i][j].value)
							minValue = BufferMatrix[i][j].value;
					}
				}

				MinElementsOfRows[i] = minValue;
			}

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
					BufferMatrix[i][j].value -= MinElementsOfRows[i];
			}

			vector<int> MinElemntsOfColumns;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				bool should_skip = false;

				for (int u = 0; u < result.size() && !should_skip; u++)
					should_skip = GetVertexIndex[result[u].second] == i;

				if (should_skip)
					continue;

				int minValue = BufferMatrix[0][i].value;

				for (int j = 0; j < BufferMatrix.size() && !flag; j++)
				{
					flag = !BufferMatrix[j][i].isNonExistantFromStart;

					if (!flag)
						minValue = BufferMatrix[j + 1][i].value;
				}

				for (int j = 1; j < BufferMatrix.size(); j++)
				{
					if (!BufferMatrix[j][i].isNonExistantFromStart)
					{
						if (minValue > BufferMatrix[j][i])
							minValue = BufferMatrix[j][i].value;
					}	
				}

				MinElemntsOfColumns[i] = minValue;
			}

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
					BufferMatrix[j][i] -= MinElemntsOfColumns[i];
			}

			vector<ZeroElement> ZeroElements;

			for (int i = 0; i < BufferMatrix.size(); i++)
			{
				for (int j = 0; j < BufferMatrix.size(); j++)
				{
					if (BufferMatrix[i][j].value == 0 && !BufferMatrix[i][j].isNonExistantFromStart && !BufferMatrix[i][j].isNonExistantFromReduction)
					{
						int maxRow = BufferMatrix[i][j].value;
						int maxColum = BufferMatrix[i][j].value;

						for (int uj = j + 1; uj < BufferMatrix.size(); uj++)
						{
							if (!BufferMatrix[i][uj].isNonExistantFromStart)
							{
								if (maxRow < BufferMatrix[i][uj])
									maxRow = BufferMatrix[i][uj];
							}	
						}

						for (int ui = i + 1; ui < BufferMatrix.size(); ui++)
						{
							if (!BufferMatrix[ui][j].isNonExistantFromStart)
							{
								if (maxColum < BufferMatrix[ui][j])
									maxColum = BufferMatrix[ui][j];
							}
						}

						ZeroElement elemet(i, j, maxColum + maxRow);
						ZeroElements.push_back(elemet);
					}
				}
			}

			ZeroElement maxElement = ZeroElements[0];

			for (int i = 0; i < ZeroElements.size(); i++)
			{
				if (maxElement.Value < ZeroElements[i].Value)
					maxElement = ZeroElements[i];
			}

			result.push_back(pair<T, T>(VertexList[maxElement.Row], VertexList[maxElement.Column]));

			for (int i = 0; i < BufferMatrix.size(); i++)
				BufferMatrix[maxElement.Row][i].isNonExistantFromReduction = true;

			for (int i = 0; i < BufferMatrix.size(); i++)
				BufferMatrix[i][maxElement.Column].isNonExistantFromReduction = true;

			BufferMatrix[maxElement.Row][maxElement.Column].isNonExistantFromReduction = true;
			BufferMatrix[maxElement.Column][maxElement.Row].isNonExistantFromReduction = true;
		}

		return result;
	}*/

	void AddEdge(T vertex1, T vertex2, int weight)
	{
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

		if (v1index == -1 || v2index == -1)
		{
			cout << "������ ��� ����� ������ �� ����������" << endl;
			return -1;
		}

		int result = -1;

		if (AdjMatrix[v1index][v2index] == 0 || AdjMatrix[v1index][v2index] == -1)
		{
			if (AdjMatrix[v2index][v1index] != 0)
			{
				cout << "��� ������, ������ ����� �� ���������� � ������� ������ �������" << endl;
				result = AdjMatrix[v2index][v1index];
			}	
		}
		else
			result = AdjMatrix[v1index][v2index];

		return result;
	}

	void Clear()
	{
		VertexList.clear();
		AdjMatrix.clear();
	}

private:

	void RemoveVertex(T vertex)
	{
		int vIndex = GetVertexIndex(vertex);

		if (vIndex == -1)
		{
			cout << "����� ������� ���!" << endl;
			return;
		}

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

		Vector2f pointB = Vector2f(boundaryForVertex2.x - 10, boundaryForVertex2.y + 20);
		Vector2f pointC = Vector2f(boundaryForVertex2.x + 10, boundaryForVertex2.y + 20);

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

	vector<GraphElement<T>> VertexList;

	vector<vector<int>> AdjMatrix;

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
			string element = to_string(VertexList[i].Data);

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

	vector<T> GetNbr(T vertex)
	{
		vector<T> nbrs;

		int vindex = GetVertexIndex(vertex);

		if (vindex != -1)
		{
			for (int i = 0; i < VertexList.size(); i++)
			{
				if (AdjMatrix[vindex][i] != 0 && AdjMatrix[vindex][i] != -1)
					nbrs.push_back(VertexList[i].Data);
			}
		}

		return nbrs;
	}

	void DepthHelper(T vertex, bool* visitedVertexes)
	{
		cout << "�������: " << vertex << " ���� ����������!" << endl;

		visitedVertexes[GetVertexIndex(vertex)] = true;

		vector<T> neighbors = GetNbr(vertex);

		for (int i = 0; i < neighbors.size(); i++)
		{
			if (!visitedVertexes[GetVertexIndex(neighbors[i])])
				DepthHelper(neighbors[i], visitedVertexes);
		}
	}

	void WidthHelper(T vertex, bool* visitedVertexes, queue<T> &queueOfVertexes) 
	{
		int vertIndex = GetVertexIndex(vertex);

		if (!visitedVertexes[vertIndex])
		{
			queueOfVertexes.push(vertex);
			cout << "�������: " << vertex << " ���������� " << endl;
			visitedVertexes[vertIndex] = true;
		}

		vector<T> ngbr = GetNbr(vertex);

		queueOfVertexes.pop();

		for (int i = 0; i < ngbr.size(); i++)
		{
			int currentVertex = ngbr[i];

			if (!visitedVertexes[GetVertexIndex(currentVertex)])
			{
				queueOfVertexes.push(currentVertex);
				visitedVertexes[GetVertexIndex(currentVertex)] = true;
				cout << "�������" << currentVertex << " ���� ����������" << endl;
			}
		}

		if (!queueOfVertexes.empty())
			WidthHelper(queueOfVertexes.front(), visitedVertexes, queueOfVertexes);
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

