#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>

// Define constants for array dimensions
const int ROWS = 51;     // Number of rows in the terrain data
const int COLS = 71;     // Number of columns in the terrain data
const int CENTER_X = 36; // Center of the terrain data
const int CENTER_Y = 26; // Center of the terrain data
//==============================================================================
// Land class to represent the terrain
class Land
{
public:
    Land();                                                      // Constructor
    void loadTerrainData(const std::string &filename);           // Load the terrain data from a file
    void placeLetters();                                         // Place the letters on the terrain
    void displayMap();                                           // Display the terrain map
    void mapValueToColor(double height, int &r, int &g, int &b); // Map a height to a color gradient

private:
    char name[20]; // Name of the user

    enum struct Direction // Enum to represent the direction of movement of the letter
    {
        START,   // Start position
        UP,      // Up
        DOWN,    // Down
        LEFT,    // Left
        RIGHT,   // Right
        END,     // End position
        DEFAULT, // Default value
    };

    struct TerrainPoint // Struct to represent a point in the terrain data
    {
        int height;                     // Height of the point
        bool visited;                   // Whether the point has been visited or not
        Direction goneThroughDirection; // Direction of movement of the letter

        TerrainPoint() : height(0), visited(false), goneThroughDirection(Direction::DEFAULT) {} // Default constructor

        // <= operator to compare TerrainPoints
        bool operator<=(const TerrainPoint &rhs) const
        {
            return height <= rhs.height;
        }

        // >= operator to compare TerrainPoints
        bool operator>=(const TerrainPoint &rhs) const
        {
            return height >= rhs.height;
        }
    };
    TerrainPoint terrainData[ROWS][COLS]; // Array to store the terrain data

    class Letter // Class to represent a letter on the terrain
    {
    public:
        char symbol;                 // Symbol of the letter
        int initialX, initialY;      // Initial position of the letter
        int currentX, currentY;      // Current position of the letter
        int finalX, finalY;          // Final position of the letter
        std::vector<Direction> path; // Path of the letter

        Letter() : symbol(' '), initialX(0), initialY(0), currentX(0), currentY(0), finalX(0), finalY(0), path() {} // Default constructor
    };

    std::vector<Letter> letters; // Vector to store the letters

    int findLowestHeightNeighbour(int x, int y); // Find the lowest height neighbour of a point
    void printColoredChar(TerrainPoint point);   // Print a colored character based on the height
    void calculateLetterPlacements();            // Calculate the placements of the letters
    void moveLetters();                          // Move the letters
    void displayLetterMovement();                // Display the movement of the letters
};
//==============================================================================
Land::Land() // Constructor
{
    // Initialize terrainData array
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j) // Initialize all the heights to 0
        {
            terrainData[i][j].height = 0;
        }
    }
}
//------------------------------------------------------------------------------
void Land::loadTerrainData(const std::string &filename)
{
    std::ifstream file(filename); // Open the file

    if (!file.is_open()) // Check if the file is open
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line; // String to store each line of the file
    int row = 0;      // Row number

    // Tokenize each line and store the values in the terrainData array. Each is seperated by a semicolon
    while (std::getline(file, line))
    {
        int col = 0;       // Column number
        std::string token; // String to store each token

        while (line.find(';') != std::string::npos) // While there are still tokens left
        {
            token = line.substr(0, line.find(';'));                     // Get the token
            terrainData[ROWS - row - 1][col].height = std::stoi(token); // Store the token in the array
            line.erase(0, line.find(';') + 1);                          // Erase the token from the line
            ++col;                                                      // Increment the column number
        }

        terrainData[ROWS - row - 1][col].height = std::stoi(line); // Store the last token in the array
        ++row;                                                     // Increment the row number
    }

    file.close();
}
//------------------------------------------------------------------------------
void Land::placeLetters()
{
    // std::cout << "Enter your name: ";
    // std::cin >> name;
    strcpy(name, "helo");

    // Capitalize the name
    for (int i = 0; i < strlen(name); ++i)
    {
        name[i] = toupper(name[i]); // Convert to uppercase
    }

    calculateLetterPlacements(); // Calculate the placements of the letters
    moveLetters();               // Move the letters
    displayLetterMovement();     // Display the movement of the letters
}
//------------------------------------------------------------------------------
// Function to calculate the placements of the letters
void Land::calculateLetterPlacements()
{
    std::cout << "Calculating letter placements...\n\n";

    srand(time(0)); // Seed the random number generator

    int totalLetters = 0;          // Total number of letters
    int nameLength = strlen(name); // Length of the name

    for (int i = 0; i < nameLength; ++i) // Calculate the total number of letters
    {
        totalLetters += name[i] - 'A' + 1; // Add the value of each letter to the total
    }

    double angle = rand() % 360; // Random angle

    for (int i = 0; i < nameLength; ++i) // Place each letter
    {
        Letter letter;           // Create a new letter
        letter.symbol = name[i]; // Set the symbol of the letter

        letter.initialX = CENTER_X + static_cast<int>(15 * cos(angle)); // Calculate the initial position of the letter
        letter.initialY = CENTER_Y + static_cast<int>(15 * sin(angle)); // Calculate the initial position of the letter

        letter.currentX = letter.initialX; // Set the current position of the letter
        letter.currentY = letter.initialY; // Set the current position of the letter

        letter.finalX = -1; // Set the final position of the letter
        letter.finalY = -1; // Set the final position of the letter

        letters.push_back(letter); // Add the letter to the vector

        terrainData[letter.initialY][letter.initialX].visited = true;                          // Mark the initial position as visited
        terrainData[letter.initialY][letter.initialX].goneThroughDirection = Direction::START; // Mark the initial position as the start position

        angle += rand() % 360; // Increment the angle
    }
}
//------------------------------------------------------------------------------
// Function to move the letters
void Land::moveLetters()
{
    for (Letter &letter : letters) // For each letter
    {
        int count = 0; // Counter to prevent infinite loops

        while (true) // While the letter has not reached the final position
        {

            // 0: down, 1: up, 2: left, 3: right
            int direction = findLowestHeightNeighbour(letter.currentX, letter.currentY);

            if (direction == -1 || count > 30) // If no valid direction found, stop! It is the final position
            {
                letter.finalX = letter.currentX;                                                 // Set the final position of the letter
                letter.finalY = letter.currentY;                                                 // Set the final position of the letter
                terrainData[letter.finalY][letter.finalX].visited = true;                        // Mark the final position as visited
                terrainData[letter.finalY][letter.finalX].goneThroughDirection = Direction::END; // Mark the final position as the end position
                break;                                                                           // Break out of the loop
            }

            Direction goneThroughDirection; // Direction of movement of the letter

            switch (direction) // Move the letter
            {
            case 0:
                letter.currentY--;
                goneThroughDirection = Direction::UP;
                break;
            case 1:
                letter.currentY++;
                goneThroughDirection = Direction::DOWN;
                break;
            case 2:
                letter.currentX--;
                goneThroughDirection = Direction::LEFT;
                break;
            case 3:
                letter.currentX++;
                goneThroughDirection = Direction::RIGHT;
                break;
            }

            letter.path.push_back(static_cast<Direction>(goneThroughDirection)); // Add the direction to the path of the letter

            terrainData[letter.currentY][letter.currentX].visited = true;                              // Mark the current position as visited
            terrainData[letter.currentY][letter.currentX].goneThroughDirection = goneThroughDirection; // Mark the current position as the direction of movement of the letter

            count++;
        }
    }
}
//------------------------------------------------------------------------------
// Function to find the lowest height neighbour of a point
int Land::findLowestHeightNeighbour(int x, int y)
{
    int lowestHeight = 100;
    int lowestHeightDirection = -1;

    // Check if the target position is visited
    if (terrainData[y - 1][x] <= terrainData[y][x] && terrainData[y - 1][x].height < lowestHeight) // Go up
    {
        lowestHeight = terrainData[y - 1][x].height;
        lowestHeightDirection = 0;
    }

    if (terrainData[y + 1][x] <= terrainData[y][x] && terrainData[y + 1][x].height < lowestHeight) // Go down
    {
        lowestHeight = terrainData[y + 1][x].height;
        lowestHeightDirection = 1;
    }

    if (terrainData[y][x - 1] <= terrainData[y][x] && terrainData[y][x - 1].height < lowestHeight) // Go left
    {
        lowestHeight = terrainData[y][x - 1].height;
        lowestHeightDirection = 2;
    }

    if (terrainData[y][x + 1] <= terrainData[y][x] && terrainData[y][x + 1].height < lowestHeight) // Go right
    {
        lowestHeight = terrainData[y][x + 1].height;
        lowestHeightDirection = 3;
    }

    switch (lowestHeightDirection)
    {
    case 0:
        if (terrainData[y - 1][x].visited)
            lowestHeightDirection = -1;
        break;
    case 1:
        if (terrainData[y + 1][x].visited)
            lowestHeightDirection = -1;
        break;
    case 2:
        if (terrainData[y][x - 1].visited)
            lowestHeightDirection = -1;
        break;
    case 3:
        if (terrainData[y][x + 1].visited)
            lowestHeightDirection = -1;
        break;
    }

    return lowestHeightDirection;
}
//------------------------------------------------------------------------------
// Function to display the movement of the letters
void Land::displayLetterMovement()
{
    for (const Letter &letter : letters)
    {
        std::cout << "\33[1;31m" << letter.symbol << "\33[0m";
        std::cout << ": initial: " << ROWS - letter.initialY << "," << letter.initialX;
        std::cout << "; final: " << ROWS - letter.finalY << "," << letter.finalX;
        std::cout << "; path: ";

        for (const Direction &direction : letter.path) // Print the path of the letter
        {
            switch (direction)
            {
            case Direction::UP:
                std::cout << "up;";
                break;
            case Direction::DOWN:
                std::cout << "down;";
                break;
            case Direction::LEFT:
                std::cout << "left;";
                break;
            case Direction::RIGHT:
                std::cout << "right;";
                break;
            case Direction::START:
                std::cout << "start;";
                break;
            case Direction::END:
                std::cout << "end;";
                break;
            default:
                std::perror("Invalid direction");
            }
        }

        std::cout << std::endl;
    }
    std::cout << std::endl;
}
//------------------------------------------------------------------------------
#pragma region CLI-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// Function to display the terrain map
void Land::displayMap()
{
    for (int i = 0; i < ROWS; ++i)
    {
        for (int j = 0; j < COLS; ++j)
        {
            if (j == 0) // label the rows
            {

                if (ROWS - i < 10) // if row number is less than 10, print a space before it
                {
                    if (i % 2 == 0) // Bg colored orange if i is even bg colored yellow if i is odd
                    {
                        std::cout << "\033[48;2;255;165;0m"
                                  << " "
                                  << "\033[0m";
                    }
                    else
                    {
                        std::cout << "\033[48;2;255;0;0m"
                                  << " "
                                  << "\033[0m";
                    }
                }

                if (i % 2 == 0) // Bg colored orange if i is even bg colored yellow if i is odd
                {
                    std::cout << "\033[48;2;255;165;0m"
                              << ROWS - i << "\033[0m";
                }
                else
                {
                    std::cout << "\033[48;2;255;0;0m"
                              << ROWS - i << "\033[0m";
                }
            }

            printColoredChar(terrainData[i][j]); // Print the colored character
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < COLS + 1; ++i) // label the columns
    {

        if (i < 10) // if column number is less than 10, print a space before it
        {
            if (i % 2 == 0) // Bg colored orange if i is even bg colored yellow if i is odd
            {
                std::cout << "\033[48;2;255;0;0m"
                          << " "
                          << "\033[0m";
            }
            else
            {
                std::cout << "\033[48;2;255;165;0m"
                          << " "
                          << "\033[0m";
            }
        }

        if (i % 2 == 0) // Bg colored orange if i is even bg colored yellow if i is odd
        {
            std::cout << "\033[48;2;255;0;0m"
                      << i << "\033[0m";
        }
        else
        {
            std::cout << "\033[48;2;255;165;0m"
                      << i << "\033[0m";
        }
    }
}
//------------------------------------------------------------------------------
// Function to map a height to a color gradient from red to blue
void Land::mapValueToColor(double height, int &r, int &g, int &b)
{
    // Define the min and max values for your data
    double minValue = 35;
    double maxValue = 65;

    // Normalize the height to be in the range [0, 1]
    double normalizedValue = (height - minValue) / (maxValue - minValue);

    // Map the normalized height to the RGB space
    r = static_cast<int>(255 * normalizedValue);
    g = static_cast<int>(255 * (1 - (2 * std::abs(0.5 - normalizedValue))));
    b = static_cast<int>(255 * (1 - normalizedValue));
}
//------------------------------------------------------------------------------
// Function to print a colored character based on the height
void Land::printColoredChar(TerrainPoint point) // if height is 0, it is final color, else if height is 100, it is initial color
{
    double height = point.height;

    int r, g, b;
    mapValueToColor(height, r, g, b);

    if (point.visited)
    {
        if (point.goneThroughDirection == Direction::START)
        {
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                      << " S"
                      << "\033[0m";
        }
        else if (point.goneThroughDirection == Direction::END)
        {
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                      << " E"
                      << "\033[0m";
        }
        else if (point.goneThroughDirection == Direction::UP)
        {
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                      << " ^"
                      << "\033[0m";
        }
        else if (point.goneThroughDirection == Direction::DOWN)
        {
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                      << " v"
                      << "\033[0m";
        }
        else if (point.goneThroughDirection == Direction::LEFT)
        {
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                      << " <"
                      << "\033[0m";
        }
        else if (point.goneThroughDirection == Direction::RIGHT)
        {
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                      << " >"
                      << "\033[0m";
        }
    }

    else
    {
        std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"
                  << "  "
                  << "\033[0m";
    }
}
#pragma endregion CLI-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//==============================================================================
// Main function
int main()
{
    Land land; // Create a new land object

    land.loadTerrainData("terrain.csv"); // Load the terrain data from the file
    land.placeLetters();                 // Place the letters on the terrain

    land.displayMap(); // Display the terrain map

    std::cout << "\n\n";

    // Press enter to exit
    std::cout << "Press enter to exit...";
    std::cin.get();

    return 0;
}

// References ------------------------------------------------------------------

// For the colored output:
// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
// https://en.wikipedia.org/wiki/ANSI_escape_code

// Compatibility ----------------------------------------------------------------
// Tested on Ubuntu 20.04 LTS works fine
// Tested on VSCode default terminal works fine
