// Replace the first line with these macOS-specific includes
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
// Keep the rest of the includes
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <initializer_list>
#include <unistd.h>

// Function Prototypes
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void update(int value);
void drawBird();
void drawPipes();
void drawGround();
void drawSky();
void drawMenu();
void drawInstructions();
void drawGameOver();
void drawScore();
void resetGame();
void initPipes();
bool checkCollision();
void renderText(float x, float y, const char* text, GLfloat* color, bool isBold = false, float scale = 1.0f);
void drawCelebration();

// Game Constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PIPE_WIDTH 60
#define PIPE_GAP 150  // Gap between top and bottom pipe (vertical space for bird)
#define PIPE_SPACING 300  // Horizontal spacing between pipe sets
#define BIRD_SIZE 30
#define GRAVITY 0.25
#define FLAP_VELOCITY -5.0
#define PIPE_SPEED 2.5



//colors
// Enhanced color definitions with gradients
typedef struct {
    GLfloat top[3];
    GLfloat bottom[3];
} GradientColor;

// Sky gradient (sunset-like colors)
GradientColor skyGradient = {
    .top = {0.529f, 0.808f, 0.922f},    // Light blue
    .bottom = {0.941f, 0.862f, 0.510f}   // Light golden
};

// Ground gradient (rich earth tones)
GradientColor groundGradient = {
    .top = {0.545f, 0.371f, 0.153f},     // Brown
    .bottom = {0.373f, 0.247f, 0.088f}   // Dark brown
};

// Bird colors (more vibrant yellow with orange gradient)
GradientColor birdGradient = {
    .top = {1.0f, 0.894f, 0.109f},       // Bright yellow
    .bottom = {0.999f, 0.659f, 0.031f}   // Golden orange
};

// Pipe colors (realistic green with darker gradient)
GradientColor pipeGradient = {
    .top = {0.180f, 0.832f, 0.372f},     // Bright green
    .bottom = {0.180f, 0.649f, 0.372f}   // Dark green
};

// Cloud colors
GLfloat cloudColor[] = {1.0f, 1.0f, 1.0f};
GLfloat cloudShadowColor[] = {0.95f, 0.95f, 0.95f};

// Shadow properties
#define SHADOW_OFFSET_X 5.0f
#define SHADOW_OFFSET_Y 5.0f
#define SHADOW_ALPHA 0.3f

// Function to draw a gradient rectangle
void drawGradientRect(float x1, float y1, float x2, float y2, 
                     GLfloat topColor[3], GLfloat bottomColor[3]) {
    glBegin(GL_QUADS);
    // Top vertices
    glColor3f(topColor[0], topColor[1], topColor[2]);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    
    // Bottom vertices
    glColor3f(bottomColor[0], bottomColor[1], bottomColor[2]);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// Function to draw shadow
void drawShadow(float x, float y, float size, float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(0.0f, 0.0f, 0.0f, alpha);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 36) {
        float angle = i * 3.14159 / 180;
        glVertex2f(x + size * cos(angle), y + size * sin(angle));
    }
    glEnd();
    
    glDisable(GL_BLEND);
}

//for animation
// Animation variables //animation_variables.h
float birdWingAngle = 0.0f;
float birdWingSpeed = 15.0f;  // Increased speed for more dynamic movement
bool wingDirection = true;
float birdWingScale = 1.0f;   // For wing scaling effect
float birdWingScaleSpeed = 0.05f;
bool wingScaleDirection = true;

// Particle system
#define MAX_PARTICLES 100  // Increased particle count
struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float r, g, b;
    float size;
    bool active;
};
Particle particles[MAX_PARTICLES];


//animation_function.h
// Initialize particles
void initParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
        particles[i].size = 2.0f + (rand() % 3);  // Random size
    }
}

// Create particles at position
void createParticles(float x, float y, float r, float g, float b) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = (rand() % 100 - 50) / 25.0f;
            particles[i].vy = (rand() % 100 - 50) / 25.0f;
            particles[i].life = 1.0f;
            particles[i].r = r;
            particles[i].g = g;
            particles[i].b = b;
            particles[i].size = 2.0f + (rand() % 3);
            particles[i].active = true;
            break;
        }
    }
}

// Update particles
void updateParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].vy += 0.1f; // Gravity effect
            particles[i].life -= 0.02f;
            
            // Add some random movement
            if (rand() % 10 == 0) {
                particles[i].vx += (rand() % 20 - 10) / 50.0f;
            }
            
            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}

// Draw particles
void drawParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            float alpha = particles[i].life;
            float size = particles[i].size * alpha;
            
            // Draw particle with glow effect
            for (int j = 0; j < 3; j++) {
                float glowAlpha = alpha * (0.3f - j * 0.1f);
                float glowSize = size + j * 2;
                
                glColor4f(particles[i].r, particles[i].g, particles[i].b, glowAlpha);
                glBegin(GL_QUADS);
                glVertex2f(particles[i].x - glowSize, particles[i].y - glowSize);
                glVertex2f(particles[i].x + glowSize, particles[i].y - glowSize);
                glVertex2f(particles[i].x + glowSize, particles[i].y + glowSize);
                glVertex2f(particles[i].x - glowSize, particles[i].y + glowSize);
                glEnd();
            }
        }
    }
    
    glDisable(GL_BLEND);
}




// Game States
enum GameState {
    MENU,
    INSTRUCTIONS,
    PLAYING,
    GAME_OVER
};

// Game Variables
GameState currentState = MENU;
int menuSelection = 0;
int score = 0;
int highScore = 0;
bool keys[256];

// Bird Properties
float birdX = WINDOW_WIDTH / 4;
float birdY = WINDOW_HEIGHT / 2;
float birdVelocity = 0;
float birdRotation = 0;

// Pipe Properties
struct Pipe {
    float x;
    float gapY;
    bool counted;
};

const int MAX_PIPES = 5;
Pipe pipes[MAX_PIPES];
float pipeTimer = 0;
int activePipes = 0;

// Colors
GLfloat skyColor[] = {0.4f, 0.7f, 1.0f};
GLfloat groundColor[] = {0.8f, 0.6f, 0.3f};
GLfloat birdColor[] = {1.0f, 1.0f, 0.0f};
GLfloat pipeColor[] = {0.0f, 0.8f, 0.0f};
GLfloat textColor[] = {1.0f, 1.0f, 1.0f};
GLfloat highlightColor[] = {1.0f, 0.8f, 0.0f};

// Add highlight colors for the bird
GLfloat birdHighlightColor[] = {1.0f, 1.0f, 0.8f};
GLfloat birdShadowColor[] = {0.8f, 0.6f, 0.0f};

// Add pipe highlight and shadow colors
GLfloat pipeHighlightColor[] = {0.2f, 0.9f, 0.4f};
GLfloat pipeShadowColor[] = {0.1f, 0.5f, 0.2f};

// Add cloud animation variables
float cloudOffset = 0.0f;
float cloudSpeed = 0.5f;

// Add ground highlight and shadow colors
GLfloat groundHighlightColor[] = {0.6f, 0.4f, 0.2f};
GLfloat groundShadowColor[] = {0.3f, 0.2f, 0.1f};

// Add milestone tracking
int lastMilestone = 0;
float celebrationTimer = 0.0f;
bool isCelebrating = false;
#define CELEBRATION_DURATION 2.0f  // Duration in seconds

// Add celebration colors
GLfloat celebrationColors[][3] = {
    {1.0f, 0.0f, 0.0f},  // Red
    {0.0f, 1.0f, 0.0f},  // Green
    {0.0f, 0.0f, 1.0f},  // Blue
    {1.0f, 1.0f, 0.0f},  // Yellow
    {1.0f, 0.0f, 1.0f}   // Purple
};

// Add milestone messages
const char* milestoneMessages[] = {
    "5 POINTS!",
    "10 POINTS!",
    "20 POINTS!",
    "40 POINTS!",
    "80 POINTS!"
};

// Main Function
int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Flappy Bird - OpenGL");

    // Add the blending setup here
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);
    
    // Initialize game
    srand(time(NULL));
    initParticles(); // Initialize particle system
    initPipes();
    memset(keys, 0, sizeof(keys));
    
    // Enter main loop
    glutMainLoop();
    return 0;
}

// Initialize pipes
void initPipes() {
    // Set each pipe with proper horizontal spacing
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x = WINDOW_WIDTH + (i * PIPE_SPACING);
        
        // Ensure the gap is within playable bounds
        int minGapPos = 100;  // Minimum distance from top
        int maxGapPos = WINDOW_HEIGHT - 150;  // Maximum distance from bottom (accounting for ground)
        pipes[i].gapY = minGapPos + (rand() % (maxGapPos - minGapPos));
        
        pipes[i].counted = false;
    }
    activePipes = MAX_PIPES;
}

// Reshape function
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    // Draw different screens based on game state
    switch (currentState) {
        case MENU:
            drawSky();
            drawGround();
            drawMenu();
            break;
        case INSTRUCTIONS:
            drawSky();
            drawGround();
            drawInstructions();
            break;
        case PLAYING:
            drawSky();
            drawPipes();
            drawBird();
            drawParticles();
            drawGround();
            drawScore();
            if (isCelebrating) {
                drawCelebration();
            }
            break;
        case GAME_OVER:
            drawSky();
            drawPipes();
            drawBird();
            drawParticles();
            drawGround();
            drawGameOver();
            break;
    }
    
    glutSwapBuffers();
}

// Update function
void update(int value) {
    if (currentState == PLAYING) {
        // Check for milestones
        if (score > lastMilestone) {
            if (score == 5 || score == 10 || score == 20 || score == 40 || score == 80) {
                lastMilestone = score;
                isCelebrating = true;
                celebrationTimer = CELEBRATION_DURATION;
                // Create celebration particles
                for (int i = 0; i < 50; i++) {
                    int colorIdx = i % 5;
                    createParticles(birdX, birdY, 
                                  celebrationColors[colorIdx][0],
                                  celebrationColors[colorIdx][1],
                                  celebrationColors[colorIdx][2]);
                }
            }
        }

        // Update celebration timer
        if (isCelebrating) {
            celebrationTimer -= 0.016f;  // Assuming 60 FPS
            if (celebrationTimer <= 0) {
                isCelebrating = false;
            }
        }

        // Update wing animation
        birdWingAngle += birdWingSpeed * (wingDirection ? 1 : -1);
        if (birdWingAngle > 45) wingDirection = false;
        if (birdWingAngle < -45) wingDirection = true;
        
        // Create trail particles
        if (rand() % 3 == 0) {
            createParticles(birdX - BIRD_SIZE, birdY, 1.0f, 1.0f, 0.8f);
        }
        
        // Update bird position
        birdVelocity += GRAVITY;
        birdY += birdVelocity;
        
        // Update bird rotation
        birdRotation = birdVelocity * 3;
        if (birdRotation > 60) birdRotation = 60;
        if (birdRotation < -60) birdRotation = -60;
        
        // Move pipes
        for (int i = 0; i < MAX_PIPES; i++) {
            pipes[i].x -= PIPE_SPEED;
            
            // Check if bird passed a pipe
            if (!pipes[i].counted && pipes[i].x + PIPE_WIDTH < birdX) {
                score++;
                pipes[i].counted = true;
                
                // Update high score
                if (score > highScore) {
                    highScore = score;
                }
            }
            
            // Reset pipe if it goes off screen
            if (pipes[i].x + PIPE_WIDTH < 0) {
                // Find the rightmost pipe
                float rightmostX = 0;
                for (int j = 0; j < MAX_PIPES; j++) {
                    if (pipes[j].x > rightmostX) {
                        rightmostX = pipes[j].x;
                    }
                }
                
                // Position the pipe after the rightmost pipe
                pipes[i].x = rightmostX + PIPE_SPACING;
                
                // Ensure the gap is within playable bounds
                int minGapPos = 100;  // Minimum distance from top
                int maxGapPos = WINDOW_HEIGHT - 150;  // Maximum distance from bottom (accounting for ground)
                pipes[i].gapY = minGapPos + (rand() % (maxGapPos - minGapPos));
                
                pipes[i].counted = false;
            }
        }
        
        // Check for collisions
        if (checkCollision() || birdY < 0 || birdY > WINDOW_HEIGHT - 50) {
            createParticles(birdX, birdY, 1.0f, 0.0f, 0.0f);
            currentState = GAME_OVER;
        }
    }
    
    // Update particles
    updateParticles();
    
    // Redisplay
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Check for collisions
bool checkCollision() {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].x < birdX + BIRD_SIZE && pipes[i].x + PIPE_WIDTH > birdX - BIRD_SIZE) {
            if (birdY - BIRD_SIZE < pipes[i].gapY - PIPE_GAP/2 || 
                birdY + BIRD_SIZE > pipes[i].gapY + PIPE_GAP/2) {
                return true;
            }
        }
    }
    
    return false;
}

// Keyboard function
void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
    
    switch (currentState) {
        case MENU:
            if (key == 13) { // Enter key
                if (menuSelection == 0) {
                    currentState = INSTRUCTIONS;
                } else {
                    exit(0);
                }
            } else if (key == 'w' || key == 'W') {
                menuSelection = 0;
            } else if (key == 's' || key == 'S') {
                menuSelection = 1;
            }
            break;
        case INSTRUCTIONS:
            if (key == 13) { // Enter key
                currentState = PLAYING;
                resetGame();
            }
            break;
        case PLAYING:
            if (key == 32) { // Space key
                birdVelocity = FLAP_VELOCITY;
                createParticles(birdX, birdY + BIRD_SIZE, 1.0f, 1.0f, 1.0f);
                birdWingAngle = -45;
                wingDirection = true;
            }
            break;
        case GAME_OVER:
            if (key == 'r' || key == 'R') {
                resetGame();
                currentState = PLAYING;
            } else if (key == 'q' || key == 'Q') {
                currentState = MENU;
            }
            break;
    }
}

// Keyboard up function
void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

// Special keys function
void specialKeys(int key, int x, int y) {
    switch (currentState) {
        case MENU:
            if (key == GLUT_KEY_UP) {
                menuSelection = 0;
            } else if (key == GLUT_KEY_DOWN) {
                menuSelection = 1;
            }
            break;
        case PLAYING:
            if (key == GLUT_KEY_UP) {
                birdVelocity = FLAP_VELOCITY;
            }
            break;
    }
}

// Draw bird
void drawBird() {
    glPushMatrix();
    glTranslatef(birdX, birdY, 0);
    glRotatef(birdRotation, 0, 0, 1);
    
    // Enhanced shadow with blur effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < 3; i++) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.1f - (i * 0.03f));
        glBegin(GL_POLYGON);
        for (int j = 0; j < 360; j += 36) {
            float angle = j * 3.14159 / 180;
            float radius = BIRD_SIZE + (i * 2);
            glVertex2f(radius * cos(angle) * 1.2, radius * sin(angle));
        }
        glEnd();
    }
    glDisable(GL_BLEND);
    
    // Enhanced bird body with dynamic gradient
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 36) {
        float angle = i * 3.14159 / 180;
        float t = (sin(angle) + 1) / 2.0f;
        float pulse = 0.1f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f);
        glColor3f(
            birdGradient.top[0] * (t + pulse) + birdGradient.bottom[0] * (1-t),
            birdGradient.top[1] * (t + pulse) + birdGradient.bottom[1] * (1-t),
            birdGradient.top[2] * (t + pulse) + birdGradient.bottom[2] * (1-t)
        );
        glVertex2f(BIRD_SIZE * cos(angle) * 1.2, BIRD_SIZE * sin(angle));
    }
    glEnd();
    
    // Enhanced animated wings with dynamic scaling
    glPushMatrix();
    glTranslatef(-BIRD_SIZE * 0.2, 0, 0);
    float wingAngle = sin(birdWingAngle) * 45.0f;  // Increased angle range
    glRotatef(wingAngle, 0, 0, 1);
    
    // Update wing scale
    if (wingScaleDirection) {
        birdWingScale += birdWingScaleSpeed;
        if (birdWingScale > 1.2f) wingScaleDirection = false;
    } else {
        birdWingScale -= birdWingScaleSpeed;
        if (birdWingScale < 0.8f) wingScaleDirection = true;
    }
    
    glScalef(birdWingScale, 1.0f, 1.0f);
    
    // Draw wings with enhanced gradient and highlight
    glBegin(GL_TRIANGLES);
    // Left wing
    glColor3f(birdGradient.top[0], birdGradient.top[1], birdGradient.top[2]);
    glVertex2f(0, 0);
    glColor3f(birdGradient.bottom[0], birdGradient.bottom[1], birdGradient.bottom[2]);
    glVertex2f(-BIRD_SIZE * 1.5, -BIRD_SIZE);
    glVertex2f(-BIRD_SIZE * 1.5, BIRD_SIZE);
    
    // Right wing
    glColor3f(birdGradient.top[0], birdGradient.top[1], birdGradient.top[2]);
    glVertex2f(0, 0);
    glColor3f(birdGradient.bottom[0], birdGradient.bottom[1], birdGradient.bottom[2]);
    glVertex2f(-BIRD_SIZE * 1.5, -BIRD_SIZE);
    glVertex2f(-BIRD_SIZE * 1.5, BIRD_SIZE);
    glEnd();
    
    // Add wing highlights
    glColor3f(birdHighlightColor[0], birdHighlightColor[1], birdHighlightColor[2]);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(-BIRD_SIZE * 1.2, -BIRD_SIZE * 0.5);
    glVertex2f(-BIRD_SIZE * 1.5, 0);
    glVertex2f(-BIRD_SIZE * 1.2, BIRD_SIZE * 0.5);
    glVertex2f(0, 0);
    glEnd();
    
    glPopMatrix();
    
    // Enhanced bird eye with dynamic highlight
    glPushMatrix();
    float eyePulse = 0.1f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f);
    
    // Eye base
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 36) {
        float angle = i * 3.14159 / 180;
        glVertex2f(BIRD_SIZE * 0.7 + 5 * cos(angle), -5 + 5 * sin(angle));
    }
    glEnd();
    
    // Dynamic eye highlight
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 36) {
        float angle = i * 3.14159 / 180;
        glVertex2f(BIRD_SIZE * 0.7 + 2 * cos(angle), -6 + 2 * sin(angle));
    }
    glEnd();
    
    // Pupil with dynamic movement
    float pupilOffset = 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 36) {
        float angle = i * 3.14159 / 180;
        glVertex2f(BIRD_SIZE * 0.7 + 1.5 * cos(angle) + pupilOffset, 
                  -5 + 1.5 * sin(angle) + pupilOffset);
    }
    glEnd();
    glPopMatrix();
    
    // Enhanced beak with gradient and highlight
    glBegin(GL_TRIANGLES);
    // Beak gradient
    glColor3f(1.0f, 0.7f, 0.0f);
    glVertex2f(BIRD_SIZE * 1.2, 0);
    glColor3f(0.8f, 0.4f, 0.0f);
    glVertex2f(BIRD_SIZE * 0.8, -10);
    glVertex2f(BIRD_SIZE * 0.8, 10);
    glEnd();
    
    // Beak highlight
    glColor3f(1.0f, 0.9f, 0.3f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(BIRD_SIZE * 0.8, -8);
    glVertex2f(BIRD_SIZE * 1.1, 0);
    glVertex2f(BIRD_SIZE * 0.8, 8);
    glEnd();
    
    glPopMatrix();
}

// Draw pipes
void drawPipes() {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].x < WINDOW_WIDTH && pipes[i].x + PIPE_WIDTH > 0) {
            // Enhanced pipe shadows with depth
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Draw multiple shadow layers for depth effect
            for (int s = 0; s < 3; s++) {
                float alpha = 0.15f - (s * 0.05f);
                float offset = s * 2.0f;
                
                // Top pipe shadow
                glColor4f(0.0f, 0.0f, 0.0f, alpha);
                glBegin(GL_QUADS);
                glVertex2f(pipes[i].x + offset, 0);
                glVertex2f(pipes[i].x + PIPE_WIDTH + offset, 0);
                glVertex2f(pipes[i].x + PIPE_WIDTH + offset, 
                          pipes[i].gapY - PIPE_GAP/2 + offset);
                glVertex2f(pipes[i].x + offset, 
                          pipes[i].gapY - PIPE_GAP/2 + offset);
                glEnd();
                
                // Bottom pipe shadow
                glBegin(GL_QUADS);
                glVertex2f(pipes[i].x + offset, 
                          pipes[i].gapY + PIPE_GAP/2 + offset);
                glVertex2f(pipes[i].x + PIPE_WIDTH + offset, 
                          pipes[i].gapY + PIPE_GAP/2 + offset);
                glVertex2f(pipes[i].x + PIPE_WIDTH + offset, WINDOW_HEIGHT);
                glVertex2f(pipes[i].x + offset, WINDOW_HEIGHT);
                glEnd();
            }
            
            glDisable(GL_BLEND);
            
            // Draw pipes with enhanced 3D effect
            // Top pipe
            drawGradientRect(pipes[i].x, 0, 
                           pipes[i].x + PIPE_WIDTH, pipes[i].gapY - PIPE_GAP/2,
                           pipeGradient.top, pipeGradient.bottom);
            
            // Bottom pipe
            drawGradientRect(pipes[i].x, pipes[i].gapY + PIPE_GAP/2,
                           pipes[i].x + PIPE_WIDTH, WINDOW_HEIGHT,
                           pipeGradient.top, pipeGradient.bottom);
            
            // Enhanced pipe caps with 3D effect
            GLfloat capTop[] = {0.180f, 0.549f, 0.372f};
            GLfloat capBottom[] = {0.180f, 0.449f, 0.372f};
            
            // Top pipe cap with highlight
            drawGradientRect(pipes[i].x - 5, pipes[i].gapY - PIPE_GAP/2 - 20,
                           pipes[i].x + PIPE_WIDTH + 5, pipes[i].gapY - PIPE_GAP/2,
                           capTop, capBottom);
            
            // Add highlight to top cap
            glColor3f(pipeHighlightColor[0], pipeHighlightColor[1], pipeHighlightColor[2]);
            glBegin(GL_LINE_STRIP);
            glVertex2f(pipes[i].x - 5, pipes[i].gapY - PIPE_GAP/2 - 20);
            glVertex2f(pipes[i].x + PIPE_WIDTH + 5, pipes[i].gapY - PIPE_GAP/2 - 20);
            glEnd();
            
            // Bottom pipe cap with shadow
            drawGradientRect(pipes[i].x - 5, pipes[i].gapY + PIPE_GAP/2,
                           pipes[i].x + PIPE_WIDTH + 5, pipes[i].gapY + PIPE_GAP/2 + 20,
                           capTop, capBottom);
            
            // Add shadow to bottom cap
            glColor3f(pipeShadowColor[0], pipeShadowColor[1], pipeShadowColor[2]);
            glBegin(GL_LINE_STRIP);
            glVertex2f(pipes[i].x - 5, pipes[i].gapY + PIPE_GAP/2 + 20);
            glVertex2f(pipes[i].x + PIPE_WIDTH + 5, pipes[i].gapY + PIPE_GAP/2 + 20);
            glEnd();
            
            // Add pipe texture details
            glColor3f(0.1f, 0.1f, 0.1f);
            for (int j = 0; j < 3; j++) {
                float y = j * 20.0f;
                glBegin(GL_LINES);
                glVertex2f(pipes[i].x + 10, y);
                glVertex2f(pipes[i].x + PIPE_WIDTH - 10, y);
                glEnd();
            }
        }
    }
}

// Draw ground
void drawGround() {
    // Enhanced ground gradient with dynamic color shift
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float colorShift = 0.05f * sin(time * 0.3f);
    
    GLfloat dynamicTop[] = {
        groundGradient.top[0] + colorShift,
        groundGradient.top[1] + colorShift * 0.5f,
        groundGradient.top[2] + colorShift * 0.3f
    };
    
    GLfloat dynamicBottom[] = {
        groundGradient.bottom[0] + colorShift * 0.5f,
        groundGradient.bottom[1] + colorShift * 0.3f,
        groundGradient.bottom[2] + colorShift * 0.2f
    };
    
    // Draw ground gradient
    drawGradientRect(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, WINDOW_HEIGHT,
                    dynamicTop, dynamicBottom);
    
    // Add ground texture with 3D effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw grass tufts with depth
    for (int i = 0; i < WINDOW_WIDTH; i += 30) {
        // Draw grass shadow
        glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
        glBegin(GL_TRIANGLES);
        glVertex2f(i + 2, WINDOW_HEIGHT - 50);
        glVertex2f(i + 17, WINDOW_HEIGHT - 50);
        glVertex2f(i + 9.5, WINDOW_HEIGHT - 35);
        glEnd();
        
        // Draw grass with gradient
        glBegin(GL_TRIANGLES);
        glColor3f(groundHighlightColor[0], groundHighlightColor[1], groundHighlightColor[2]);
        glVertex2f(i, WINDOW_HEIGHT - 50);
        glColor3f(groundShadowColor[0], groundShadowColor[1], groundShadowColor[2]);
        glVertex2f(i + 15, WINDOW_HEIGHT - 50);
        glColor3f(groundHighlightColor[0], groundHighlightColor[1], groundHighlightColor[2]);
        glVertex2f(i + 7.5, WINDOW_HEIGHT - 35);
        glEnd();
        
        // Draw texture pattern with depth
        glColor3f(0.0f, 0.0f, 0.0f);
        for (int j = 0; j < 3; j++) {
            float alpha = 0.1f - (j * 0.03f);
            glColor4f(0.0f, 0.0f, 0.0f, alpha);
            glBegin(GL_LINES);
            glVertex2f(i, WINDOW_HEIGHT - 45 + j);
            glVertex2f(i + 15, WINDOW_HEIGHT - 45 + j);
            glEnd();
        }
    }
    
    glDisable(GL_BLEND);
}

// Draw sky
void drawSky() {
    // Enhanced sky gradient with dynamic color shift
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float colorShift = 0.1f * sin(time * 0.5f);
    
    GLfloat dynamicTop[] = {
        skyGradient.top[0] + colorShift,
        skyGradient.top[1] + colorShift * 0.5f,
        skyGradient.top[2] + colorShift * 0.2f
    };
    
    GLfloat dynamicBottom[] = {
        skyGradient.bottom[0] + colorShift * 0.5f,
        skyGradient.bottom[1] + colorShift * 0.3f,
        skyGradient.bottom[2] + colorShift * 0.1f
    };
    
    drawGradientRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT - 50,
                    dynamicTop, dynamicBottom);
    
    // Enhanced clouds with animation
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Update cloud position
    cloudOffset += cloudSpeed;
    if (cloudOffset > WINDOW_WIDTH) cloudOffset = -200;
    
    float cloudPositions[][2] = {
        {100 + cloudOffset, 100}, 
        {300 + cloudOffset, 150}, 
        {500 + cloudOffset, 80}, 
        {700 + cloudOffset, 130}
    };
    
    for (int c = 0; c < 4; c++) {
        // Draw cloud shadows with depth
        for (int s = 0; s < 3; s++) {
            float alpha = 0.15f - (s * 0.05f);
            float offset = s * 2.0f;
            
            glColor4f(0.0f, 0.0f, 0.0f, alpha);
            for (int i = 0; i < 3; i++) {
                glBegin(GL_POLYGON);
                for (int j = 0; j < 360; j += 36) {
                    float angle = j * 3.14159 / 180;
                    float x = cloudPositions[c][0] + i * 25 + 25 * cos(angle) + offset;
                    float y = cloudPositions[c][1] + 25 * sin(angle) + offset;
                    glVertex2f(x, y);
                }
                glEnd();
            }
        }
        
        // Draw clouds with enhanced 3D effect
        for (int i = 0; i < 3; i++) {
            glBegin(GL_POLYGON);
            for (int j = 0; j < 360; j += 36) {
                float angle = j * 3.14159 / 180;
                float x = cloudPositions[c][0] + i * 25 + 25 * cos(angle);
                float y = cloudPositions[c][1] + 25 * sin(angle);
                float t = (sin(angle) + 1) / 2.0f;
                float pulse = 0.05f * sin(time + c * 0.5f);
                glColor3f(
                    cloudColor[0] * (t + pulse) + cloudShadowColor[0] * (1-t),
                    cloudColor[1] * (t + pulse) + cloudShadowColor[1] * (1-t),
                    cloudColor[2] * (t + pulse) + cloudShadowColor[2] * (1-t)
                );
                glVertex2f(x, y);
            }
            glEnd();
        }
    }
    
    glDisable(GL_BLEND);
}

// Draw menu
void drawMenu() {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 + 100);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 100);
    glEnd();
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 + 100);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 100);
    glEnd();
    
    // Enhanced menu title
    char title[] = "FLAPPY BIRD";
    renderText(WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT/2 - 70, title, textColor, true, 1.2f);
    
    // Enhanced menu options
    char newGame[] = "New Game";
    char exit[] = "Exit";
    
    if (menuSelection == 0) {
        renderText(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2, newGame, highlightColor, true, 1.1f);
        renderText(WINDOW_WIDTH/2 - 25, WINDOW_HEIGHT/2 + 40, exit, textColor, false, 1.0f);
    } else {
        renderText(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2, newGame, textColor, false, 1.0f);
        renderText(WINDOW_WIDTH/2 - 25, WINDOW_HEIGHT/2 + 40, exit, highlightColor, true, 1.1f);
    }
}

// Draw instructions
void drawInstructions() {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 - 150);
    glVertex2f(WINDOW_WIDTH/2 + 200, WINDOW_HEIGHT/2 - 150);
    glVertex2f(WINDOW_WIDTH/2 + 200, WINDOW_HEIGHT/2 + 150);
    glVertex2f(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 + 150);
    glEnd();
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 - 150);
    glVertex2f(WINDOW_WIDTH/2 + 200, WINDOW_HEIGHT/2 - 150);
    glVertex2f(WINDOW_WIDTH/2 + 200, WINDOW_HEIGHT/2 + 150);
    glVertex2f(WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT/2 + 150);
    glEnd();
    
    // Enhanced instructions title
    char title[] = "INSTRUCTIONS";
    renderText(WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 - 120, title, textColor, true, 1.2f);
    
    // Enhanced instructions text
    char line1[] = "Press UP ARROW or SPACE to flap";
    char line2[] = "Avoid hitting pipes";
    char line3[] = "Score points by passing pipes";
    char line4[] = "Press ENTER to start";
    
    renderText(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 60, line1, textColor, false, 1.0f);
    renderText(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 20, line2, textColor, false, 1.0f);
    renderText(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 20, line3, textColor, false, 1.0f);
    renderText(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 80, line4, highlightColor, true, 1.1f);
}

// Draw game over
void drawGameOver() {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 + 100);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 100);
    glEnd();
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 - 100);
    glVertex2f(WINDOW_WIDTH/2 + 150, WINDOW_HEIGHT/2 + 100);
    glVertex2f(WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 + 100);
    glEnd();
    
    // Enhanced game over title
    char title[] = "GAME OVER";
    renderText(WINDOW_WIDTH/2 - 60, WINDOW_HEIGHT/2 - 70, title, textColor, true, 1.3f);
    
    // Enhanced score display
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    renderText(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2 - 30, scoreText, textColor, true, 1.1f);
    
    // Enhanced high score display
    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", highScore);
    renderText(WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT/2, highScoreText, textColor, true, 1.1f);
    
    // Enhanced options
    char restart[] = "Press R to Restart";
    char quit[] = "Press Q to Quit";
    
    renderText(WINDOW_WIDTH/2 - 90, WINDOW_HEIGHT/2 + 40, restart, textColor, false, 1.0f);
    renderText(WINDOW_WIDTH/2 - 70, WINDOW_HEIGHT/2 + 70, quit, textColor, false, 1.0f);
}

// Draw score
void drawScore() {
    // Enhanced score display
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    renderText(10, 30, scoreText, textColor, true, 1.1f);
    
    // Enhanced high score display
    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", highScore);
    renderText(10, 60, highScoreText, textColor, true, 1.1f);
}

// Reset game
void resetGame() {
    birdY = WINDOW_HEIGHT / 2;
    birdVelocity = 0;
    birdRotation = 0;
    score = 0;
    
    // Reset pipes with proper spacing
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x = WINDOW_WIDTH + (i * PIPE_SPACING);
        
        // Ensure the gap is within playable bounds
        int minGapPos = 100;  // Minimum distance from top
        int maxGapPos = WINDOW_HEIGHT - 150;  // Maximum distance from bottom (accounting for ground)
        pipes[i].gapY = minGapPos + (rand() % (maxGapPos - minGapPos));
        
        pipes[i].counted = false;
    }
}

// Function definition without default arguments
void renderText(float x, float y, const char* text, GLfloat* color, bool isBold, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);
    
    // Draw text shadow for depth
    if (isBold) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glRasterPos2f(2.0f, -2.0f);
        for (const char* c = text; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
    
    // Draw main text
    glColor3f(color[0], color[1], color[2]);
    glRasterPos2f(0.0f, 0.0f);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    
    glPopMatrix();
}

// Draw celebration
void drawCelebration() {
    if (!isCelebrating) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Calculate milestone index
    int milestoneIndex = 0;
    if (score >= 80) milestoneIndex = 4;
    else if (score >= 40) milestoneIndex = 3;
    else if (score >= 20) milestoneIndex = 2;
    else if (score >= 10) milestoneIndex = 1;
    else if (score >= 5) milestoneIndex = 0;

    // Draw celebration message with animation
    float scale = 1.0f + 0.2f * sin(celebrationTimer * 10.0f);
    float alpha = celebrationTimer / CELEBRATION_DURATION;

    glPushMatrix();
    glTranslatef(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0);
    glScalef(scale, scale, 1.0f);

    // Draw text background
    glColor4f(0.0f, 0.0f, 0.0f, alpha * 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(-150, -50);
    glVertex2f(150, -50);
    glVertex2f(150, 50);
    glVertex2f(-150, 50);
    glEnd();

    // Draw text with rainbow effect
    const char* message = milestoneMessages[milestoneIndex];
    int messageLength = strlen(message);
    float x = -messageLength * 9;  // Approximate text width
    
    for (int i = 0; i < messageLength; i++) {
        float colorIndex = (i + celebrationTimer * 5) / 5.0f;
        int colorIdx = ((int)colorIndex) % 5;
        glColor4f(celebrationColors[colorIdx][0],
                 celebrationColors[colorIdx][1],
                 celebrationColors[colorIdx][2],
                 alpha);
        glRasterPos2f(x + i * 18, -10);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
    }

    glPopMatrix();
    glDisable(GL_BLEND);
}