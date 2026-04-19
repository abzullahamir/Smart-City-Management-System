// SFMLGraphVisualizer.h - FIXED SPREADING WITH HARDCODED ISLAMABAD COORDINATES
#ifndef SFMLGRAPHVISUALIZER_H
#define SFMLGRAPHVISUALIZER_H

#include "Graph.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace sf;

class SFMLGraphVisualizer {
private:
    RenderWindow* window;
    Graph* graph;
    string currentFilter;
    float spriteSize;
    Font font;

    struct VisualNode {
        string id, name, type;
        float x, y, lat, lon;
        Sprite sprite;
        Text label;
    };
    VisualNode* nodes;
    int nodeCount;

    struct VisualEdge {
        string from, to;
    };
    VisualEdge* edges;
    int edgeCount;

    // HARDCODED ISLAMABAD BOUNDS FROM YOUR CSV DATA [file:8]
    float minLat, maxLat, minLon, maxLon;
    float windowWidth, windowHeight;

    struct TextureCache {
        string filename;
        Texture tex;
        bool loaded;
    };
    TextureCache textureCache[20];
    int textureCacheCount;

public:
    SFMLGraphVisualizer(Graph* g);
    ~SFMLGraphVisualizer();
    void visualize(string filter = "ANY");
    void calculateBounds();
    void layoutGraph();
    void drawNodes(string filter);
    void drawEdges();
    void handleInput();
    string getSpriteName(string type);
    int findTextureCache(string filename);
    Texture* loadTextureIfNeeded(string filename);
};

SFMLGraphVisualizer::SFMLGraphVisualizer(Graph* g) : graph(g), nodeCount(0), edgeCount(0), textureCacheCount(0) {
    windowWidth = 1600.0f;
    windowHeight = 1000.0f;
    window = new RenderWindow(VideoMode((unsigned int)windowWidth, (unsigned int)windowHeight), "City Graph Visualizer - Islamabad");
    window->setFramerateLimit(60);
    spriteSize = 28.0f;

    // ? HARDCODED EXACT BOUNDS FROM YOUR nodes.csv [33.52°-33.87°N, 73.01°-73.11°E]
    minLat = 33.523f;    // Giga Mall
    maxLat = 33.873f;    // Shifa Hospital  
    minLon = 73.015f;    // Maroof Hospital
    maxLon = 73.115f;    // Giga Mall

    for (int i = 0; i < 20; i++) {
        textureCache[i].loaded = false;
    }

    if (!font.loadFromFile("arial.ttf")) {
        cout << "Using default font" << endl;
    }

    nodes = new VisualNode[2000];
    edges = new VisualEdge[10000];

    cout << "Islamabad bounds set: Lat[" << minLat << "," << maxLat << "] Lon[" << minLon << "," << maxLon << "]" << endl;
}

SFMLGraphVisualizer::~SFMLGraphVisualizer() {
    delete[] nodes;
    delete[] edges;
    delete window;
}

void SFMLGraphVisualizer::layoutGraph() {
    GraphNode* current = graph->getHead();
    nodeCount = 0;

    while (current && nodeCount < 2000) {
        Node n = current->node;
        string id = n.getNodeID();
        string name = n.getName();
        string type = n.getNodeType();
        float lat = n.getLatitude();
        float lon = n.getLongitude();

        // ? PERFECT SPREADING WITH HARDCODED BOUNDS
        float paddingX = 120.0f;
        float paddingY = 100.0f;
        float latRange = maxLat - minLat;  // ~0.35°
        float lonRange = maxLon - minLon;  // ~0.10°

        float normX = ((lon - minLon) / lonRange) * (windowWidth - 2 * paddingX) + paddingX;
        float normY = ((maxLat - lat) / latRange) * (windowHeight - 2 * paddingY) + paddingY;

        // Clamp to screen
        if (normX < paddingX) normX = paddingX;
        if (normX > windowWidth - paddingX) normX = windowWidth - paddingX;
        if (normY < paddingY) normY = paddingY;
        if (normY > windowHeight - paddingY) normY = windowHeight - paddingY;

        nodes[nodeCount].id = id;
        nodes[nodeCount].name = name;
        nodes[nodeCount].type = type;
        nodes[nodeCount].lat = lat;
        nodes[nodeCount].lon = lon;
        nodes[nodeCount].x = normX;
        nodes[nodeCount].y = normY;
        nodeCount++;
        current = current->next;
    }

    // Build edges
    edgeCount = 0;
    for (int i = 0; i < nodeCount && edgeCount < 10000; i++) {
        GraphNode* gnode = graph->findNode(nodes[i].id);
        if (gnode) {
            Edge* e = gnode->edges;
            while (e && edgeCount < 10000) {
                edges[edgeCount].from = nodes[i].id;
                edges[edgeCount].to = e->destNodeID;
                edgeCount++;
                e = e->next;
            }
        }
    }

    cout << "SPREAD: " << nodeCount << " nodes across " << windowWidth << "x" << windowHeight << " | Edges: " << edgeCount << endl;
}

string SFMLGraphVisualizer::getSpriteName(string type) {
    if (type == "HOSPITAL") return "hospital";
    if (type == "SCHOOL") return "school";
    if (type == "MALL") return "mall";
    if (type == "BUSSTOP") return "busstop";
    if (type == "HOUSE") return "house";
    if (type == "PARK") return "park";
    if (type == "MOSQUE") return "mosque";
    if (type == "PHARMACY") return "pharmacy";
    if (type == "WATERCOOLER") return "watercooler";
    return "node";
}

int SFMLGraphVisualizer::findTextureCache(string filename) {
    for (int i = 0; i < textureCacheCount; i++) {
        if (textureCache[i].filename == filename && textureCache[i].loaded) {
            return i;
        }
    }
    return -1;
}

Texture* SFMLGraphVisualizer::loadTextureIfNeeded(string filename) {
    int cachedIdx = findTextureCache(filename);
    if (cachedIdx >= 0) return &textureCache[cachedIdx].tex;

    if (textureCacheCount >= 20) return nullptr;

    if (textureCache[textureCacheCount].tex.loadFromFile("sprites/" + filename + ".png")) {
        textureCache[textureCacheCount].filename = filename;
        textureCache[textureCacheCount].loaded = true;
        cout << "Loaded: " << filename << ".png" << endl;
        textureCacheCount++;
        return &textureCache[textureCacheCount - 1].tex;
    }
    return nullptr;
}

void SFMLGraphVisualizer::visualize(string filter) {
    currentFilter = filter;
    layoutGraph();

    while (window->isOpen()) {
        Event event;
        while (window->pollEvent(event)) {
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
                window->close();
                return;
            }
        }

        handleInput();
        window->clear(Color(8, 15, 25));

        drawEdges();
        drawNodes(filter);

        // UI
        Text filterText;
        filterText.setFont(font);
        filterText.setString("Filter: " + filter + " | 1=Hosp 2=Sch 3=Mall 4=Bus 5=House 6=Park 7=Mosque 0=ALL | ESC=Exit");
        filterText.setCharacterSize(20);
        filterText.setFillColor(Color::White);
        filterText.setPosition(20, 15);
        window->draw(filterText);

        Text stats;
        stats.setFont(font);
        stats.setString("Islamabad: " + to_string(nodeCount) + " nodes | " + to_string(edgeCount) + " roads");
        stats.setCharacterSize(18);
        stats.setFillColor(Color(150, 200, 255));
        stats.setPosition(20, 45);
        window->draw(stats);

        window->display();
    }
}

void SFMLGraphVisualizer::drawNodes(string filter) {
    for (int i = 0; i < nodeCount; i++) {
        if (filter != "ANY" && nodes[i].type != filter) continue;

        string spriteName = getSpriteName(nodes[i].type);
        Texture* tex = loadTextureIfNeeded(spriteName);

        Sprite& sprite = nodes[i].sprite;
        if (tex) {
            sprite.setTexture(*tex);
            float scaleX = spriteSize / (float)tex->getSize().x;
            float scaleY = spriteSize / (float)tex->getSize().y;
            sprite.setScale(scaleX, scaleY);
        }
        else {
            // Small fallback circle
            CircleShape circle(spriteSize * 0.45f);
            circle.setFillColor(Color(80, 120, 180));
            circle.setOutlineColor(Color(180, 220, 255));
            circle.setOutlineThickness(1.8f);
            circle.setPosition(nodes[i].x - spriteSize * 0.45f, nodes[i].y - spriteSize * 0.45f);
            window->draw(circle);
            continue; // Skip sprite drawing
        }

        sprite.setPosition(nodes[i].x - spriteSize / 2.0f, nodes[i].y - spriteSize / 2.0f);
        sprite.setOrigin(0, 0);
        window->draw(sprite);

        // Tiny labels
        Text& label = nodes[i].label;
        label.setFont(font);
        string shortName = nodes[i].name;
        if (shortName.length() > 12) shortName = shortName.substr(0, 12) + "...";
        label.setString(shortName);
        label.setCharacterSize(11);
        label.setFillColor(Color(255, 255, 255, 220));
        FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.width / 2.0f, 0);
        label.setPosition(nodes[i].x, nodes[i].y + spriteSize / 2.0f + 4);
        window->draw(label);

        Text idTag;
        idTag.setFont(font);
        string shortId = nodes[i].id;
        if (shortId.length() > 5) shortId = shortId.substr(0, 5);
        idTag.setString(shortId);
        idTag.setCharacterSize(9);
        idTag.setFillColor(Color(100, 180, 255, 200));
        FloatRect idBounds = idTag.getLocalBounds();
        idTag.setOrigin(idBounds.width / 2.0f, idBounds.height);
        idTag.setPosition(nodes[i].x, nodes[i].y - spriteSize / 2.0f - 3);
        window->draw(idTag);
    }
}

void SFMLGraphVisualizer::drawEdges() {
    for (int i = 0; i < edgeCount; i++) {
        int fromIdx = -1, toIdx = -1;
        for (int j = 0; j < nodeCount; j++) {
            if (nodes[j].id == edges[i].from) { fromIdx = j; break; }
        }
        for (int j = 0; j < nodeCount; j++) {
            if (nodes[j].id == edges[i].to) { toIdx = j; break; }
        }
        if (fromIdx == -1 || toIdx == -1) continue;

        VertexArray line(Lines, 2);
        line[0].position = Vector2f(nodes[fromIdx].x, nodes[fromIdx].y);
        line[1].position = Vector2f(nodes[toIdx].x, nodes[toIdx].y);
        line[0].color = Color(70, 110, 150, 70);
        line[1].color = Color(70, 110, 150, 70);
        window->draw(line);
    }
}

void SFMLGraphVisualizer::handleInput() {
    static Clock inputTimer;
    if (inputTimer.getElapsedTime().asSeconds() < 0.3f) return;

    if (Keyboard::isKeyPressed(Keyboard::Num1)) { visualize("HOSPITAL"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num2)) { visualize("SCHOOL"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num3)) { visualize("MALL"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num4)) { visualize("BUSSTOP"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num5)) { visualize("HOUSE"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num6)) { visualize("PARK"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num7)) { visualize("MOSQUE"); inputTimer.restart(); }
    else if (Keyboard::isKeyPressed(Keyboard::Num0)) { visualize("ANY"); inputTimer.restart(); }
}

#endif