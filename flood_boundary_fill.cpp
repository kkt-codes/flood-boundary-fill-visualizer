#include <GL/glut.h>
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>

// Fixed Logical Coordinate Constants for UI Layout mapping
const int WINDOW_WIDTH = 850;
const int WINDOW_HEIGHT = 600;
const int CANVAS_SIZE = 600;
const int GRID_SIZE = 40; // Increased resolution for better contrast shapes
const int CELL_SIZE = CANVAS_SIZE / GRID_SIZE;

enum FillMode { FLOOD_FILL, BOUNDARY_FILL };
enum CellState { EMPTY, BOUNDARY, FILLED, PATCH };

// Global Execution States
CellState grid[GRID_SIZE][GRID_SIZE];
FillMode currentMode = FLOOD_FILL;
CellState targetColor = EMPTY; 
int currentShape = 1;
bool isAnimating = false;
std::queue<std::pair<int, int>> fillQueue;

// Active Runtime Window Dimensions (for coordinate tracking)
int currentWindowWidth = WINDOW_WIDTH;
int currentWindowHeight = WINDOW_HEIGHT;

// High-Contrast UI Color Palette
const float COLOR_BG[3]       = {0.09f, 0.10f, 0.12f}; 
const float COLOR_GRID[3]     = {0.14f, 0.16f, 0.19f}; 
const float COLOR_BOUND[3]    = {1.00f, 1.00f, 1.00f}; // White Walls
const float COLOR_FLOOD[3]    = {0.00f, 0.80f, 0.55f}; // Emerald Green
const float COLOR_BOUNDARY[3] = {0.95f, 0.40f, 0.10f}; // Blaze Orange
const float COLOR_PATCH[3]    = {0.00f, 0.65f, 0.90f}; // Cyan Interiors
const float COLOR_TEXT[3]     = {0.90f, 0.92f, 0.95f}; 

void clearGridData() {
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            grid[r][c] = EMPTY;
        }
    }
    // Outer canvas perimeter frame layout
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[0][i] = BOUNDARY;
        grid[GRID_SIZE - 1][i] = BOUNDARY;
        grid[i][0] = BOUNDARY;
        grid[i][GRID_SIZE - 1] = BOUNDARY;
    }
    isAnimating = false;
    while(!fillQueue.empty()) fillQueue.pop();
}

void loadShape(int shapeNum) {
    clearGridData();
    currentShape = shapeNum;

    if (shapeNum == 1) {
        // SHAPE 1: Standard Room Frame (Classic Boundary vs Flood Test)
        for (int i = 10; i <= 30; i++) {
            grid[10][i] = BOUNDARY;
            grid[30][i] = BOUNDARY;
            grid[i][10] = BOUNDARY;
            grid[i][30] = BOUNDARY;
        }
    } 
    else if (shapeNum == 2) {
        // SHAPE 2: The Patch Room (Highlights Color Sensitivity)
        // Large boundary box filled with a solid interior cyan patch color.
        for (int i = 8; i <= 32; i++) {
            grid[8][i] = BOUNDARY;
            grid[32][i] = BOUNDARY;
            grid[i][8] = BOUNDARY;
            grid[i][32] = BOUNDARY;
        }
        for (int r = 9; r <= 31; r++) {
            for (int c = 9; c <= 31; c++) {
                grid[r][c] = PATCH;
            }
        }
    } 
    else if (shapeNum == 3) {
        // SHAPE 3: Divided Core (The Ultimate Contrast Test)
        // One solid white box structure containing a split room configuration.
        for (int i = 6; i <= 34; i++) {
            grid[6][i] = BOUNDARY;
            grid[34][i] = BOUNDARY;
            grid[i][6] = BOUNDARY;
            grid[i][34] = BOUNDARY;
        }
        // Top half filled with cyan patch, bottom half remains empty
        for (int r = 7; r <= 20; r++) {
            for (int c = 7; c <= 33; c++) {
                grid[r][c] = PATCH;
            }
        }
    }
}

void renderText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawUI() {
    // Structural Sidebar Divider Panel Line
    glColor3f(0.22f, 0.25f, 0.30f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(CANVAS_SIZE, 0);
        glVertex2f(CANVAS_SIZE, WINDOW_HEIGHT);
    glEnd();

    // Headers Layout
    glColor3fv(COLOR_TEXT);
    renderText(620, 560, "ALGORITHM CONTRAST");
    renderText(620, 545, "=================");

    // Active Selection Status Displays
    renderText(620, 505, "Active Shape:");
    glColor3f(0.00f, 0.65f, 0.90f);
    if (currentShape == 1)      renderText(620, 485, "-> [ 1 ] Simple Closed Room");
    else if (currentShape == 2) renderText(620, 485, "-> [ 2 ] Homogeneous Patch Box");
    else if (currentShape == 3) renderText(620, 485, "-> [ 3 ] Two-Tone Divided Core");

    glColor3fv(COLOR_TEXT);
    renderText(620, 440, "Active Mode:");
    if (currentMode == FLOOD_FILL) {
        glColor3fv(COLOR_FLOOD);
        renderText(620, 420, "[ FLOOD FILL ]");
        glColor3fv(COLOR_TEXT);
        renderText(620, 390, "Fills target matching color.");
        renderText(620, 375, "Stops at walls OR any color");
        renderText(620, 360, "change encountered.");
    } else {
        glColor3fv(COLOR_BOUNDARY);
        renderText(620, 420, "[ BOUNDARY FILL ]");
        glColor3fv(COLOR_TEXT);
        renderText(620, 390, "Plows through internal color");
        renderText(620, 375, "variations. Stops ONLY when");
        renderText(620, 360, "it collides with White walls.");
    }

    // Controls Configuration Layout
    glColor3fv(COLOR_TEXT);
    renderText(620, 280, "Controls:");
    renderText(620, 255, "F - Flood Fill Mode");
    renderText(620, 235, "B - Boundary Fill Mode");
    renderText(620, 215, "C - Reset Current Canvas");
    
    renderText(620, 165, "Select Test Shape:");
    renderText(620, 145, "1 - Clear Framing Room");
    renderText(620, 125, "2 - Full Cyan Patch");
    renderText(620, 105, "3 - Divided Core Structure");

    renderText(620, 40, "ESC - Exit System");
}

void display() {
    glClearColor(COLOR_BG[0], COLOR_BG[1], COLOR_BG[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Matrix Grid Pipeline Processing Loops
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            int x1 = c * CELL_SIZE;
            int y1 = r * CELL_SIZE;

            // Compute Color States
            if (grid[r][c] == BOUNDARY)       glColor3fv(COLOR_BOUND);
            else if (grid[r][c] == PATCH)      glColor3fv(COLOR_PATCH);
            else if (grid[r][c] == FILLED) {
                glColor3fv(currentMode == FLOOD_FILL ? COLOR_FLOOD : COLOR_BOUNDARY);
            } else glColor3fv(COLOR_BG);

            glRecti(x1, y1, x1 + CELL_SIZE, y1 + CELL_SIZE);

            // High-Performance Grid Outlines Vector Generation
            glColor3fv(COLOR_GRID);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2i(x1, y1);
                glVertex2i(x1 + CELL_SIZE, y1);
                glVertex2i(x1 + CELL_SIZE, y1 + CELL_SIZE);
                glVertex2i(x1, y1 + CELL_SIZE);
            glEnd();
        }
    }

    drawUI();
    glutSwapBuffers();
}

void stepAnimation(int value) {
    if (!isAnimating || fillQueue.empty()) {
        isAnimating = false;
        glutPostRedisplay();
        return;
    }

    // Process a batch of cells per frame loop tick to generate an energetic, fast-paced filling rhythm
    int stepsPerFrame = 4; 
    for (int step = 0; step < stepsPerFrame; step++) {
        if (fillQueue.empty()) break;

        auto [r, c] = fillQueue.front();
        fillQueue.pop();

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d];
            int nc = c + dc[d];

            if (nr >= 0 && nr < GRID_SIZE && nc >= 0 && nc < GRID_SIZE) {
                if (currentMode == FLOOD_FILL) {
                    if (grid[nr][nc] == targetColor) { 
                        grid[nr][nc] = FILLED;
                        fillQueue.push({nr, nc});
                    }
                } else if (currentMode == BOUNDARY_FILL) {
                    if (grid[nr][nc] != BOUNDARY && grid[nr][nc] != FILLED) {
                        grid[nr][nc] = FILLED;
                        fillQueue.push({nr, nc});
                    }
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, stepAnimation, 0); // ~60Hz scheduling cadence updates
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !isAnimating) {
        
        // Logical aspect ratio conversion framework to correctly resolve clicks in full-screen window configurations
        float logicalX = x * ((float)WINDOW_WIDTH / currentWindowWidth);
        float logicalY = (currentWindowHeight - y) * ((float)WINDOW_HEIGHT / currentWindowHeight);

        if (logicalX < CANVAS_SIZE) {
            int gridX = (int)(logicalX / CELL_SIZE);
            int gridY = (int)(logicalY / CELL_SIZE);

            gridX = std::max(0, std::min(gridX, GRID_SIZE - 1));
            gridY = std::max(0, std::min(gridY, GRID_SIZE - 1));

            // Prevent processing if seeding falls directly onto standard barriers
            if (grid[gridY][gridX] == BOUNDARY || grid[gridY][gridX] == FILLED) return;

            targetColor = grid[gridY][gridX];

            grid[gridY][gridX] = FILLED;
            fillQueue.push({gridY, gridX});
            isAnimating = true;

            glutTimerFunc(16, stepAnimation, 0);
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'f': case 'F': if (!isAnimating) currentMode = FLOOD_FILL; break;
        case 'b': case 'B': if (!isAnimating) currentMode = BOUNDARY_FILL; break;
        case 'c': case 'C': loadShape(currentShape); break;
        
        // Shape Isolation Selection Switches
        case '1': if (!isAnimating) loadShape(1); break;
        case '2': if (!isAnimating) loadShape(2); break;
        case '3': if (!isAnimating) loadShape(3); break;
            
        case 27: exit(0); break; // Escape Key
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    currentWindowWidth = w;
    currentWindowHeight = h;
    
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT); // Locks logical canvas resolution domain maps
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    
    // Configure default dimensions, initialize layout context engine, then scale to full screen
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Flood Fill vs Boundary Fill Contrast Visualizer");
    glutFullScreen(); 

    loadShape(1); // Set Default starting structure

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}