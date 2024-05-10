#include "pch.h"
#include "Graph.h"
#include "SFMLButton.h"

using namespace System;
using namespace sf;
using namespace std;

int counter = 1;

int main(cli::array<System::String ^> ^args)
{
	setlocale(LC_ALL, "RUS");

	WorkMode mode = WorkMode::None;

	vector<SFMLButton> Buttons;

	RenderWindow window(VideoMode(800, 600), "MerchantProblem");

	Graph<string> mainGraph;

	vector<string> choosenElements;

	int chosenButtonIndex = -1;

	srand(time(0));

	while (window.isOpen())
	{
		window.clear(Color(255, 125, 24));
		Buttons.clear();

		RectangleShape visualCanvas(Vector2f(631, 651));
		visualCanvas.move(200, 0);
		visualCanvas.setFillColor(Color::Green);
		visualCanvas.setOutlineColor(Color::Black);

		RectangleShape functionalCanvas(Vector2f(631 - VertexRadius * 2, 651 - VertexRadius * 2));
		functionalCanvas.setPosition(200 + VertexRadius, 0 + VertexRadius);
		functionalCanvas.setFillColor(Color::Green);
		functionalCanvas.setOutlineColor(Color::Green);

		window.draw(visualCanvas);
		window.draw(functionalCanvas);

		SFMLButton::SetUpButtons(10, 20, 40, Buttons, window, chosenButtonIndex);

		Event ev;

		while (window.pollEvent(ev))
		{
			Vector2i mousePosition;

			switch (ev.type)
			{
			case Event::Closed:
				window.close();
				break;

			case Event::MouseButtonPressed:

				mousePosition = Mouse::getPosition(window);

				if (ev.key.code == Mouse::Left) 
				{
					if (mode == MoveVertex && choosenElements.size() == 0 ||
						((mode == CreateEdge || mode == DestroyEdge) && choosenElements.size() < 2))
						mainGraph.TrySetNewChoosen(mousePosition, choosenElements);

					int anotheButtonIndex = -1;

					for (int i = 0; i < 6 && anotheButtonIndex == -1; i++)
					{
						SFMLButton& button = Buttons[i];

						if (button.getShape().getGlobalBounds().contains(mousePosition.x, mousePosition.y))
							anotheButtonIndex = i;
					}

					if (anotheButtonIndex != -1)
					{
						chosenButtonIndex = anotheButtonIndex;
						mode = Buttons[chosenButtonIndex].GetWorkMode();
						choosenElements.clear();
					}
					else
					{
						string content;

						switch (mode)
						{
						case CreateVertex:

							if (functionalCanvas.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) 
							{
								content = "A" + to_string(counter++);
								mainGraph.AddVertex(content, mousePosition);
							}
							
							break;
						case DestroyVertex:

							mainGraph.TryRemoveVertexByCoordinates(mousePosition);
							break;

						case MoveVertex:

							if (choosenElements.size() != 0 && 
								functionalCanvas.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
								mainGraph.MoveVertex(choosenElements[0], mousePosition);

							break;

						case CreateEdge:

							if (choosenElements.size() == 2)
							{
								mainGraph.AddEdge(choosenElements[0], choosenElements[1], rand() % 100);
								mode = WorkMode::None;
								chosenButtonIndex = -1;
								choosenElements.clear();
							}

							break;
						case DestroyEdge:

							if (choosenElements.size() == 2)
							{
								mainGraph.RemoveEdge(choosenElements[0], choosenElements[1]);

								mode = WorkMode::None;
								chosenButtonIndex = -1;
								choosenElements.clear();
							}

							break;
						case MerchantProblem:
							break;
						case None:
							break;
						default:
							break;
						}
					}
				}
				break;

			default:
				break;
			}	
		}

		mainGraph.DrawGraph(window, choosenElements);

		window.display();
	}
}
