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
public:
	float x, y;
public:
	float w, h, dx, dy, speed;
	int dir, playerScore, health;
	bool life;
	String File;																										
	Image image;
	Texture texture;
	Sprite sprite;

	FloatRect getRect() {
		return sf::FloatRect(x, y, w, h);
	}

	Player(String F, int X, int Y, float W, float H) {
		dir = 0; speed = 0; playerScore = 0; health = 100;
		life = true;
		dx = 0; dy = 0;
		File = F;
		w = W; h = H;
		image.loadFromFile("images/" + File);
		image.createMaskFromColor(Color(255, 255, 255));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		x = X; y = Y;
		sprite.setTextureRect(IntRect(0, 0, w, h));
		sprite.setOrigin(w / 2, h / 2);
	}

	void reloadTexture(String F) {
		File = F;
		image.loadFromFile("images/" + File);
		image.createMaskFromColor(Color(255, 255, 255));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, w, h));
		sprite.setOrigin(w / 2, h / 2);
	}

	void update(float time)
	{
		isShiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

		if (isShiftPressed && !isReloading && !isBoosting) {
			isBoosting = true;
			boostTimer.restart();
		}

		if (isBoosting && boostTimer.getElapsedTime().asSeconds() > 3.0f) {
			isBoosting = false;
			isReloading = true;
			reloadTimer.restart();
		}

		if (isReloading && reloadTimer.getElapsedTime().asSeconds() > 10.0f) {
			isReloading = false;
		}

		float speedMultiplier = (isBoosting ? 2.0f : 1.0f);

		switch (dir)
		{
		case 0: dx = speed * speedMultiplier; dy = 0; break;
		case 1: dx = -speed * speedMultiplier; dy = 0; break;
		case 2: dx = 0; dy = speed * speedMultiplier; break;
		case 3: dx = 0; dy = -speed * speedMultiplier; break;
		}

		x += dx * time;
		y += dy * time;

		speed = 0;
		sprite.setPosition(x, y);
		if (health <= 0) { life = false; speed = 0; }
	}
	float GetPlayerCoordinateX() {
		return x;
	}
	float GetPlayerCoordinateY() {
		return y;
	}
	void interactionWithMap() {
		for (int i = y / 32; i < (y + h) / 32; i++)
			for (int j = x / 32; j < (x + w) / 32; j++)
			{
				if (TileMap[i][j] == '0')
				{
					if (dy > 0)
					{
						y = i * 32 - h;
					}
					if (dy < 0)
					{
						y = i * 32 + 32;
					}
					if (dx > 0)
					{
						x = j * 32 - w;
					}
					if (dx < 0)
					{
						x = j * 32 + 32;
					}
				}
				if (TileMap[i][j] == 'f') {
					health -= 40;
					deathN -= 10;
					TileMap[i][j] = ' ';
					int new_i, new_j;
					do {
						new_i = std::rand() % HEIGHT_MAP; // замените HEIGHT на высоту вашей карты
						new_j = std::rand() % WIDTH_MAP; // замените WIDTH на ширину вашей карты
					} while (TileMap[new_i][new_j] != ' '); // повторяем, пока не найдем свободное место


					TileMap[new_i][new_j] = 'f';
				}

				if (TileMap[i][j] == 'h') {
					health += 20;
					deathN += 5;
					TileMap[i][j] = ' ';
					if (health > 100) {
						health = 100;
					}
					int new_i, new_j;
					do {
						new_i = std::rand() % HEIGHT_MAP; // замените HEIGHT на высоту вашей карты
						new_j = std::rand() % WIDTH_MAP; // замените WIDTH на ширину вашей карты
					} while (TileMap[new_i][new_j] != ' '); // повторяем, пока не найдем свободное место


					TileMap[new_i][new_j] = 'h';
				}
			}
	}
private:
	bool isShiftPressed;
	bool isBoosting;
	bool isReloading;
	Clock boostTimer;
	Clock reloadTimer;
};

class Enemy :public Entity {
public:
	Enemy(sf::Texture& texture, float X, float Y, int W, int H, String Name) :Entity(texture, X, Y, W, H, Name) {
		if (name == "Enemy") {
			sprite.setTextureRect(sf::IntRect(0, 0, w, h));
			dx = 0.1;//даем скорость.этот объект всегда двигается
		}
	}
	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
			for (int j = x / 32; j < (x + w) / 32; j++)
			{
				// Проверяем, что индексы не выходят за пределы массива
				if (i >= 0 && i < HEIGHT_MAP && j >= 0 && j < WIDTH_MAP && TileMap[i][j] == '0')
				{
					if (dy > 0 && i * 32 - h >= 0)
					{
						y = i * 32 - h;
						dy = -dy; // изменяем направление движения
					}
					if (dy < 0 && i * 32 + 32 < HEIGHT_MAP * 32)
					{
						y = i * 32 + 32;
						dy = -dy; // изменяем направление движения
					}
					if (dx > 0 && j * 32 - w >= 0)
					{
						x = j * 32 - w;
						dx = -dx; // изменяем направление движения
					}
					if (dx < 0 && j * 32 + 32 < WIDTH_MAP * 32)
					{
						x = j * 32 + 32;
						dx = -dx; // изменяем направление движения
					}
				}
			}
	}
	float GetEnemyCoordinateX() {
		return x;
	}
	float GetEnemyCoordinateY() {
		return y;
	}
	void update(float time, float playerX, float playerY) // добавляем координаты игрока в параметры функции
	{
		if (name == "Enemy") {
			checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
			x += dx * time;
			checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
			y += dy * time;
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра

			// Вычисляем вектор направления от врага к игроку
			float directionX = playerX - x;
			float directionY = playerY - y;

			// Нормализуем вектор направления (делаем его длину равной 1), чтобы скорость врага была постоянной
			float length = sqrt(directionX * directionX + directionY * directionY);
			directionX /= length;
			directionY /= length;

			// Обновляем dx и dy в соответствии с направлением к игроку
			dx = directionX * 0.05; // speed - это скорость врага
			dy = directionY * 0.05;


			if (health <= 0) { life = false; }
		}
	}
};

class Bullet : public Entity {
public:
	Bullet(sf::Texture& texture, String Name, float X, float Y, int W, int H, float DX, float DY)
		: Entity(texture, X, Y, W, H, Name) {
		dx = DX;
		dy = DY;
		speed = 0.8;
		w = h = 16;
		life = true;

		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	void update(float time)
	{
		x += dx * time * speed; // обновляем положение пули
		y += dy * time * speed;

		sprite.setPosition(x + w / 2, y + h / 2);
	}
};

// Текстура врага
Texture enemyTexture;

int main()
{
	srand(time(NULL));

	// Window
	RenderWindow window(VideoMode(1440, 960), "game", Style::Fullscreen);
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

		bool spawnEnemy = true;

		if (spawnEnemy == true) {
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
		if (spawnEnemy == true) {
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
		if (spawnEnemy == true) {
			for (int i = 0; i < enemies.size(); i++) {
				window.draw(enemies[i].sprite);
			}
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
				enemies.clear();

			}

			// Сброс флагов и времени
			lastSpawnTime = 0;
			isMove = false;
			spawnEnemy = false;
			dX = 0;
			dY = 0;

			// Очистка векторов врагов 
			bullets.clear();
			enemies.clear();

			// Если игрок умер, показываем экран окончания игры
			gameOverScreen.draw(window);

			// Если нажата клавиша Enter, перезапускаем игру
			if (gameOverScreen.isRestart(window)) {
				
				spawnEnemy = false;

				// Очистка векторов врагов и пульdddddddddddd
				bullets.clear();

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
