#pragma once

#include "SFML/Graphics.hpp"
#include "WorkMode.h"

#include <vector>

using namespace sf;
using namespace std;

class SFMLButton //����� ������
{
public:
	//����������� ������
	SFMLButton(RectangleShape &shape, Vector2f position, string text, WorkMode mode, RenderWindow &window) 
	{
		Shape.setSize(shape.getSize());

		Shape.setPosition(position);

		Shape.setFillColor(shape.getFillColor());

		window.draw(Shape);

		sf::Text line;
		Font font;

		font.loadFromFile("Roboto-Light.ttf");
		line.setFont(font);

		line.setString(text);
		line.setFillColor(Color::Black);
		line.setOutlineColor(Color::White);
		line.setCharacterSize(20);

		line.setPosition(Vector2f(position.x, position.y));

		window.draw(line);

		Mode = mode;
	};

	//����������� ����� �������� ������ ���������
	static void SetUpButtons(float X, float startY, float YDyff, vector<SFMLButton>& AddTo, RenderWindow &window, 
		int activeButtonIndex)
	{
		int y = startY - YDyff;

		for (int i = 0; i < 6; i++)
		{
			string text;
			WorkMode mode;

			y += YDyff;

			switch (i)
			{
			case 0:
				text = "Create Vertex";
				mode = WorkMode::CreateVertex;
				break;
			case 1:
				text = "Detroy Vertex";
				mode = WorkMode::DestroyVertex;
				break;
			case 2:
				text = "Move Vertex";
				mode = WorkMode::MoveVertex;
				break;
			case 3:
				text = "Create Edge";
				mode = WorkMode::CreateEdge;
				break;
			case 4:
				text = "Detroy Edge";
				mode = WorkMode::DestroyEdge;
				break;
			case 5:
				text = "Solve";
				mode = WorkMode::MerchantProblem;
				break;
			default:
				break;
			}

			RectangleShape shape(Vector2f(180, 30));

			if (i == activeButtonIndex)
				shape.setFillColor(Color::Magenta);

			AddTo.push_back(SFMLButton(shape, Vector2f(X, y), text, mode, window));
		}
	}

	//����� ��� ��������� ������ ������
	RectangleShape& getShape()
	{
		return Shape;
	}

	//����� ��� ��������� ������ ������
	WorkMode GetWorkMode()
	{
		return Mode;
	}

private:

	WorkMode Mode; //����� ������ ������

	RectangleShape Shape; //������ ������
};
