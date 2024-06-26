#pragma once

#include "ofMain.h"
#include <vector>
#include <string>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

    // Player attributes
    ofVec2f playerPos;
    ofVec2f playerVel;
    bool isJumping;

    // Platform attributes
    vector<ofRectangle> platforms;

    // Obstacle attributes
    struct Obstacle {
        ofVec2f pos;
        ofVec2f size;
        int type; // 0: rectangle, 1: triangle, 2: circle
    };
    vector<Obstacle> obstacles;
    float obstacleSpawnTime;
    float lastObstacleSpawn;
    float obstacleSpeed;

    // Game attributes
    int lives;
    float startTime;
    float endTime;
    bool isGameOver;
    bool isInvincible;
    float invincibilityTime;
    float invincibilityDuration;

    // Constants
    const float gravity = 0.5;
    const float jumpForce = -15;
    float moveSpeed;

    // Background attributes
    ofImage cloudImage;
    vector<ofVec2f> clouds;

    // Player image
    ofImage kirbyImage;

    // Ranking system
    vector<float> rankings;
    void loadRankings();
    void saveRankings();
    void updateRankings();
};
