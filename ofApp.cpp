#include "ofApp.h"
#include <fstream>
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup() {
    playerPos.set(100, 300); // Initial position of the player
    playerVel.set(0, 0);
    isJumping = false;

    // Setup platforms
    platforms.push_back(ofRectangle(0, 400, 1024, 20)); // Ground platform
   

    // Initialize game attributes
    lives = 3;
    isGameOver = false;
    isInvincible = false;
    invincibilityDuration = 2.0; // 2 seconds of invincibility after getting hit
    startTime = ofGetElapsedTimef();
    lastObstacleSpawn = 0;
    obstacleSpawnTime = 2.0; // Spawn a new obstacle every 2 seconds
    obstacleSpeed = moveSpeed = 5;

    // Load images
    cloudImage.load("cloud.png"); // Ensure you have a cloud.png in the data folder
    kirbyImage.load("kirby.png"); // Ensure you have a kirby.png in the data folder

    // Initialize clouds with fixed positions above the play area
    clouds.push_back(ofVec2f(50, 3));
    clouds.push_back(ofVec2f(200, 10));
    clouds.push_back(ofVec2f(450, 15));
    clouds.push_back(ofVec2f(600, 5));
    clouds.push_back(ofVec2f(850, 10));

    ofSetFrameRate(60);
    ofBackground(135, 206, 235); // Light blue sky color

    // Load rankings
    loadRankings();
}

//--------------------------------------------------------------
void ofApp::update() {
    if (isGameOver) return;

    // Increase obstacle speed every 10 seconds
    float elapsedTime = ofGetElapsedTimef() - startTime;
    if (int(elapsedTime) % 10 == 0 && elapsedTime > 0) {
        obstacleSpeed += 0.1;
    }

    // Apply gravity
    playerVel.y += gravity;

    // Apply movement
    playerPos += playerVel;

    // Check for collisions with platforms
    for (auto& platform : platforms) {
        if (playerPos.y + 80 > platform.y && playerPos.y < platform.y + platform.height &&
            playerPos.x + 80 > platform.x && playerPos.x < platform.x + platform.width) {
            playerVel.y = 0;
            playerPos.y = platform.y - 80;
            isJumping = false;
        }
    }

    // Spawn new obstacles
    if (ofGetElapsedTimef() - lastObstacleSpawn > obstacleSpawnTime) {
        Obstacle obstacle;
        obstacle.pos.set(ofGetWidth(), 400);
        obstacle.size.set(ofRandom(50, 150), ofRandom(50, 150));
        obstacle.type = int(ofRandom(3)); // Randomly choose between 0, 1, and 2
        obstacles.push_back(obstacle);
        lastObstacleSpawn = ofGetElapsedTimef();
    }

    // Move and check collision with obstacles
    for (auto& obstacle : obstacles) {
        obstacle.pos.x -= obstacleSpeed;

        if (playerPos.x + 80 > obstacle.pos.x && playerPos.x < obstacle.pos.x + obstacle.size.x &&
            playerPos.y + 80 > obstacle.pos.y - obstacle.size.y && playerPos.y < obstacle.pos.y) {
            if (!isInvincible) {
                lives--;
                isInvincible = true;
                invincibilityTime = ofGetElapsedTimef();
                if (lives <= 0) {
                    isGameOver = true;
                    endTime = ofGetElapsedTimef();
                    updateRankings();
                }
            }
        }
    }

    // Handle invincibility timing
    if (isInvincible && (ofGetElapsedTimef() - invincibilityTime > invincibilityDuration)) {
        isInvincible = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    if (!isGameOver) {
        // Draw background ground
        ofSetColor(139, 69, 19); // Brown color for ground
        ofDrawRectangle(0, 420, ofGetWidth(), ofGetHeight() - 420);

        // Draw player
        if (isInvincible) {
            ofSetColor(255, 0, 0); // Yellow for invincibility
        }
        else {
            ofSetColor(255, 255, 255);
        }
        kirbyImage.draw(playerPos.x, playerPos.y, 80, 80); // Larger player image

        // Draw platforms
        ofSetColor(139, 69, 19);
        for (auto& platform : platforms) {
            ofDrawRectangle(platform);
        }

        // Draw obstacles
        for (auto& obstacle : obstacles) {
            ofSetColor(255,0,0);
            if (obstacle.type == 0) { // Rectangle
                ofDrawRectangle(obstacle.pos.x, obstacle.pos.y - obstacle.size.y, obstacle.size.x, obstacle.size.y);
            }
            else if (obstacle.type == 1) { // Triangle
                ofDrawTriangle(obstacle.pos.x, obstacle.pos.y,
                    obstacle.pos.x + obstacle.size.x / 2, obstacle.pos.y - obstacle.size.y,
                    obstacle.pos.x + obstacle.size.x, obstacle.pos.y);
            }
            else if (obstacle.type == 2) { // Circle
                ofDrawCircle(obstacle.pos.x + obstacle.size.x / 2, obstacle.pos.y - obstacle.size.y / 2, obstacle.size.x / 2);
            }
        }

        // Draw clouds
        ofSetColor(255);
        for (auto& cloud : clouds) {
            cloudImage.draw(cloud.x, cloud.y, cloudImage.getWidth() * 0.5, cloudImage.getHeight() * 0.5);
        }

        // Draw lives
        ofSetColor(255, 255, 255);
        ofDrawBitmapString("Lives: " + ofToString(lives), 10, 20);
    }
    else {
        // Clear the screen
        ofClear(0, 0, 0);

        // Draw game over and time
        ofSetColor(255, 255, 255);
        ofDrawBitmapString("Game Over!", ofGetWidth() / 2 - 40, ofGetHeight() / 2 - 20);
        ofDrawBitmapString("Time Survived: " + ofToString(endTime - startTime) + " seconds", ofGetWidth() / 2 - 80, ofGetHeight() / 2);

        // Draw rankings
        ofDrawBitmapString("Top 10 Rankings:", ofGetWidth() / 2 - 80, ofGetHeight() / 2 + 40);
        for (int i = 0; i < min(10, (int)rankings.size()); ++i) {
            ofDrawBitmapString(ofToString(i + 1) + ": " + ofToString(rankings[i]) + " seconds", ofGetWidth() / 2 - 80, ofGetHeight() / 2 + 60 + i * 20);
        }
        // Draw player's rank
        auto it = find(rankings.begin(), rankings.end(), endTime - startTime);
        if (it != rankings.end()) {
            int rank = distance(rankings.begin(), it) + 1;
            ofDrawBitmapString("Your Rank: " + ofToString(rank), ofGetWidth() / 2 - 80, ofGetHeight() / 2 + 280);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == OF_KEY_UP && !isJumping) {
        playerVel.y = jumpForce;
        isJumping = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == OF_KEY_UP) {
        playerVel.y = 0;
    }
}

//--------------------------------------------------------------
void ofApp::loadRankings() {
    ifstream file("rank.txt");
    if (file.is_open()) {
        float time;
        while (file >> time) {
            rankings.push_back(time);
        }
        file.close();
    }
    sort(rankings.begin(), rankings.end(), greater<float>());
}

//--------------------------------------------------------------
void ofApp::saveRankings() {
    ofstream file("rank.txt");
    if (file.is_open()) {
        for (float time : rankings) {
            file << time << endl;
        }
        file.close();
    }
}

//--------------------------------------------------------------
void ofApp::updateRankings() {
    rankings.push_back(endTime - startTime);
    sort(rankings.begin(), rankings.end(), greater<float>());
    if (rankings.size() > 10) {
        rankings.resize(10);
    }
    saveRankings();
}
