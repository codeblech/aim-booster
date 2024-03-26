#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <random> // random target location
#include <chrono> // to seed rand with nanoseconds
#include <fstream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "AimBooster");
    sf::Texture texture;
    if (!texture.loadFromFile("Textures\\target.png", sf::IntRect(0, 0, 280, 280)))
    {
        std::cout << "can't load target texture";
    }
    texture.setSmooth(true);

    sf::Clock target_clock;
    sf::Clock game_clock;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.5, 1.5);

    float spawnTime = 1.f;
    std::cout << "spawn time " << rand() << '\n';
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<sf::CircleShape, float>> targets;

    sf::Font font;
    if (!font.loadFromFile("Font\\Charlie don't surf.ttf"))
    {
        std::cout << "can't load font";
    }

    int score = 0;
    sf::Text score_text;
    score_text.setFont(font);
    score_text.setFillColor(sf::Color::White);
    score_text.setCharacterSize(30);
    score_text.setPosition(0, 0);
    std::string score_header = "Score: ";
    score_text.setString(score_header + std::to_string(score));

    int lives = 3;
    sf::Text lives_text;
    lives_text.setFont(font);
    lives_text.setFillColor(sf::Color::White);
    lives_text.setCharacterSize(30);
    lives_text.setPosition(0, 35);
    std::string lives_header = "Lives: ";
    lives_text.setString(lives_header + std::to_string(lives));

    bool game_over = false;
    sf::Text game_over_text;
    game_over_text.setFont(font);
    game_over_text.setString("GAME OVER");
    game_over_text.setFillColor(sf::Color::Red);
    game_over_text.setCharacterSize(50);
    game_over_text.setOrigin(game_over_text.getGlobalBounds().width / 2.f, game_over_text.getGlobalBounds().height / 2.f);
    game_over_text.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 100.f);

    bool saved_score_to_file = false;

    while (window.isOpen())
    {
        if (!game_over)
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                else if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }

                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                    // remove (not erase) based on the predicate of a lambda function that checks if the click was within bounds of any target.
                    // if predicate(condition) in the lambda function returns true, target is removed.
                    auto iter = std::remove_if(targets.begin(), targets.end(), [&](const auto &target)
                                               {
                        sf::FloatRect bounds = target.first.getGlobalBounds();
                        if (bounds.contains(mousePosition.x, mousePosition.y))
                        {
                            // calculate the index of clicked target using address of the clicked target and first target in the vector 'targets'
                            std::cout << "clicked on target " << &target - &targets[0] << std::endl;
                            return true;
                        }
                        return false; });

                    // erase the removed elements. erase only if a target was removed, i.e, iter is not pointing to the last target.
                    // iter pointing to last target means no target was removed, suggesting user clicked outside of any target
                    if (iter != targets.end())
                    {
                        score++;
                        score_text.setString(score_header + std::to_string(score));
                        targets.erase(iter, targets.end());
                    }
                    else
                    {
                        // Clicked outside of any target
                        lives--;
                        lives_text.setString(lives_header + std::to_string(lives));
                        std::cout << "clicked outside of any target" << std::endl;
                    }
                }
            }

            if (game_clock.getElapsedTime().asSeconds() > spawnTime)
            {
                std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
                srand(duration.count());

                float target_radius = 60;
                sf::CircleShape target(target_radius);
                target.setOrigin(target.getGlobalBounds().width / 2.f, target.getGlobalBounds().height / 2.f);
                target.setPosition(rand() % window.getSize().x, rand() % window.getSize().y);
                target.setFillColor(sf::Color::White);
                target.setTexture(&texture);
                target.setTextureRect(sf::IntRect(0, 0, 280, 280));
                std::cout << "newtarget\n";
                targets.push_back(std::make_pair(target, 1.f));
                game_clock.restart();
            }

            float duration = target_clock.restart().asSeconds();

            std::vector<std::pair<sf::CircleShape, float>>::iterator ptr;
            ptr = targets.begin();
            for (auto &target : targets)
            {
                if (target.first.getScale().x > 0.05f)
                {
                    target.second -= duration * 0.3;
                    target.first.setScale(target.second, target.second);
                }
                else
                {
                    std::cout << "erase target\n";
                    targets.erase(ptr);
                    lives--;
                    lives_text.setString(lives_header + std::to_string(lives));
                }
                ptr++;
            }

            window.clear();
            for (const auto &target : targets)
            {
                window.draw(target.first);
            }
            window.draw(score_text);
            window.draw(lives_text);
            window.display();
        }

        if (lives <= 0) // if there are no lives remaining, display the final score
        {
            game_over = true;
            if (!saved_score_to_file)
            {
                std::ofstream score_writer;
                score_writer.open("Saves\\scores.txt", std::ios::app);
                const auto now = std::chrono::system_clock::now();
                const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
                score_writer << "Score: " << score << " - Date: " << std::ctime(&t_c) << std::endl;
                score_writer.close();
                saved_score_to_file = true;

                std::ifstream highscore_reader;
                highscore_reader.open("Saves\\highscore.dat");
                if(!highscore_reader)
                    std::cout << "error opening highscore.dat";

                int highscore;
                highscore_reader >> highscore;
                std::cout << "highscore: " << highscore;
                if (score > highscore)
                {
                    highscore = score;
                    std::ofstream highscore_writer;
                    highscore_writer.open("Saves\\highscore.dat", std::ios::out);
                    highscore_writer << highscore;
                    std::cout << "highscore rewritten: " << highscore;
                }
            }

            sf::RectangleShape NewGameButton(sf::Vector2f(200.f, 100.f));
            NewGameButton.setFillColor(sf::Color(255, 96, 0, 100));
            NewGameButton.setOutlineThickness(5.f);
            NewGameButton.setOutlineColor(sf::Color(255, 96, 0, 255));
            NewGameButton.setOrigin(NewGameButton.getGlobalBounds().width / 2.f, NewGameButton.getGlobalBounds().height / 2.f);
            NewGameButton.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);

            sf::Text NewGameText;
            NewGameText.setFont(font);
            NewGameText.setString("New Game");
            NewGameText.setFillColor(sf::Color(255, 96, 0, 255));
            NewGameText.setCharacterSize(25);
            NewGameText.setOrigin(NewGameText.getGlobalBounds().width / 2.f, NewGameText.getGlobalBounds().height / 2.f);
            NewGameText.setPosition(NewGameButton.getPosition().x, NewGameButton.getPosition().y);

            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    sf::FloatRect bounds = NewGameButton.getGlobalBounds();
                    if (bounds.contains(mousePosition.x, mousePosition.y))
                    {
                        std::cout << "newgame\n";
                        game_over = false;
                        saved_score_to_file = false;
                        targets.clear();

                        score = 0;
                        score_text.setString(score_header + std::to_string(score));
                        lives = 3;
                        lives_text.setString(lives_header + std::to_string(lives));
                    }
                }
            }
            window.clear();
            window.draw(NewGameButton);
            window.draw(NewGameText);
            window.draw(score_text);
            window.draw(game_over_text);
            window.display();
        }
    }
}