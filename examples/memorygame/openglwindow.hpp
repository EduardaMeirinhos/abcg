#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>
#include <glm/vec4.hpp>
#include <string>
#include <random>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_program{};
  int m_viewportWidth{};
  int m_viewportHeight{};
 
  enum class GameState {Play, WinPlayer, Waiting, Starting}; // Game states
  enum class GameDificulty {Easy, Medium, Hard}; // Levels of dificulty
  int m_N{};  // Board size is m_N x m_N

  // Definition of a card
  struct Card {
    int value;
    int showing;
    float colors[9];
    GLuint m_vao;
    GLuint m_vboPositions;
    GLuint m_vboColors;
  };
  typedef struct Card Card;

  GameState m_gameState{};
  // GameDificulty m_gameDificulty{GameDificulty::Easy};
  Card cards[16]; // Cards in the game
  int qntShowing {0}; // Number of cards showing on the screen 
  abcg::ElapsedTimer timer; // Amount of time showing the cards in the beggining
  abcg::ElapsedTimer universalTimer; // Game time
  std::string elapsedTimeWin{}; // Time when player wins

  void createTriangle(Card* cd, int pos); // Triangle definitions (position, color, vbo, vao...)
  void restart(GameDificulty dificulty); // Start new game 
  void checkWin(); // Check if the player won
  void checkEquals(); // Check if they are equal cards
  void update(); // Controls when cards are hidden  
  std::string convertTimeSeconds(); // Convert seconds to hours, minutes, seconds
};

#endif
