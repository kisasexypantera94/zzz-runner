#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "ResourcePath.hpp"

#include <random>

const int GROUND = 140;
long long score = 0;
long long cnt_coins = 0;

float offset_x = 0;
float offset_y = 0;

class Player {
   public:
    float dx;
    float dy;
    sf::FloatRect rect;
    bool on_ground;
    sf::Sprite sprite;
    float current_frame;

    Player(sf::Texture& image) {
        sprite.setTexture(image);
        sprite.setTextureRect(sf::IntRect(32, 0, -32, 32));
        rect = sf::FloatRect(50, 120, 32, 32);

        dx = 0;
        dy = 0;
        current_frame = 0;

        on_ground = true;
    }

    void Update(float time) {
        rect.left += dx * time;

        if (!on_ground) {
            dy += 0.0009 * time;
        }

        rect.top += dy * time;

        current_frame += 0.01 * time;

        if (current_frame > 5) {
            current_frame -= 5;
        }

        if (dx > 0) {
            if (!on_ground) {
                sprite.setTextureRect(sf::IntRect(32 * 7, 0, -32, 32));

            } else {
                sprite.setTextureRect(
                    sf::IntRect(32 + 32 * int(current_frame) + 32, 0, -32, 32));
            }
        }

        if (dx < 0) {
            if (!on_ground) {
                sprite.setTextureRect(sf::IntRect(32 * 6, 0, 32, 32));
            } else {
                sprite.setTextureRect(
                    sf::IntRect(32 + 32 * int(current_frame), 0, 32, 32));
            }
        }

        if (dx == 0) {
            if (!on_ground) {
                sprite.setTextureRect(sf::IntRect(32 * 7, 0, -32, 32));
            } else {
                sprite.setTextureRect(sf::IntRect(32, 0, -32, 32));
            }
        }

        dx = 0;

        sprite.setPosition(rect.left - offset_x, rect.top - offset_y);
    }
};

// TODO: fix magic numbers, data types, format code
// TODO: add coin animation
// TODO: add states (menu, game, credits)
int main(int, char const**) {
    sf::RenderWindow window(sf::VideoMode(400, 300), "zzz-runner");

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    sf::Texture t, t1, t2, t3, t4, t5;
    t.loadFromFile(resourcePath() + "zaycev_sprite.png");
    t1.loadFromFile(resourcePath() + "coin2.png");
    t3.loadFromFile(resourcePath() + "dirt.png");
    t4.loadFromFile(resourcePath() + "cloud.png");
    t5.loadFromFile(resourcePath() + "microsoft.png");

    sf::SoundBuffer collect_buffer;
    collect_buffer.loadFromFile(resourcePath() + "collect_coin.wav");
    sf::Sound collect_coin(collect_buffer);

    sf::Sprite s_coin(t1);
    sf::Sprite s_platform(t3);
    sf::Sprite s_background(t2);
    sf::Sprite s_background2(t2);
    sf::Sprite s_cloud(t4);
    sf::Sprite s_obstacle(t5);
    s_obstacle.setOrigin(8.5, 8.5);

    Player p(t);

    sf::Text text;

    sf::Font font;
    font.loadFromFile(resourcePath() + "sansation.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(14);
    text.setStyle(sf::Text::Bold);

    std::vector<std::pair<int, int> > coins(10);
    std::vector<int> coin_pos = {120, 70, 30};

    std::vector<std::vector<std::pair<double, double> > > platforms(
        10, std::vector<std::pair<double, double> >(26));

    std::vector<std::pair<double, double> > clouds(10);

    std::vector<std::pair<double, double> > obstacles(2);

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(0, 399);
    std::uniform_int_distribution<int> distr2(1, 2);
    std::uniform_int_distribution<int> distr3(0, 299);
    std::uniform_int_distribution<int> distr4(10, 100);
    std::uniform_int_distribution<int> distr5(0, 2);

    for (int i = 0; i < 10; ++i) {
        coins[i].first = distr(generator);
        coins[i].second = coin_pos[distr2(generator)];
    }

    for (int j = 0; j < 10; ++j) {
        for (int i = 0; i < 26; ++i) {
            platforms[j][i].first = i * 16;
            platforms[j][i].second = 152 + j * 16;
        }
    }

    for (int i = 0; i < 10; ++i) {
        clouds[i].first = distr(generator);
        clouds[i].second = distr4(generator);
    }

    for (int i = 0; i < 2; ++i) {
        obstacles[i].first = 400 + distr(generator);
        obstacles[i].second = 135;
    }

    // MENU (BEGIN)
    sf::Event ev;

    while (window.isOpen() && ev.type != sf::Event::KeyPressed) {
        sf::Text start;
        start.setFont(font);
        start.setFillColor(sf::Color::White);
        start.setCharacterSize(14);
        start.setStyle(sf::Text::Bold);
        start.setString("NAZHMI NA KNOPKOO");
        start.setPosition(100, 150);

        window.clear(sf::Color::Black);
        window.draw(start);
        window.display();

        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();

            if (ev.type == sf::Event::KeyPressed) break;
        }
    }
    // MENU (END)

    sf::Clock clock;

    // GAME LOOP (BEGIN)
    while (window.isOpen()) {
        score = (1000 * std::clock() / CLOCKS_PER_SEC) + cnt_coins;
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();

        time /= 600;

        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        p.dx = 0.08;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            if (p.on_ground) {
                p.dy = -0.45;
                p.on_ground = false;
            }
        }

        p.Update(time);

        if (p.rect.left > 200) {
            offset_x = p.rect.left - 400 / 2;
        }

        if (p.rect.top > 150) {
            offset_y = p.rect.top - 300 / 2;
        }

        if (p.rect.top + 32 >= 152 && !p.on_ground) {
            p.rect.top = 120;
            p.on_ground = true;
            p.dy = 0;
        }

        window.clear(sf::Color(107, 140, 255));

        // CLOUDS (BEGIN)
        for (int i = 0; i < 3; ++i) {
            if (clouds[i].first + 32 + 198 <= p.rect.left) {
                clouds[i].first = p.rect.left + 200 + distr(generator);
                clouds[i].second = distr4(generator);
            }

            clouds[i].first += (offset_x ? 0.04 * time : 0);

            s_cloud.setPosition(clouds[i].first - offset_x,
                                clouds[i].second - offset_y);
            window.draw(s_cloud);
        }
        // CLOUDS (END)

        // COINS (BEGIN)
        for (int i = 0; i < 6; ++i) {
            if (((p.rect.left + 24 >= coins[i].first &&
                  p.rect.left + 24 <= coins[i].first + 16) ||
                 (p.rect.left + 8 >= coins[i].first &&
                  p.rect.left + 8 <= coins[i].first + 16)) &&
                ((p.rect.top >= coins[i].second &&
                  p.rect.top <= coins[i].second + 16) ||
                 (p.rect.top + 32 >= coins[i].second &&
                  p.rect.top + 24 <= coins[i].second + 16))) {
                collect_coin.play();
                cnt_coins += 100;
            }

            if (coins[i].first + 16 + 200 < p.rect.left ||
                (((p.rect.left + 24 >= coins[i].first &&
                   p.rect.left + 24 <= coins[i].first + 16) ||
                  (p.rect.left + 8 >= coins[i].first &&
                   p.rect.left + 8 <= coins[i].first + 16)) &&
                 ((p.rect.top >= coins[i].second &&
                   p.rect.top <= coins[i].second + 16) ||
                  (p.rect.top + 32 >= coins[i].second &&
                   p.rect.top + 24 <= coins[i].second + 16)))) {
                coins[i].first = p.rect.left + 200 + distr(generator);
                coins[i].second = coin_pos[distr2(generator)];
            }

            s_coin.setPosition(coins[i].first - offset_x,
                               coins[i].second - offset_y);
            window.draw(s_coin);
        }
        // COINS (END)

        // OBSTACLES (BEGIN)
        for (int i = 0; i < 1; ++i) {
            if (((p.rect.left + 24 >= obstacles[i].first &&
                  p.rect.left + 24 <= obstacles[i].first + 17) ||
                 (p.rect.left + 10 >= obstacles[i].first &&
                  p.rect.left + 10 <= obstacles[i].first + 17)) &&
                ((p.rect.top >= obstacles[i].second &&
                  p.rect.top <= obstacles[i].second + 16) ||
                 (p.rect.top + 32 >= obstacles[i].second &&
                  p.rect.top + 24 <= obstacles[i].second + 16))) {
                window.clear(sf::Color::Black);
                sf::Text game_over;
                game_over.setFont(font);
                game_over.setFillColor(sf::Color::Red);
                game_over.setCharacterSize(18);
                game_over.setStyle(sf::Text::Bold);
                game_over.setString("TI PROIGRAL");
                game_over.setPosition(100, 120);

                window.clear(sf::Color::Black);
                window.draw(game_over);
                window.display();

                while (window.isOpen()) {
                    sf::Event ev;

                    while (window.pollEvent(ev)) {
                        if (ev.type == sf::Event::Closed) {
                            window.close();
                        }
                    }
                }
            }

            if (obstacles[i].first + 17 + 200 < p.rect.left) {
                obstacles[i].first = p.rect.left + 200 + distr(generator);
            }

            s_obstacle.rotate(2);
            s_obstacle.setPosition(obstacles[i].first - offset_x,
                                   obstacles[i].second - offset_y);
            window.draw(s_obstacle);
        }
        // OBSTACLES (END)

        // PLATFORMS (BEGIN)
        for (int j = 0; j < 10; ++j) {
            for (int i = 0; i < 26; ++i) {
                if (platforms[j][i].first + 16 + 200 <= p.rect.left) {
                    platforms[j][i].first =
                        (i > 0 ? platforms[j][i - 1].first + 16
                               : platforms[j].back().first + 16);
                }

                s_platform.setPosition(platforms[j][i].first - offset_x,
                                       platforms[j][i].second - offset_y);
                window.draw(s_platform);
            }
        }
        // PLATFORMS (END)

        std::string s = "SCORE: " + std::to_string(score);
        text.setString(s);

        window.draw(text);

        window.draw(p.sprite);

        window.display();
    }
    // GAME LOOP (END)

    return EXIT_SUCCESS;
}
