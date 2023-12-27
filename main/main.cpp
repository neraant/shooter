#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include "map.h"
#include "view.h"
#include "GameOverScreen.h"
#include "Entity.h"

using namespace sf;
using namespace std;

// Кол-во убитый врагов
int deathN = 0;

class Player {

};

class Enemy :public Entity {

};

class Bullet : public Entity {

};

// Текстура врага
Texture enemyTexture;

int main()
{
	srand(time(NULL));

	// Window
	RenderWindow window(VideoMode(1440, 960), "Zombie house");
	view.reset(FloatRect(0, 0, 640, 480));

	GameOverScreen gameOverScreen;

	// Текст
	Font font;//шрифт 
	font.loadFromFile("fonts/ArialBold.ttf");//передаем нашему шрифту файл шрифта
	Text healthEntity("", font, 20);//создаем объект текст. закидываем в объект текст строку, шрифт, размер шрифта(в пикселях);//сам объект текст (не строка)
	Text deathEntity("", font, 20);

	// Создаем вектор для хранения пуль
	vector<Bullet> bullets;

	// Создаем вектор для хранения врагов
	vector<Enemy> enemies;
	if (!enemyTexture.loadFromFile("images/enemy.png")) {
		cout << "Could not load enemy image" << endl;
		return -1;
	}

	// Map
	Image map_image;
	map_image.loadFromFile("images/map.png");
	Texture map;
	map.loadFromImage(map_image);
	Sprite s_map;
	s_map.setTexture(map);

	// Bullet
	Texture bulletTexture;
	if (!bulletTexture.loadFromFile("images/bullet.png")) {
		cout << "Could not load bullet image" << endl;
		return -1;
	}

	// Moving player
	bool isMove = false;
	float dX = 0;
	float dY = 0;

	Player player("player.png", 300, 300, 66, 38);

	// frames + time
	float CurrentFrame = 0;
	Clock clock;
	Clock gameTimeClock;//переменная игрового времени, будем здесь хранить время игры 
	int gameTime = 0;//объявили игровое время, инициализировали.

	// Время последнего спавна врага
	float lastSpawnTime = 0;

	// Время последнего урона для каждого врага
	vector<Clock> lastDamageTime(enemies.size());

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();

		if (player.life)
			gameTime = gameTimeClock.getElapsedTime().asSeconds();

		clock.restart();
		time = time / 1000;

		Vector2i pixelPos = Mouse::getPosition(window);
		Vector2f pos = window.mapPixelToCoords(pixelPos);

		// Спавн врага каждые 5 секунд
		if (gameTime - lastSpawnTime >= 5 && enemies.size() < 30)
		{
			// Генерируем случайные координаты для нового врага
			int randomX = rand() % window.getSize().x;
			int randomY = rand() % window.getSize().y;

			// Создаем нового врага с новой текстурой
			enemies.emplace_back(enemyTexture, randomX, randomY, 32, 32, "Enemy");

			// Добавляем новый таймер в lastDamageTime
			lastDamageTime.emplace_back();

			// Обновляем время последнего спавна
			lastSpawnTime = gameTime;
		}

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				float dx = pos.x - player.x;
				float dy = pos.y - player.y;
				float length = sqrt(dx * dx + dy * dy);
				dx /= length;
				dy /= length;

				bullets.push_back(Bullet(bulletTexture, "Bullet", player.x, player.y, 16, 16, dx, dy));
			}

		}

		// Обновление пуль
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].update(time);

			// Если пуля выходит за пределы карты, удаляем ее
			if (bullets[i].x < 0 || bullets[i].y < 0 || bullets[i].x > window.getSize().x || bullets[i].y > window.getSize().y) {
				bullets.erase(bullets.begin() + i);
				i--; // Уменьшаем индекс, чтобы не пропустить следующую пулю
			}

			// Проверка столкновения пули с врагом
			for (int i = 0; i < bullets.size(); i++) {
				for (int j = 0; j < enemies.size(); j++) {
					if (bullets[i].getRect().intersects(enemies[j].getRect())) {
						enemies[j].health -= 30; // Уменьшаем здоровье врага на 40
						if (enemies[j].health <= 0) { // Если здоровье врага <= 0, помечаем его как "мертвого"
							deathN += 15;
							enemies[j].life = false;
						}
						bullets.erase(bullets.begin() + i); // Удаляем пулю после столкновения
						break;
					}
				}
			}

			// Удаляем всех "мертвых" врагов и добавляем нового врага для каждого убитого
			for (int i = 0; i < enemies.size(); i++) {
				if (!enemies[i].life) {
					enemies.erase(enemies.begin() + i);
					i--; // Уменьшаем индекс, чтобы не пропустить следующего врага

					// Генерируем случайные координаты для нового врага
					int randomX = rand() % window.getSize().x;
					int randomY = rand() % window.getSize().y;

					// Добавляем нового врага в вектор с случайными координатами
					enemies.emplace_back(enemyTexture, randomX, randomY, 32, 32, "Enemy");

				}
			}
		}
		for (int i = 0; i < enemies.size(); i++) {
			enemies[i].update(time, player.x, player.y);

			// Проверяем столкновение врага с игроком и прошло ли 2 секунды с последнего урона
			if (enemies[i].getRect().intersects(player.getRect()) && lastDamageTime[i].getElapsedTime().asSeconds() >= 2) {
				// Уменьшаем здоровье игрока
				player.health -= 15; // Уменьшаем здоровье игрока на 40

				// Сбрасываем таймер урона
				lastDamageTime[i].restart();

				// Проверяем, жив ли еще игрок
				if (player.health <= 0) {
					player.life = false;
				}
			}
		}

		// Обработка столкновений между врагами
		for (int i = 0; i < enemies.size(); i++) {
			for (int j = i + 1; j < enemies.size(); j++) {
				if (enemies[i].getRect().intersects(enemies[j].getRect())) {
					// Обрабатываем столкновение между enemies[i] и enemies[j]
					// Например, меняем направление движения врагов
					float temp = enemies[i].dx;
					enemies[i].dx = enemies[j].dx;
					enemies[j].dx = temp;

					temp = enemies[i].dy;
					enemies[i].dy = enemies[j].dy;
					enemies[j].dy = temp;
				}
			}
		}

		// Движение игрока
		if (player.life) {
			player.update(time);
			if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
				player.dir = 1; player.speed = 0.12;
			}
			if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
				player.dir = 0; player.speed = 0.12;
			}
			if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
				player.dir = 3; player.speed = 0.12;
			}
			if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) {
				player.dir = 2; player.speed = 0.12;
			}
		
			GetPlayerCoordinateForView(player.GetPlayerCoordinateX(), player.GetPlayerCoordinateY());

			// Поворот камеры за курсором
			if (Mouse::isButtonPressed(Mouse::Left)) { player.sprite.setTextureRect(IntRect(66, 0, 80, 38)); }
			float dX = pos.x - player.x;
			float dY = pos.y - player.y;
			float rotation = (atan2(dY, dX)) * 180 / 3.14159265;
			player.sprite.setRotation(rotation);
			player.update(time);
		}

		for (int i = 0; i < enemies.size(); i++) {
			enemies[i].update(time, player.x, player.y);
		}

		player.interactionWithMap();
		window.setView(view);
		window.clear();

		for (int i = 0; i < HEIGHT_MAP; i++) {
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));//добавить побольше вариативности карты
				if (TileMap[i][j] == 'f')  s_map.setTextureRect(IntRect(96, 0, 32, 32));
				if (TileMap[i][j] == 'h')  s_map.setTextureRect(IntRect(128, 0, 32, 32));

				s_map.setPosition(j * 32, i * 32);

				window.draw(s_map);
			}
		}

		window.draw(player.sprite);
		player.sprite.setTextureRect(IntRect(0, 0, 66, 38));

		for (const auto& bullet : bullets) {
			window.draw(bullet.sprite);
		}
		for (int i = 0; i < enemies.size(); i++) {
			window.draw(enemies[i].sprite);
		}

		ostringstream playerHealth;    // объявили переменную
		playerHealth << player.health;		//занесли в нее число очков, то есть формируем строку
		healthEntity.setString("HEALTH: " + playerHealth.str());//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		healthEntity.setPosition(view.getCenter().x - 260, view.getCenter().y - 215);//задаем позицию текста, отступая от центра камеры
		window.draw(healthEntity);//рисую этот текст

		ostringstream playerScore;
		playerScore << deathN;
		deathEntity.setString("SCORE: " + playerScore.str());
		deathEntity.setPosition(view.getCenter().x - 260, view.getCenter().y - 195);
		window.draw(deathEntity);//рисую этот текст

		if (!player.life) {

			if (player.health < 0) {
				player.health = 0;

				bullets.clear();

			}

			// Сброс флагов и времени
			lastSpawnTime = 0;
			isMove = false;
			dX = 0;
			dY = 0;

			// Очистка векторов врагов и 
			bullets.clear();

			// Если игрок умер, показываем экран окончания игры
			gameOverScreen.draw(window);

			// Если нажата клавиша Enter, перезапускаем игру
			if (gameOverScreen.isRestart(window)) {
				
				// Очистка векторов врагов и пульdddddddddddd
				bullets.clear();
				enemies.clear();

				// Сброс игрового времени
				gameTimeClock.restart();
				gameTime = 0;
				deathN = 0;

				// Сброс параметров существующего игрока
				player.reloadTexture("player.png");
				player.x = 300;
				player.y = 300;
				player.w = 66;
				player.h = 38;
				player.dir = 0;
				player.speed = 0;
				player.dx = 0;
				player.dy = 0;
				player.playerScore = 0;
				player.health = 100;
				player.life = true;

				// Сброс флагов и времени
				lastSpawnTime = 0;
				isMove = false;
				dX = 0;
				dY = 0;
			}
		}

		window.display();
	}
	return 0;
}
