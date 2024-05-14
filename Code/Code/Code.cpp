#include "pch.h"
#include "Graph.h"
#include "SFMLButton.h"

using namespace System;
using namespace sf;
using namespace std;

int counter = 1;

void CenterText(sf::Text &text, RectangleShape &shape)
{
	FloatRect bounds(text.getLocalBounds());
	Vector2f box(shape.getSize());

	text.setOrigin((bounds.width - box.x) / 2 + bounds.left, (bounds.height - box.y) / 2 + bounds.top);
}

int GetValueFromDialog()
{
	RenderWindow dialogWindow(VideoMode(800, 135), "Enter Data");

	string currentText = "";

	Font font;
	font.loadFromFile("CyrilicOld.TTF");

	while (dialogWindow.isOpen())
	{
		dialogWindow.clear(Color(255, 125, 24));

		RectangleShape TextBox(Vector2f(521, 31));

		TextBox.setPosition(120, 10);

		TextBox.setOutlineColor(Color::Black);

		sf::Text text;

		text.setString(currentText.length() == 0 ? "Введите длину пути" : currentText + "_");
		text.setFillColor(Color::Black);
		text.setOutlineColor(Color::White);
		text.setCharacterSize(15);
		text.setPosition(TextBox.getPosition());
		text.setFont(font);

		RectangleShape ExitButton(Vector2f(241, 41));

		ExitButton.setPosition(250, 60);
		ExitButton.setOutlineColor(Color::Black);

		sf::Text OkText;

		OkText.setString("Ok");
		OkText.setFillColor(Color::Black);
		OkText.setOutlineColor(Color::White);
		OkText.setCharacterSize(20);
		OkText.setPosition(ExitButton.getPosition());
		OkText.setFont(font);

		CenterText(OkText, ExitButton);
		CenterText(text, TextBox);

		dialogWindow.draw(TextBox);
		dialogWindow.draw(text);
		dialogWindow.draw(ExitButton);
		dialogWindow.draw(OkText);

		Uint32 uniText;

		Event ev;
		char chr;

		while (dialogWindow.pollEvent(ev))
		{
			switch (ev.type)
			{
			case Event::Closed:
				dialogWindow.close();
				currentText = "";
				break;

			case Event::MouseButtonPressed:

				if (ev.key.code == Mouse::Left)
				{
					Vector2i mousePosition = Mouse::getPosition(dialogWindow);

					if (ExitButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
						dialogWindow.close();
				}

				break;


			case Event::TextEntered:

				uniText = ev.text.unicode;

				if (uniText == 0x8 && currentText.length() != 0)
					currentText.erase(currentText.size() - 1);
				else if (currentText.length() < 2)
				{
					chr = static_cast<wchar_t>(uniText);

					if (!isdigit(chr))
						break;

					currentText += static_cast<wchar_t>(ev.text.unicode);
				}

				break;

			default:
				break;
			}
		}

		dialogWindow.display();
	}

	if (currentText == "")
		return -1;

	return stoi(currentText);
}

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
					if ((mode == MoveVertex  || mode == MerchantProblem) && choosenElements.size() == 0 ||
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
								content = "Г" + to_string(counter++);
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
								mainGraph.AddEdge(choosenElements[0], choosenElements[1], GetValueFromDialog());
								choosenElements.clear();
							}

							break;
						case DestroyEdge:

							if (choosenElements.size() == 2)
							{
								mainGraph.RemoveEdge(choosenElements[0], choosenElements[1]);
								choosenElements.clear();
							}

							break;
						case MerchantProblem:

							if (choosenElements.size() == 0)
								break;

							if (!mainGraph.CanSolveMerchantProblem())
							{
								sf::Text errMessage;

								errMessage.setString("Невозможно решить задачу при таких условиях!");
								errMessage.setFillColor(Color::Black);
								errMessage.setOutlineColor(Color::White);

								Font font;
								font.loadFromFile("CyrilicOld.TTF");

								errMessage.setFont(font);
								errMessage.setPosition(50, 30);
								errMessage.setCharacterSize(30);

								RenderWindow ErrorWindow(VideoMode(1051, 158), "Error!");

								while (ErrorWindow.isOpen())
								{
									ErrorWindow.clear(Color(255, 125, 24));

									Event ev;

									while (ErrorWindow.pollEvent(ev))
									{
										if (ev.type == Event::Closed)
											ErrorWindow.close();
									}

									ErrorWindow.draw(errMessage);

									ErrorWindow.display();
								}	
							}
							else
							{
								map<string, string> solution = mainGraph.SolveMerchantProblem();

								int totalDistance = 0;
								string current = choosenElements[0];

								string result = current;

								for (int i = 0; i < mainGraph.GetVertexCount(); i++)
								{
									result += " -> " + solution[current];

									totalDistance += mainGraph.GetWeight(current, solution[current]);

									current = solution[current];
								}

								sf::Text visualText;

								visualText.setFillColor(Color::Black);
								visualText.setOutlineColor(Color::White);

								Font font;
								font.loadFromFile("CyrilicOld.TTF");

								visualText.setFont(font);
								visualText.setPosition(50, 30);
								visualText.setCharacterSize(30);

								RenderWindow ResultWindow(VideoMode(result.length() * 30, 158), "Result!");

								visualText.setString(result + "\n\n" + "Длина пути: " + to_string(totalDistance));
								
								cout << totalDistance << endl;

								while (ResultWindow.isOpen())
								{
									ResultWindow.clear(Color(255, 125, 24));

									Event ev;

									while (ResultWindow.pollEvent(ev))
									{
										if (ev.type == Event::Closed)
											ResultWindow.close();
									}

									ResultWindow.draw(visualText);

									ResultWindow.display();
								}
							}

							choosenElements.clear();

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
