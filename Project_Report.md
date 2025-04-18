# Computer Graphics and Visualization
## Course Code: 20CS510

# Mini Project Report
## Flappy Bird Game using OpenGL

### Team Members:
1. Name: [Your Name] (USN: [Your USN])
2. Name: [Team Member Name] (USN: [Team Member USN])

### Submitted to:
[Professor's Name]  
Department of Computer Science and Engineering  
[Your University Name]

---

## Problem Statement
To develop an interactive 2D game "Flappy Bird" using OpenGL graphics library that implements:
- Real-time graphics rendering using OpenGL
- Physics-based gameplay mechanics
- Collision detection system
- Dynamic animations and particle effects
- User interaction and score management
- Multiple game states with smooth transitions

## Implementation Details

### Development Environment
- **Operating System:** macOS
- **Graphics Library:** OpenGL with GLUT
- **Programming Language:** C++
- **Build System:** Standard C++ compiler

### Key OpenGL Functions Used:

1. **Window and Display Functions**
   - `glutInit()`: Initializes GLUT library
   - `glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)`: Sets double buffering and RGB color mode
   - `glutInitWindowSize(800, 600)`: Sets window dimensions
   - `glutCreateWindow()`: Creates the game window
   - `glutDisplayFunc()`: Registers display callback
   - `glutTimerFunc()`: Sets up game loop timing

2. **Drawing Functions**
   - `glBegin()` and `glEnd()`: Define primitive drawing blocks
   - `glVertex2f()`: Specifies vertex coordinates in 2D
   - `glColor3f()`: Sets RGB colors
   - `glColor4f()`: Sets RGBA colors with transparency
   - `glTranslatef()`: Handles object translation
   - `glRotatef()`: Manages object rotation
   - `glScalef()`: Controls object scaling
   - `glPushMatrix()` and `glPopMatrix()`: Manages transformation stack

3. **Blending and Visual Effects**
   - `glEnable(GL_BLEND)`: Enables transparency
   - `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`: Configures alpha blending
   - `glOrtho()`: Sets up 2D coordinate system
   - Custom gradient rendering for enhanced visuals

4. **Text Rendering System**
   - `glutBitmapCharacter()`: Renders text characters
   - Custom `renderText()` function with features:
     - Shadow effects
     - Color gradients
     - Scale control
     - Bold text support

### Key Game Components:

1. **Physics Engine**
   ```cpp
   #define GRAVITY 0.25
   #define FLAP_VELOCITY -5.0
   #define PIPE_SPEED 2.5

   // Bird physics update
   birdVelocity += GRAVITY;
   birdY += birdVelocity;
   birdRotation = birdVelocity * 3;
   ```

2. **Advanced Collision Detection**
   ```cpp
   bool checkCollision() {
       for (int i = 0; i < MAX_PIPES; i++) {
           if (pipes[i].x < birdX + BIRD_SIZE && 
               pipes[i].x + PIPE_WIDTH > birdX - BIRD_SIZE) {
               if (birdY - BIRD_SIZE < pipes[i].gapY - PIPE_GAP/2 || 
                   birdY + BIRD_SIZE > pipes[i].gapY + PIPE_GAP/2) {
                   return true;
               }
           }
       }
       return false;
   }
   ```

3. **Particle System**
   ```cpp
   struct Particle {
       float x, y;
       float vx, vy;
       float life;
       float r, g, b;
       float size;
       bool active;
   };
   ```
   - Implements dynamic particle effects for:
     - Bird trail
     - Collision feedback
     - Celebration effects
     - Score milestones

4. **Animation System**
   - Wing animation with dynamic angles
   - Cloud movement in background
   - Color gradient transitions
   - Celebration effects
   ```cpp
   float birdWingAngle = 0.0f;
   float birdWingSpeed = 15.0f;
   bool wingDirection = true;
   ```

5. **Game State Management**
   ```cpp
   enum GameState {
       MENU,
       INSTRUCTIONS,
       PLAYING,
       GAME_OVER
   };
   ```
   - Handles different game screens
   - Manages transitions
   - Controls game flow

### Visual Enhancement Features:

1. **Dynamic Color Gradients**
   ```cpp
   typedef struct {
       GLfloat top[3];
       GLfloat bottom[3];
   } GradientColor;
   ```
   - Sky gradient with sunset effects
   - Ground with rich earth tones
   - Bird with vibrant color scheme
   - Pipes with realistic green gradients

2. **Environmental Effects**
   - Animated clouds with shadows
   - Dynamic ground textures
   - Particle trail effects
   - Celebration animations

## Output Screenshots

[Note: Please add actual screenshots of your running game here]

1. **Main Menu Screen**
   - Shows game title
   - Play and Exit options
   - Animated background

2. **Instructions Screen**
   - Control explanations
   - Game objectives
   - Starting instructions

3. **Gameplay Screen**
   - Active bird animation
   - Scrolling pipes
   - Score display
   - Particle effects

4. **Game Over Screen**
   - Final score
   - High score
   - Restart option
   - Return to menu option

5. **Achievement Effects**
   - Milestone celebrations
   - Particle explosions
   - Score animations

## Conclusion

The Flappy Bird OpenGL implementation successfully demonstrates:

1. **Technical Achievements:**
   - Efficient use of OpenGL for 2D graphics
   - Smooth animation system
   - Robust physics implementation
   - Effective collision detection
   - Advanced particle systems

2. **Visual Accomplishments:**
   - Dynamic color gradients
   - Smooth animations
   - Particle effects
   - Environmental details

3. **Gameplay Features:**
   - Responsive controls
   - Progressive difficulty
   - Score tracking
   - Achievement system

The project effectively combines computer graphics concepts with game development principles to create an engaging and visually appealing gaming experience. The implementation showcases the versatility of OpenGL in creating 2D games while maintaining optimal performance and visual quality. 