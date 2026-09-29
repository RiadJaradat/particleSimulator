#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

const float gravity = 0.f;
const float strength = 10.f;
const float renderScale = 1 * std::pow(10, -5);
// const double gravitationalConstant = 6.67430 * std::pow(10, -11);
const double gravitationalConstant = 5'000'0;

float simulationSpeed = 20;
float zoom = 2;

sf::VideoMode desktop = sf::VideoMode(1920, 1080);
sf::View camera(
    sf::FloatRect(
        0.f,
        0.f,
        static_cast<float>(desktop.width),
        static_cast<float>(desktop.height)));

sf::Vector2f originalSize = camera.getSize();

struct particle
{
    float radius = 10.f;
    float diameter = radius * 2;
    float mass = 1 * std::pow(10, 9); // 1,000,000,000 kg
    float area;
    float density;

    sf::Vector2f position = {0, 0};
    sf::Vector2f oldPostion = {0, 0};
    sf::Vector2<double> velocity = {0, 0};
    sf::Color color = sf::Color::White;
    sf::CircleShape collisionCircle;

    std::vector<sf::Vertex> trail;
};

std::vector<particle> particles;

inline float delta(float x_final, float x_initial)
{
    return x_final - x_initial;
}

inline double calculateRadius(float mass, float density)
{
    return std::sqrt(mass / (density * std::acosf(-1.0)));
}

inline double calculateDistance(float delta_x, float delta_y)
{
    return std::sqrt(std::pow(delta_x, 2) + std::pow(delta_y, 2));
}

inline double calculateOrbitalVelocity(float mass, float distance)
{
    return std::sqrt((gravitationalConstant * mass) / distance);
}

bool checkCircleCollision(const sf::CircleShape &c1, const sf::CircleShape &c2)
{
    sf::Vector2f center1 = c1.getPosition() + sf::Vector2f(c1.getRadius(), c1.getRadius());
    sf::Vector2f center2 = c2.getPosition() + sf::Vector2f(c2.getRadius(), c2.getRadius());

    float dx = delta(center1.x, center2.x);
    float dy = delta(center1.y, center2.y);

    float distanceSquared = (dx * dx) + (dy * dy);

    float radiiSum = c1.getRadius() + c2.getRadius();
    float radiiSumSquared = radiiSum * radiiSum;

    return distanceSquared <= radiiSumSquared;
}

void update(float dt)
{
    if (simulationSpeed == 0)
        return;

    for (auto &part : particles)
    {
        if (part.position.y < desktop.height - (part.radius * 2))
        {
            part.velocity.y += gravity * strength * dt;
        }
        else
        {
            continue;
            float absorption = part.velocity.y * (40.f / 100.f);
            part.velocity.y -= absorption;
            part.velocity.y *= -1;
        }
    }

    for (size_t i = 0; i < particles.size(); i++)
    {
        for (size_t j = i + 1; j < particles.size(); j++)
        {
            particle &part1 = particles.at(i);
            particle &part2 = particles.at(j);

            float part2Dx = delta(part1.position.x + part1.radius, part2.position.x + part2.radius);
            float part2Dy = delta(part1.position.y + part1.radius, part2.position.y + part2.radius);

            float part1Dx = delta(part2.position.x + part2.radius, part1.position.x + part1.radius);
            float part1Dy = delta(part2.position.y + part2.radius, part1.position.y + part1.radius);

            double distance = calculateDistance(part1Dx, part1Dy);

            if (distance == 0.f)
                continue;

            double force =
                gravitationalConstant * ((part1.mass * part2.mass) /
                                         std::pow(distance, 2));

            double part1Acceleration = force / part1.mass;
            double part2Acceleration = force / part2.mass;

            double part1DirectionX = part1Dx / distance;
            double part1DirectionY = part1Dy / distance;

            double part2DirectionX = part2Dx / distance;
            double part2DirectionY = part2Dy / distance;

            part1.velocity.x += part1Acceleration * part1DirectionX * dt;
            part1.velocity.y += part1Acceleration * part1DirectionY * dt;

            part2.velocity.x += part2Acceleration * part2DirectionX * dt;
            part2.velocity.y += part2Acceleration * part2DirectionY * dt;

            if (checkCircleCollision(part1.collisionCircle, part2.collisionCircle))
            {
                part1.position = part1.oldPostion;
                part2.position = part2.oldPostion;

                part1.collisionCircle.setPosition(part1.oldPostion);
                part2.collisionCircle.setPosition(part2.oldPostion);

                part1.velocity.x -= (part1.velocity.x * 40.f / 100.f);
                part1.velocity.y -= (part1.velocity.y * 40.f / 100.f);
                part2.velocity.x -= (part2.velocity.x * 40.f / 100.f);
                part2.velocity.y -= (part2.velocity.y * 40.f / 100.f);

                part1.velocity.x *= -1;
                part1.velocity.y *= -1;
                part2.velocity.x *= -1;
                part2.velocity.y *= -1;
            }
        }
    }

    for (auto &part : particles)
    {
        float cx = (part.position.x + part.radius) * renderScale;
        float cy = (part.position.y + part.radius) * renderScale;

        part.trail.push_back(sf::Vertex({cx, cy}, sf::Color(255, 255, 255, 120)));
    }

    for (auto &part : particles)
    {
        part.oldPostion = part.position;
        part.position.x += part.velocity.x * dt;
        part.position.y += part.velocity.y * dt;
        part.collisionCircle.setPosition(part.position);
    }
}

void keybinds(sf::View &camera)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if (simulationSpeed >= 20)
        {
            simulationSpeed = 20;
            return;
        }
        simulationSpeed += 0.5f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        if (simulationSpeed <= -20)
        {
            simulationSpeed = -20;
            return;
        }
        simulationSpeed -= 0.5f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        zoom += 0.01f;

        if (zoom > 2.5f)
            zoom = 2.5f;

        camera.setSize(originalSize / zoom);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        zoom -= 0.01f;

        if (zoom < 0.5f)
            zoom = 0.5f;

        camera.setSize(originalSize / zoom);
    }
}

int main()
{

    sf::RenderWindow window(desktop, "Particle Simulator", sf::Style::Fullscreen);
    sf::Clock deltaClock;
    sf::Font font;
    sf::Text simulationSpeedText;
    sf::Text zoomText;

    window.setFramerateLimit(60);
    font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");
    simulationSpeedText.setFont(font);
    simulationSpeedText.setPosition({10, 10});
    zoomText.setFont(font);
    zoomText.setPosition({10, 40});

    // int numberOfParticles = 192;
    int numberOfParticles = 0;
    float spacing = 100.f;

    for (int i = 0; i < numberOfParticles; i++)
    {
        particle part;

        int column = i % static_cast<int>(desktop.width / spacing);
        int row = i / static_cast<int>(desktop.width / spacing);

        part.position.x = column * spacing;
        part.position.y = 100.f + row * spacing;

        part.radius = 5.f;
        part.diameter = part.radius * 2;

        part.collisionCircle.setRadius(part.radius);
        part.collisionCircle.setPosition(part.position);
        part.collisionCircle.setOutlineColor(sf::Color::Red);
        part.collisionCircle.setOutlineThickness(1.f);

        particles.push_back(part);
    }

    particle planet;

    planet.mass = 1 * std::pow(10, 15);
    planet.density = 10;

    planet.radius = calculateRadius(planet.mass, planet.density);
    planet.diameter = planet.radius * 2;

    planet.position.x = (desktop.width - planet.diameter) / 2.f;
    planet.position.y = (desktop.height - planet.diameter) / 2.f;
    planet.oldPostion = planet.position;

    planet.collisionCircle.setPosition(planet.position);
    planet.collisionCircle.setRadius(planet.radius);
    planet.collisionCircle.setOutlineColor(sf::Color::Red);
    planet.collisionCircle.setOutlineThickness(1.f);

    particle rock;

    rock.mass = 1 * std::pow(10, 12);
    rock.density = 1;
    rock.area = 100000;

    rock.radius = calculateRadius(rock.mass, rock.density);
    rock.diameter = rock.radius * 2;

    rock.position = {-1 * std::pow(10, 8), 0};
    rock.oldPostion = rock.position;

    sf::Vector2f rockCenter = rock.position + sf::Vector2f(rock.radius, rock.radius);
    sf::Vector2f planetCenter = planet.position + sf::Vector2f(planet.radius, planet.radius);

    double dist = calculateDistance(planetCenter.x - rockCenter.x,
                                    planetCenter.y - rockCenter.y);
    rock.velocity.y = calculateOrbitalVelocity(planet.mass, dist);

    rock.collisionCircle.setRadius(rock.radius);
    rock.collisionCircle.setPosition(rock.position);
    rock.collisionCircle.setOutlineColor(sf::Color::Red);
    rock.collisionCircle.setOutlineThickness(1.f);

    particles.push_back(rock);
    particles.push_back(planet);

    camera.zoom(2);

    sf::CircleShape partCircle;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        float dt = deltaClock.restart().asSeconds();

        keybinds(camera);
        update(dt * simulationSpeed);
        simulationSpeedText.setString(std::to_string(simulationSpeed));
        zoomText.setString(std::to_string(zoom));
        camera.setCenter(
            (particles.back().position.x + particles.back().radius) * renderScale,
            (particles.back().position.y + particles.back().radius) * renderScale);
        window.setView(camera);
        window.clear();

        for (auto part : particles)
        {
            partCircle.setRadius(part.radius * renderScale);
            partCircle.setPosition({part.position.x * renderScale,
                                    part.position.y * renderScale});
            window.draw(part.trail.data(), part.trail.size(), sf::LinesStrip);
            window.draw(partCircle);
            // window.draw(part.collisionCircle);
        }

        window.setView(window.getDefaultView());
        window.draw(simulationSpeedText);
        window.draw(zoomText);

        window.display();
    }

    return 0;
}
