// HEXGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <complex> // if you make use of complex number facilities in C++
#include <string>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <iomanip>
#include <queue>
#include <stack>
#define INFINITY 65535

std::string hex[11][11]
{
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
	{ ".", ".", ".", ".", ".", ".", ".", ".", ".", ".", "." },
};
char a1;
int a;
int b;
std::string currentPlayer;
std::string SWAP;
bool red = true;
bool blue = false;
int counter;
std::stack<int> Xstack;
std::stack<int> Ystack;
char Color;
std::pair<int, int> aPair;
std::vector<std::pair<int, int>> Taken;

std::vector<std::string> startRed;
std::vector<std::string> stopRed;

std::vector<std::string> startBlue;
std::vector<std::string> stopBlue;

//alle buren die een coordinaat in het veld kan hebben.
std::vector<std::pair<int, int>> Neighbours(int b, int a)
{
	std::vector<std::pair<int, int>> neighbours;

	if (a == 0 && b == 0)
	{
		neighbours.push_back(std::make_pair(b + 1, a));
		neighbours.push_back(std::make_pair(b, a + 1));
	}
	else if (a == 10 && b == 10)
	{
		neighbours.push_back(std::make_pair(b - 1, a));
		neighbours.push_back(std::make_pair(b, a - 1));
	}
	else if (a == 0 && b == 10)
	{
		neighbours.push_back(std::make_pair(b - 1, a));
		neighbours.push_back(std::make_pair(b, a + 1));
		neighbours.push_back(std::make_pair(b - 1, a + 1));
	}
	else if (a == 10 && b == 0)
	{
		neighbours.push_back(std::make_pair(b + 1, a));
		neighbours.push_back(std::make_pair(b, a - 1));
		neighbours.push_back(std::make_pair(b + 1, a - 1));
	}
	else if (a == 0 && b > 0 && b < 10)
	{
		neighbours.push_back(std::make_pair(b - 1, a));
		neighbours.push_back(std::make_pair(b + 1, a));
		neighbours.push_back(std::make_pair(b - 1, a + 1));
		neighbours.push_back(std::make_pair(b, a + 1));
	}
	else if (a > 0 && a < 10 && b == 10)
	{
		neighbours.push_back(std::make_pair(b - 1, a));
		neighbours.push_back(std::make_pair(b, a - 1));
		neighbours.push_back(std::make_pair(b - 1, a + 1));
		neighbours.push_back(std::make_pair(b, a + 1));
	}
	else if (a == 10 && b > 0 && b < 10)
	{
		neighbours.push_back(std::make_pair(b - 1, a));
		neighbours.push_back(std::make_pair(b, a - 1));
		neighbours.push_back(std::make_pair(b + 1, a - 1));
		neighbours.push_back(std::make_pair(b + 1, a));
	}
	else if (a > 0 && a < 10 && b == 0)
	{
		neighbours.push_back(std::make_pair(b, a + 1));
		neighbours.push_back(std::make_pair(b, a - 1));
		neighbours.push_back(std::make_pair(b + 1, a - 1));
		neighbours.push_back(std::make_pair(b + 1, a));
	}
	else
	{
		neighbours.push_back(std::make_pair(b - 1, a));
		neighbours.push_back(std::make_pair(b - 1, a + 1));
		neighbours.push_back(std::make_pair(b, a - 1));
		neighbours.push_back(std::make_pair(b, a + 1));
		neighbours.push_back(std::make_pair(b + 1, a - 1));
		neighbours.push_back(std::make_pair(b + 1, a));
	}

	return neighbours;
}

//kijkt of een coordinaat al gevuld is
bool isfilled()
{
	for (int xx = 0; xx < 11; xx++)
	{
		for (int yy = 0; yy < 11; yy++)
		{
			if (hex[yy][xx] != ".")
				return true;
			else
				return false;
		}
	}
}

//kijkt of de methode solver al op deze positie geweest is
bool stepTaken(int x, int y, std::vector<std::pair<int, int>> &pathRed)
{
	for (int i = 0; i < pathRed.size(); i++)
	{
		int pathX = std::get<0>(pathRed[i]);
		int pathY = std::get<1>(pathRed[i]);

		if (pathX == x && pathY == y)
			return true;
	}
	return false;
}

//idem: stepTaken maar dan voor blauw.
bool stepTakenBlue(int x, int y, std::vector<std::pair<int, int>> &pathBlue)
{
	for (int i = 0; i < pathBlue.size(); i++)
	{
		int pathX = std::get<0>(pathBlue[i]);
		int pathY = std::get<1>(pathBlue[i]);

		if (pathX == x && pathY == y)
			return true;
	}
	return false;
}

//recursieve methode om te kijken of er een pad is opgebouwd in het bord.
void solver(int currentX, int currentY, std::vector<std::pair<int, int>> &pathRed)
{
	std::vector<std::pair<int, int>> Neighbour = Neighbours(currentY, currentX);
	for (int z = 0; z < Neighbour.size(); z++)
	{
		int neighbourY = std::get<0>(Neighbour[z]);
		int neighbourX = std::get<1>(Neighbour[z]);
		//std::cout << "YCOORD: " << neighbourY << "XCOORD: " << neighbourX << "\n";

		if (hex[neighbourX][neighbourY] == "R" && stepTaken(neighbourY, neighbourX, pathRed) == false)
		{
			std::cout << "YCOORD: " << neighbourY << "XCOORD: " << neighbourX << "\n";
			pathRed.push_back(std::make_pair(neighbourY, neighbourX));
			if (currentX == 0 && currentY >= 0 && currentY <= 11)
				std::cout << "RED WINS";
			else
				solver(neighbourX, neighbourY, pathRed);
		}
	}

}

//idem: solver, maar dan voor blauw
void solverBlue(int currentX, int currentY, std::vector<std::pair<int, int>> &pathBlue)
{
	std::vector<std::pair<int, int>> Neighbour = Neighbours(currentY, currentX);
	for (int z = 0; z < Neighbour.size(); z++)
	{
		int neighbourY = std::get<0>(Neighbour[z]);
		int neighbourX = std::get<1>(Neighbour[z]);
		//std::cout << "YCOORD: " << neighbourY << "XCOORD: " << neighbourX << "\n";

		if (hex[neighbourX][neighbourY] == "B" && stepTakenBlue(neighbourY, neighbourX, pathBlue) == false)
		{
			std::cout << "YCOORD: " << neighbourY << "XCOORD: " << neighbourX << "\n";
			pathBlue.push_back(std::make_pair(neighbourY, neighbourX));
			if (currentY == 1 && currentX >= 0 && currentX <= 11)
				std::cout << "BlUE WINS";
			else
				solverBlue(neighbourX, neighbourY, pathBlue);
		}
	}
}

//als de onderste rij gevuld is met een "R", dan begint hij met kijken naar een pad rood
void beginSolver()
{
	std::vector<std::pair<int, int>> pathRed;
	for (int x = 0; x < 11; x++)
	{
		startRed.push_back(hex[0][x]);
		stopRed.push_back(hex[10][x]);

		if (stopRed[x] == "R")
		{
			std::cout << "found" << x << std::endl;
			std::thread t1(solver, 10, x, pathRed);
			t1.join();
		}
	}
}

//als de rechter rij gevuld is met een "B", dan begint hij te kijken naar een pad voor blauw
void beginSolverBlue()
{
	std::vector<std::pair<int, int>> pathBlue;
	for (int x = 0; x < 11; x++)
	{
		startBlue.push_back(hex[x][0]);
		stopBlue.push_back(hex[x][10]);

		if (counter == 1)
			if (stopBlue[x] == "B")
			{
				std::cout << "found" << x << std::endl;
				std::thread t2(solverBlue, x, 10, pathBlue);
				t2.join();
			}
	}
}

//random AI voor blauw
void blueTurn()
{
	int v1 = rand() % 11;
	int v2 = rand() % 11 + 1;
	char X[11] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K' };

	std::cout << "Blauw is aan zet\n";

	v1 = rand() % 11;
	v2 = rand() % 11 + 1;
	if (hex[v2][v1] == ".")
	{
		a1 = X[v1];
		b = v2;
	}

	std::cout << "a= " << v1 + 1 << "  " << "b= " << v2 << "\n";
	currentPlayer = "B";
	red = true;
}

//random AI voor rood
void redTurn()
{
	std::cout << "Rood is aan zet\n";
	char X[11] = { 'A','B','C','D','E','F','G','H','I','J','K' };
	int v1 = rand() % 11;
	int v2 = rand() % 11 + 1;
	a1 = X[v1];
	b = v2;
	std::cout << "a= " << v1 + 1 << "  " << "b= " << v2 << "\n";
	currentPlayer = "R";
	blue = true;
}

//move wordt gezet
void setBoard(int b, int a)
{

	if (hex[b - 1][a - 1] == ".") {
		hex[b - 1][a - 1] = currentPlayer;
		Xstack.push(a - 1);
		Ystack.push(b - 1);
	}
	else {
		while (true)
		{
			a = rand() % 11;
			b = rand() % 11;
			if (hex[b - 1][a - 1] == ".")
			{
				hex[b - 1][a - 1] = currentPlayer;
				Xstack.push(a - 1);
				Ystack.push(b - 1);
				break;
			}
		}
	}
	hex[b - 1][a - 1] = currentPlayer;
	Xstack.push(a - 1);
	Ystack.push(b - 1);

	std::cout << "last move:" << Ystack.top() + 1 << Xstack.top() + 1 << "\n\n";
}
void turn(char c, std::string color) {
	std::cout << color << " is aan zet\n";
	std::cout << "Voer de gewenste coordinaten in\n";
	std::cin >> a1;
	if (a1 != 'P' && a1 != 'U')
		std::cin >> b;
	currentPlayer = c;
}
//bord wordt gemaakt
void GameBoard()
{
	int space = 0;
	counter = 0;

	std::cout << "Pick your color: R/B";
	std::cin >> Color;

	while (true)
	{
		if (Color == 'R')
		{
			if (red == true)
			{
				turn('R', "Rood");
				if (a1 != 'U')
					red = false;
			}
			else
			{
				if (counter == 1)
				{
					char Pie;
					std::cout << "Want to swap? Y/N";
					std::cin >> Pie;
					if (Pie == 'N')
					{
						std::cout << "Laatste move Red: " << a1 << b << "\n";
						blueTurn();
					}
					else
					{
						for (int i = 0; i < 11; i++)
						{
							for (int j = 0; j < 11; j++)
							{
								if (hex[j][i] == "R")
								{
									hex[j][i] = "B";
									turn('R', "Rood");
								}
							}
						}
					}
				}
				else if (red == false)
				{
					if (counter == 1)
					{
						turn('R', "Rood");
					}
					else
						blueTurn();
				}
			}
		}
		else
		{
			if (blue == false)
			{
				if (counter == 2)
					blue = true;
				else
					redTurn();
			}
			else
			{

				if (counter == 1)
				{
					char Pie;
					std::cout << "Want to swap? Y/N";
					std::cin >> Pie;
					if (Pie == 'N')
					{
						turn('B', "Blauw");
						counter--;
					}
					else
					{
						for (int i = 0; i < 11; i++)
						{
							for (int j = 0; j < 11; j++)
							{
								if (hex[j][i] == "R")
								{
									hex[j][i] = "B";
									redTurn();
								}
							}
						}
					}
				}
				else
					turn('B', "Blauw");
				blue = false;
			}
		}
		switch (a1)
		{
			if (hex[b][a] == ".")
			{
		case 'A':
			setBoard(b, 1);
			break;
		case 'B':
			setBoard(b, 2);
			break;
		case 'C':
			setBoard(b, 3);
			break;
		case 'D':
			setBoard(b, 4);
			break;
		case 'E':
			setBoard(b, 5);
			break;
		case 'F':
			setBoard(b, 6);
			break;
		case 'G':
			setBoard(b, 7);
			break;
		case 'H':
			setBoard(b, 8);
			break;
		case 'I':
			setBoard(b, 9);
			break;
		case 'J':
			setBoard(b, 10);
			break;
		case 'K':
			setBoard(b, 11);
			break;
		case 'U':
			hex[Ystack.top()][Xstack.top()] = ".";
			if (!Ystack.size() == 0 && !Xstack.size() == 0)
			{
				Ystack.pop();
				Xstack.pop();
			}
			if (Color == 'R')
				counter -= 2;

			else
			{
				if (counter == 1)
					counter--;
				else if (counter > 1)
					counter -= 2;
				blue = true;
			}

			break;
		default:
			break;
			}
			else
				std::cout << "Move already taken! Hava another go!";
		}

		std::cout << "    A    B    C    D    E    F    G    H    I    J    K \n";
		std::cout << "    1    2    3    4    5    6    7    8    9    10   11 ";
		std::cout << counter;

		for (int x = 0; x < 11; x++)
		{
			std::cout << "\n";
			std::cout << x + 1 << " ";
			for (int i = 0; i < space; i++)
			{
				std::cout << " ";
			}

			for (int y = 0; y < 11; y++)
			{
				if (x >= 9)
					std::cout << " " << hex[x][y] << "   ";
				else if (x <= 8)
					std::cout << "  " << hex[x][y] << "  ";
			}
			space += 2;
		}
		beginSolver();
		beginSolverBlue();
		std::cout << "\n";
		space = 0;
		std::cout << "\n";
		counter++;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	GameBoard();
	int x;
	std::cin >> x;
	return 0;
}


