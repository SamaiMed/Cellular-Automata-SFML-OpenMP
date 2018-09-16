// ConsoleApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <omp.h>

#define NUM_THREADS 4

int main()
{
	const int w = 1366;
	const int h = 766;
	const float wf = (float)w;
	const float hf = (float)h;
	omp_set_num_threads(NUM_THREADS);

	sf::RenderWindow window(sf::VideoMode(w, h), "SFML works!");

	sf::Clock clock;
	sf::Time time;

	sf::RectangleShape player(sf::Vector2f(wf,hf));
	player.setPosition(0.f, 0.f);
	sf::Texture player_texture;
	sf::Image image;
	image.create(w, h);
	static int map[w][h];
	static int stats[w][h];

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			map[i][j] = 0;
			if (((float)rand() / (RAND_MAX+1.0)) < 0.75f) {
				image.setPixel(i, j, sf::Color::Blue);
				map[i][j] = 1;
				//std::cout << ((float)rand() / (RAND_MAX)) << "\n";
			}
		}
	}



	player_texture.loadFromImage(image);
	//player_texture.loadFromFile("texture/Ske.png");
	player.setTexture(&player_texture);

	while (window.isOpen())
	{
		time = clock.getElapsedTime();
		clock.restart().asSeconds();
		float fps = 1.0f / time.asSeconds();

		printf("FPS:: %f \r", fps);
		sf::Event event;
		
		while (window.pollEvent(event))
		{

			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				printf("Windows size : %d, %d \n", event.size.height, event.size.width);
				//window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				break;
			case sf::Event::TextEntered:
				if (event.text.unicode < 128) {
					printf("%c", event.text.unicode);
				}
				break;
			}


		}

#pragma omp parallel for schedule(static) 
		for (int i = 1; i < w-1; i++) {
			for (int j = 1; j < h-1; j++) {
				//printf("i = %d, j= %d, threadId = %d \n \r", i, j, omp_get_thread_num());
				int a = map[i - 1][j + 1] + map[i][j + 1] + map[i + 1][j + 1] +
					    map[i - 1][j]     +       0       + map[i + 1][j] +
					    map[i - 1][j - 1] + map[i][j - 1] + map[i + 1][j - 1];
				
				if (map[i][j]==1) {
					stats[i][j] = (int)((a == 2 || a == 3) ? 1 : 0);
				}
				else {
					stats[i][j] = (int)(a == 3 ? 1 : 0);
				}
			}
		}

#pragma omp parallel for schedule(static)
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				map[i][j] = stats[i][j];
				if (map[i][j] == 1) {
					image.setPixel(i, j, sf::Color::Blue);
				}
				else {
					image.setPixel(i, j, sf::Color::Black);
				}
			}
		}
		
		player_texture.loadFromImage(image);
		player.setTexture(&player_texture);
		window.clear();
		window.draw(player);
		window.display();
	}
	return 0;
}

